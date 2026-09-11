这个工程实现了一个 **基于 100MHz 系统时钟、115200bps 的 UART 接收器**，包含顶层复位控制、UART 接收核心和 testbench 激励。核心思路是：**检测起始位 → 波特率计数 → 每比特 8 次过采样 → 积分判决 → 移位拼接 → 输出有效脉冲**。

## 一、整体结构

### 1. `uart_rx_top`
顶层模块主要做两件事：

1. **计算波特率分频系数**
   ```verilog
   localparam SYSCLKHZ = 100_000_000;
   .BAUD_DIV(SYSCLKHZ / 115200 - 1)
   ```
   即根据系统时钟和波特率自动计算分频值，115200bps 下约为 `867`。

2. **产生内部复位释放信号 `rst_done`**
   - `rst_cnt` 是 16 位计数器，从 0 开始计数。
   - 当最高位 `rst_cnt[15]` 变为 1 后保持。
   - `rst_done = rst_cnt[15] && sys_rest_i`。
   - 因此外部复位有效后，要等待约 `32768` 个时钟周期，内部 UART 才释放复位，起到上电延时复位作用。

### 2. `uart_rx`
核心接收模块，负责：
- 起始位检测；
- 波特率计数；
- 每比特 8 次采样；
- 采样值积分判决；
- 串并转换；
- 输出 `uart_rvalid_o` 有效脉冲。

### 3. `tb_top_uart_rx`
仿真测试平台：
- 产生 100MHz 系统时钟；
- 产生 115200bps 波特率时钟 `bsp_clk`；
- 依次发送 `H`、`E`、`L` 三个字节；
- 每个字节发送格式为：起始位 0 → 8 位数据 LSB 先发 → 停止位 1。

---

## 二、UART 接收实现逻辑

### 1. 起始位检测
`uart_rx_i` 被连续打 5 拍，形成移位寄存器：
```verilog
uart_rx_i_r <= {uart_rx_i_r[3:0], uart_rx_i};
assign uart_rx_init = |uart_rx_i_r;
```
当 `uart_rx_init == 0`，也就是连续 5 个时钟采样到低电平，才认为检测到起始位，并置位：
```verilog
bps_start_en <= 1'b1;
```
这相当于对起始位做同步和去抖，防止毛刺误触发。

### 2. 波特率计数
`baud_div` 从 0 计数到 `BAUD_DIV-1`，产生 `bps_en`：
```verilog
assign bps_en = (baud_div == BAUD_DIV - 1'b1);
```
`bps_en` 表示一个比特周期基本结束，用于推进 `bit_cnt`。

### 3. 每比特 8 次过采样
采样点分频：
```verilog
localparam BAUD_DIV_SAMP = (BAUD_DIV / 8) - 1;
```
`baud_div` 负责一个 bit 的总周期，`samp_cnt` 负责把一个 bit 分成 8 个采样段，`cap_cnt` 统计一个 bit 内已经采样了多少次。

当：
```verilog
assign bit_cap_done = (cap_cnt == 3'd7);
```
表示当前 bit 的 8 个采样点完成。

### 4. 积分式采样判决
每个采样点根据 `uart_rx_i_r[4]` 对 `rx_bit_tmp` 做加/减：
```verilog
if(uart_rx_i_r[4] == 1'b1)
    rx_bit_tmp <= rx_bit_tmp + 1'b1;
else
    rx_bit_tmp <= rx_bit_tmp - 1'b1;
```
初始值为 15，一个 bit 采样 8 次后：
```verilog
assign bit_data = (rx_bit_tmp < 5'd15) ? 1'b0 : 1'b1;
```
也就是采样中低电平多则判 0，高电平多则判 1。这是一种简单的积分/多数表决抗噪方法。

### 5. 起始位确认
第一个 bit 采样完成后，检查起始位是否正确：
```verilog
start_check_failed <= bit_data ? 1'b1 : 1'b0;
```
如果起始位采样判决为 1，说明不是有效起始位，置失败标志。

### 6. 数据移位接收
`bit_cnt` 含义：
- 0：起始位；
- 1~8：8 个数据位；
- 9：停止位。

数据接收时：
```verilog
rx_data <= {bit_data, rx_data[7:1]};
```
即 UART 的 LSB 先接收，通过右移拼接，最终得到完整字节。

### 7. 输出有效信号
```verilog
assign uart_rx_done = (bit_cnt == 4'd9) && (baud_div == BAUD_DIV >> 1);
assign uart_rvalid_o = uart_rx_done;
```
当进入停止位，并且在停止位中间时刻，产生一个时钟周期的高电平 `uart_rvalid_o`，表示 `uart_rdata_o` 数据有效。

---

## 三、用到的关键技巧

1. **参数化波特率分频**
   顶层根据 `SYSCLKHZ / 115200 - 1` 自动计算 `BAUD_DIV`，便于修改时钟频率或波特率。

2. **上电复位延时**
   用 `rst_cnt[15]` 产生约 32768 个时钟周期的延时，保证系统稳定后再释放 UART 内部复位。

3. **输入同步与去抖**
   `uart_rx_i` 连续打 5 拍，并用 5 位或运算判断是否空闲，连续低电平才认为起始位，提高抗干扰能力。

4. **8 倍过采样**
   每个比特采样 8 次，而不是只在中间采一次，降低误码率。

5. **积分/多数表决判决**
   通过 `rx_bit_tmp` 加减计数，采样结果多数为 1 则判 1，多数为 0 则判 0，比单点采样稳定。

6. **边沿检测**
   使用 `bit_cap_done_r`、`bps_start_en_r` 检测上升沿，避免同一事件重复触发。

7. **LSB 先收的移位拼接**
   使用 `{bit_data, rx_data[7:1]}` 实现 UART 标准低位先收。

8. **提前产生 valid**
   在停止位中间就产生 `uart_rvalid_o`，而不是等整个停止位结束，提高响应速度。

9. **testbench 独立波特率时钟**
   测试平台用 `bsp_clk` 模拟 UART 发送时序，按起始位、8 数据位、停止位顺序发送数据，验证接收逻辑。

---

## 四、一句话总结

这个工程通过顶层参数计算和上电复位延时，驱动一个 UART 接收模块；接收模块连续检测起始位，使用波特率计数和 8 倍过采样，对每个比特做积分判决，再按 LSB 先收的方式移位拼接，最终在停止位中间输出有效数据和 `valid` 脉冲。核心技巧是：**参数化分频、5 拍同步去抖、8 倍过采样、积分投票判决、边沿检测和移位接收**。
`timescale 1ns / 1ns

module uart_rx#
(
    parameter integer BAUD_DIV = 10416 //波特率分频系数
)
(
    input wire clk_i, //系统时钟输入
    input wire uart_rx_rstn_i,//系统复位输入
    input wire uart_rx_i,//串口接收输入
    output wire [7:0] uart_rdata_o,//串口接收数据输出
    output wire uart_rvalid_o //串口接收数据有效输出
);

localparam BAUD_DIV_SAMP = (BAUD_DIV / 8) - 1; //采样点分频系数，7个采样点

reg [4:0] uart_rx_i_r = 5'd0;
wire uart_rx_init;
reg bps_start_en = 1'b0; //开始接收
reg [13:0] baud_div = 14'd0;//波特率分频计数器
wire bps_en;//比特率计数周期结束信号
reg [13:0] samp_cnt = 14'd0; //采样点计数器
wire samp_en; //采样点计数周期结束信号
reg [3:0] cap_cnt = 4'd0; //采样次数计数
reg [4:0] rx_bit_tmp = 5'd0; //采样临时变量
wire bit_cap_done; 
reg bit_cap_done_r = 1'b0; //备份bit_cap_done
reg start_check_done = 1'b0; //开始信号检测
reg start_check_failed = 1'b0; //开始信号检测失败
reg bps_start_en_r = 1'b0; //备份bps_start_en
wire bit_data; //接收到一个bit位
reg [3:0] bit_cnt = 4'd0; //接收数据位计数器 0-9个周期 其中0为起始位，1-8为数据位，9为停止位
reg [7:0] rx_data = 8'd0; //接收到一个字节数据
wire uart_rx_done; //接受一个字节结束


assign uart_rx_init = uart_rx_i_r[4] | uart_rx_i_r[3] | uart_rx_i_r[2] 
                    | uart_rx_i_r[1] | uart_rx_i_r[0];
assign bps_en = (baud_div == (BAUD_DIV - 1'b1));
assign samp_en = (samp_cnt == (BAUD_DIV_SAMP - 1'b1));
assign bit_cap_done = (cap_cnt == 3'd7); //一个比特位接收完成
assign bit_data = (rx_bit_tmp < 5'd15) ? 1'b0 : 1'b1;
assign uart_rx_done = (bit_cnt == 4'd9) && (baud_div == BAUD_DIV >> 1) && start_check_done; //>>1 相当于除以2，表示在停止位的中间采样点接收完成
assign uart_rdata_o = rx_data;
assign uart_rvalid_o = uart_rx_done;

always @(posedge clk_i) begin //采样点计数器
    if(bps_start_en == 1'b1 && samp_cnt < BAUD_DIV_SAMP) begin
        samp_cnt <= samp_cnt + 1'b1;
    end
    else begin
        samp_cnt <= 14'd0;
    end
end

always @(posedge clk_i) //将uart_rx_i信号延迟5个时钟周期，形成一个5位的移位寄存器，作用是保证是稳定的起始位
begin
    uart_rx_i_r <= {uart_rx_i_r[3:0], uart_rx_i};
end

always @(posedge clk_i) //判断起始位
begin
    if(uart_rx_rstn_i == 1'b0 || uart_rx_done == 1'b1) begin 
        bps_start_en <= 1'b0;
    end
    else if((uart_rx_init == 1'b0) && (bps_start_en == 1'b0)) begin
        bps_start_en <= 1'b1;
    end
end

always @(posedge clk_i) begin //波特率计数
    if(bps_start_en == 1'b1 && baud_div < BAUD_DIV) begin
        baud_div <= baud_div + 1'b1;
    end
    else begin
        baud_div <= 14'd0;
    end
end

always @(posedge clk_i) begin
    if(uart_rx_rstn_i == 0 || bps_en == 1'b1 || bps_start_en == 1'b0) begin
       cap_cnt <= 4'd0;
       rx_bit_tmp <= 5'd15; //初始值偏置为1，匹配UART的空闲状态，避免在接收第一个比特位时误判
    end
    else begin
        if(samp_en == 1'b1) begin
            cap_cnt <= cap_cnt + 1'b1;
            //rx_bit_tmp <= uart_rx_i_r[4] ? rx_bit_tmp + 1'b1 : rx_bit_tmp - 1'b1; //稳定后是1，则加1，否则减1.用来判断采样点的值
            if(uart_rx_i_r[4] == 1'b1) begin
                if(rx_bit_tmp < 5'd31) 
                    rx_bit_tmp <= rx_bit_tmp + 1'b1;
            end
            else begin
                if(rx_bit_tmp > 5'd0)
                    rx_bit_tmp <= rx_bit_tmp - 1'b1;
            end
        end
    end
end

always @(posedge clk_i) begin
    bit_cap_done_r <= bit_cap_done;
end

always @(posedge clk_i) begin
    bps_start_en_r <= bps_start_en;
end

always @(posedge clk_i) begin
    if(uart_rx_rstn_i == 1'b0 || start_check_failed == 1'b1) begin
        start_check_done <= 1'b0;
        start_check_failed <= 1'b0;
    end
    else if(bps_start_en_r == 1'b0 && bps_start_en == 1'b1) begin
        start_check_done <= 1'b0;
        start_check_failed <= 1'b0;
    end
    else if((bit_cap_done_r == 1'b0 && bit_cap_done == 1'b1) && start_check_done == 1'b0) begin
        start_check_done <= 1'b1;
        start_check_failed <= bit_data ? 1'b1 : 1'b0; //如果采样点的值为1，则说明起始位检测失败
    end
end

always @(posedge clk_i) begin
    if(uart_rx_rstn_i == 1'b0 || bps_start_en == 1'b0 || uart_rx_done == 1'b1) begin
        bit_cnt <= 4'd0;
    end
    else if(bps_en == 1'b1) begin
        bit_cnt <= bit_cnt + 1'b1;
    end
end

always @(posedge clk_i) begin
    if(uart_rx_rstn_i == 1'b0 || bps_start_en == 1'b0) begin
        rx_data <= 8'd0;
    end
    else if(start_check_done == 1'b1 && (bit_cap_done_r == 1'b0 && bit_cap_done == 1'b1) && bit_cnt < 4'd9) begin
        rx_data <= {bit_data, rx_data[7:1]}; //接收数据位，低位先接收
    end
end

endmodule
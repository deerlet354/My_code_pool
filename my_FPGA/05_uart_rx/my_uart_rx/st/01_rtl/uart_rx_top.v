`timescale 1ns / 1ns

module uart_rx_top
(
	input wire sys_clk_i, //系统时钟输入
	input wire sys_rest_i,//系统复位输入
	input wire uart_rx_i,//串口接收输入
	output wire [7:0] uart_rx_data_o,//串口接收数据输出
	output wire uart_rvalid_o, //串口接收数据有效输出
	output wire rst_done
);

localparam SYSCLKHZ = 100_000_000; //系统时钟频率

reg [15:0] rst_cnt = 16'd0;

assign rst_done = ((rst_cnt[15] == 1'b1) && (sys_rest_i));

always @(posedge sys_clk_i) begin
	rst_cnt <= (rst_cnt[15] == 1'b0) ? (rst_cnt + 1'b1) : rst_cnt;
end

uart_rx#
(
	.BAUD_DIV(SYSCLKHZ / 115200 - 1) //BAUD_DIV = (系统时钟频率 / 波特率) - 1
)
uart_rx_u
(
	.clk_i(sys_clk_i), //系统时钟输入
	.uart_rx_rstn_i(rst_done),//系统复位输入
	.uart_rx_i(uart_rx_i), //uart串行数据接收
	.uart_rdata_o(uart_rx_data_o),//uart接收数据输出
	.uart_rvalid_o(uart_rvalid_o)//uart接收数据有效输出，当uart_rvalid_o为高电平时，表示uart_rdata_o数据有效
);

endmodule

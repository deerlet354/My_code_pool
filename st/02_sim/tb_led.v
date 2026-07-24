`timescale 1ns / 1ns

module tb_led();

reg t_sys_clk;
reg t_sys_rest;
wire [3:0] t_sys_out;

initial begin
	t_sys_clk <= 1'b0;
	t_sys_rest <= 1'b0;
	#100
	@(posedge t_sys_clk);//等待一个时钟上升沿
	#1//等待1ns
	t_sys_rest <= 1'b1;
end

always #5 t_sys_clk = ~t_sys_clk;

led #
(
	.CNT_MAX(9)
)
led_inst
(
	.sys_clk(t_sys_clk),
	.sys_reset(t_sys_rest),
	.sys_out(t_sys_out)
);

endmodule

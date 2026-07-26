`timescale 1ns / 1ns

module tb_top_key();

reg t_sys_clk;
reg t_sys_rest;
reg key_1 = 1'b1;


initial begin
	t_sys_clk <= 1'b0;
	t_sys_rest <= 1'b0;
	key_1 = 1'b1;
	#100;
	@(posedge t_sys_clk);
	#1;
	t_sys_rest <= 1'b1;
	#500;
	forever begin//模拟按键按下抬起的抖动
		key_1 <= 1'b1; #1000;
		key_1 <= 1'b0; #1000;
		key_1 <= 1'b1; #1000;
		key_1 <= 1'b0; #1000;
		#5000000;
		key_1 <= 1'b0; #1000;
		key_1 <= 1'b1; #1000;
		key_1 <= 1'b0; #1000;
		key_1 <= 1'b1; #1000;
		#5000000;
	end
end

always #5 t_sys_clk = ~t_sys_clk;

top_key #
(
	.CNT_MAX(1000000)
)
top_key_instence
(
	.sys_clk(t_sys_clk),
	.sys_rest(t_sys_rest),
	.key1_i(key_1),
	.sig_led()
);

endmodule


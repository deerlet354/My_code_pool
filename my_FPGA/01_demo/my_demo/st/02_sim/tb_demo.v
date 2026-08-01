`timescale 1ns / 1ns

module tb_demo();

reg t_sys_clk;
reg t_sys_reset;
wire t_sys_in;
wire t_data_in;

initial begin
	t_sys_clk <= 1'b0;
	t_sys_reset <= 1'b0;
	#100
	@(posedge t_sys_clk);
	#1
	t_sys_reset <= 1'b1;
end

demo #
(
	.CNT_MAX(9)
)
test_instance
(
	.sys_clk(t_sys_clk),
	.sys_reset(t_sys_reset),
	.sys_out(t_sys_in),
	.data_out(t_data_in)
);

/*reg d, e, f;
initial begin
	d = 1'b0;
	e = 1'b0;
	f = 1'b0;
end

always @(posedge t_sys_clk) //上升沿触发
begin
	d <= 1'b1;
	e <= d;
	f <= e;
end

reg a, b, c;
initial begin
	a = 1'b0;
	#100
	a = 1'b1;
	#100
	a = 1'b0;
	#100
	a = 1'b1;
end

always @(*) begin
	b = a;
	c = a | b;
end*/


always #20 t_sys_clk <= ~t_sys_clk;//20ns反转一次，周期40ns，模仿25MHZ时钟 1/25000000 = 40ns

endmodule

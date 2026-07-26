module top_key #
(
	parameter CNT_MAX = 32'd100_000_000
)
(
	input wire sys_clk,
	input wire sys_rest,
	input wire key1_i,
	output wire [3:0] sig_led
);

wire key1_down;

key #
(
	.SYS_CLK(CNT_MAX)
)
key1_inst
(
	.sys_clk(sys_clk),
	.sys_rest(sys_rest),
	.key_i(key1_i),
	.key_down(key1_down)
);

led #
(
	.SYS_CLK(CNT_MAX)
)
led_inst
(
	.sys_clk(sys_clk),
	.sys_rest(sys_rest),
	.key_down(key1_down),
	.led_out(sig_led)
);

endmodule
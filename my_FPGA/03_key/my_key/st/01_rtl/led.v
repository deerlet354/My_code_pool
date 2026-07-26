module led#
(
	parameter SYS_CLK = 32'd99999999
)
(
	input wire sys_clk,
	input wire sys_rest,
	input wire key_down,
	output wire [3:0] led_out
);

reg [3:0] out = 4'd0;

assign led_out = out;

always @(posedge sys_clk or negedge sys_rest) begin
	if(sys_rest == 1'b0) begin
		out <= 4'b0010;
	end
	else if(key_down == 1'b1) begin
		out <= {out[2:0],out[3]};
	end
end

endmodule
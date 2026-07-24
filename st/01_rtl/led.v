module led#
(
	parameter CNT_MAX = 32'd99999999
)
(
	input sys_clk,
	input sys_reset,
	output wire [3:0] sys_out
);

reg [31:0] cnt = 32'd0;
reg [3:0]  out = 4'd0;//按下reset约束的按键后才开始流动
//reg [3:0]  out = 4'b1000;//上电后就开始流水
wire led_signal;

assign sys_out = out;
assign led_signal = (cnt == CNT_MAX);

/*always @(posedge sys_clk or negedge sys_reset) 
begin
	if(sys_reset == 1'b0) begin
		out <= 4'b1000;
	end
	else if(cnt == CNT_MAX) begin
		//out <= {out[2:0],out[3]};
		out <= {out[0],out[3:1]};
	end
end*/

always @(posedge sys_clk or negedge sys_reset) begin
	if(sys_reset == 1'b0) begin
		out <= 4'b1000;
	end
	else if(led_signal == 1'b1) begin
		//out <= {out[0],out[3:1]};
		out <= {out[2:0],out[3]};
	end
end

always @(posedge sys_clk or negedge sys_reset) begin
	if(sys_reset == 1'b0) begin
		cnt <= 32'd0;
	end
	else if(cnt == CNT_MAX) begin
		cnt <= 32'd0;
	end
	else begin
		cnt <= cnt + 32'd1;
		
	end
end

endmodule
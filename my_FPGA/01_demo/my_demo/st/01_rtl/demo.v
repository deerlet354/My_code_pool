//`timescale 1ns / 1ns

module demo#
(
	parameter CNT_MAX = 32'd99999999
)
(
	input wire sys_clk,
	input wire sys_reset,
	output wire sys_out,
	output wire data_out
);

localparam DATA_LEN = 8;

reg [31:0] i = 32'd0;
reg [3:0] cnt = 4'd0;
reg [7:0] data = 8'hab;

assign sys_out = (i == CNT_MAX);

assign data_out = data[7];

always @(posedge sys_clk)
begin
	if(sys_reset == 1'b0) begin
		i <= 32'd0;
	end
	else if(i < CNT_MAX) begin
		i <= i + 32'd1;
	end
	else begin
		i <= 32'd0;
	end
end

always @(posedge sys_clk)
begin
	if(sys_reset == 1'b0) begin
		cnt <= 4'd0;
	end
	else if(cnt < DATA_LEN - 1) begin
		cnt <= cnt + 4'd1;
	end
	else begin
		cnt <= 4'd0;
	end
end

always @(posedge sys_clk or negedge sys_reset)
begin
	if(sys_reset == 1'b0) begin
		data <= 8'hab; //1010 1011
	end
	else if(cnt < DATA_LEN) begin
		data <= {data[6:0],1'b0};
	end
	else begin
		data <= data;
	end
end

endmodule
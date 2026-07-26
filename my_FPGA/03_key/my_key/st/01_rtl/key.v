module key#
(
	parameter SYS_CLK = 32'd100_000_000
)
(
	input wire sys_clk,
	input wire sys_rest,
	input wire key_i,
	output wire key_down,
	output wire key_up
);

localparam T25MS_CNT = SYS_CLK / 1000 * 25 - 1;//25us 25ms
localparam KEY_S0 = 2'd0; //按下开始
localparam KEY_S1 = 2'd1; //按下稳定
localparam KEY_S2 = 2'd2; //抬起开始
localparam KEY_S3 = 2'd3; //抬起结束

reg [23:0] t25ms_cnt = 24'd0;
reg [3:0] key_r = 4'hf;
wire t25ms_done;
reg [1:0] key_s = KEY_S0;
reg [1:0] key_s_r = KEY_S0;

assign t25ms_done = (t25ms_cnt == T25MS_CNT);
assign key_down = (key_s == KEY_S2 && key_s_r == KEY_S1);
assign key_up = (key_s == KEY_S0 && key_s_r == KEY_S3);

always @(posedge sys_clk) begin
	if(sys_rest == 1'b0) begin
		t25ms_cnt <= 24'd0;
	end
	else if(t25ms_cnt == T25MS_CNT) begin
		t25ms_cnt <= 24'd0;
	end
	else begin
		t25ms_cnt <= t25ms_cnt + 24'd1;
	end
end

always @(posedge sys_clk) begin
	key_r <= {key_r[2:0],key_i};
end

always @(posedge sys_clk) begin
	key_s_r <= key_s;
end

always @(posedge sys_clk) begin
	if(sys_rest == 1'b0) begin
		key_s <= KEY_S0;
	end
	else if(t25ms_done == 1'b1) begin
		case(key_s)
			KEY_S0: begin
				if(key_r[3] == 1'b0) begin
					key_s <= KEY_S1;
				end
			end
			KEY_S1: begin
				if(key_r[3] == 1'b0) begin
					key_s <= KEY_S2;
				end
			end
			KEY_S2: begin
				if(key_r[3] == 1'b1) begin
					key_s <= KEY_S3;
				end
			end
			KEY_S3: begin
				if(key_r[3] == 1'b1) begin
					key_s <= KEY_S0;
				end
			end
		endcase
	end
end

endmodule 
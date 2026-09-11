`timescale 1ns / 1ns

module tb_top_uart_rx();

  localparam BPS		    = 'd115200;				        	//波特率 单位时间内传输的码元数
  localparam CLK_FRE  	= 'd100_000_000;			      //时钟频率100MHZ
  localparam CLK_TIME   = 'd1000_000_000 / CLK_FRE;	//时钟周期
  localparam BIT_TIME   = 'd1000_000_000 / BPS;		  //波特率周期
  localparam NUM_BYTES   = 'd3;					           //发送数据的字节数

  reg sysclk_p; 	//系统时钟
  reg uart_rstn;	//复位信号
  wire rst_done_p;	//复位完成信号
  reg bsp_clk;		//波特率时钟
  reg [7:0] uart_send_data[0:2];	//发送的数据
  reg [7:0] uart_send_data_r;//发送的数据寄存器
  reg uart_tx;//将数据发送到tx端，发送到rx
  wire uart_rdata_o;//接收到的数据
  wire uart_rvalid_o;//接收到的数据有效信号
  integer i,j;

  initial
  begin
    sysclk_p <= 1'b0;
    uart_rstn <= 1'b0;
    bsp_clk <= 1'b0;
    uart_send_data_r = 7'd0;
    uart_tx = 1'b1;

    uart_send_data[0] = 8'h48; //h
    uart_send_data[1] = 8'h45; //e
    uart_send_data[2] = 8'h4c; //l

    #1000;
     @(posedge sysclk_p);
    #1;
     uart_rstn <= 1'b1;//复位完成
     @(posedge rst_done_p);
      for(i=0;i<NUM_BYTES;i=i+1) begin
        uart_send_data_r = uart_send_data[i];
        @(posedge bsp_clk);
        uart_tx <= 1'b0; //发送起始位
        for(j=0;j<8;j=j+1) begin
          @(posedge bsp_clk);
          uart_tx <= uart_send_data_r[j]; //发送数据位
        end
        @(posedge bsp_clk);
        uart_tx <= 1'b1; //发送停止位
      end
  end

  always #(CLK_TIME / 2) sysclk_p = ~sysclk_p; //系统时钟周期为10ns，频率为100MHz
  always #(BIT_TIME / 2) bsp_clk = ~bsp_clk; //产生波特率时钟，周期为8680ns，频率为115200Hz

  uart_rx_top uart_inst
  (
    .sys_clk_i(sysclk_p),
    .sys_rest_i(uart_rstn),
    .uart_rx_i(uart_tx),
    .rst_done(rst_done_p),
    .uart_rx_data_o(uart_rdata_o),
    .uart_rvalid_o(uart_rvalid_o)
  );

endmodule

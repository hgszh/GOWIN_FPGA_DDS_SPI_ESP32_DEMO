module top (
    input rst,

    input xtal_27MHz,

    output dac_sclk,
    output dac_mosi,
    output dac_cs,

    output spi_miso,
    input  spi_mosi,
    input  spi_sclk,
    input  spi_cs
);

  wire clk_2MHz, clk_72MHz, clk_216MHz;
  wire reset_p_i;
  sys_clk sys_clk_inst (
      .clkout(clk_216MHz),
      .clkoutd(clk_2MHz),
      .clkoutd3(clk_72MHz),
      .reset(!rst),
      .reset_p(reset_p_i),
      .clkin(xtal_27MHz)
  );
  assign reset_p_i = 1'b0;

  wire [31:0] pinc;
  wire [11:0] ampl;
  dac8830 dac8830_inst (
      .rst(rst),
      .pinc(pinc),  //DDS相位增量
      .ampl(ampl),  //DDS输出幅度
      .sampling_clock(clk_2MHz),  //DAC采样时钟
      .spi_clock(clk_72MHz),  //SPI模块时钟（需大于16*sampling_clock*2）
      //spi接口
      .dac_sclk(dac_sclk),
      .dac_mosi(dac_mosi),
      .dac_cs(dac_cs)
  );

  wire [48-1:0] data_send;
  wire [48-1:0] data_recv;
  wire rx_valid, tx_valid;
  assign tx_valid = !spi_cs;
  SPI_Slave #(48, 0) spi_mcu2fpga (
      // 控制/数据信号
      .i_Rst_L(rst),
      .i_Clk(clk_216MHz),  // FPGA时钟
      .o_RX_DV(rx_valid),  // 数据有效脉冲（1个时钟周期）
      .o_RX_Byte(data_recv),  // 通过MOSI接收到的字节
      .i_TX_DV(tx_valid),  // 数据有效脉冲，用于注册i_TX_Byte
      .i_TX_Byte(data_send),  // 待通过MISO串行发送的字节

      // SPI接口
      .i_SPI_Clk (spi_sclk),
      .o_SPI_MISO(spi_miso),
      .i_SPI_MOSI(spi_mosi),
      .i_SPI_CS_n(spi_cs)
  );

  spi_sync_and_decode spi_sync_inst (
      .rst(rst),
      .clk_spi(spi_sclk),
      .clk_fpga(clk_72MHz),
      .rx_valid(rx_valid),
      .data_recv(data_recv),
      .data_send(data_send),
      .pinc(pinc),
      .ampl(ampl)
  );

endmodule

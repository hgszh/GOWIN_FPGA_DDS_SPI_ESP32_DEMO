module dac8830 (
    input rst,

    input [31:0] pinc,  //DDS相位增量
    input [11:0] ampl,  //DDS输出幅度

    input sampling_clock,  //DAC采样时钟
    input spi_clock,  //SPI模块时钟（需大于16*sampling_clock*2）
    //spi接口
    output dac_sclk,
    output dac_mosi,
    output dac_cs
);


  //========================例化DDS核================================
  reg wr_dds;  //写使能
  reg [15:0] waddr_dds;  //写寄存器
  reg [31:0] wdata_dds;  //写数据
  wire outvalid;  //数据输出有效信号，未使用
  wire [15:0] dout;
  DDS_Top DDS_inst (
      .clk(sampling_clock),  //input clk
      .rstn(rst),  //input rstn
      .wr(wr_dds),  //input wr_dds
      .waddr(waddr_dds),  //input [15:0] waddr_dds
      .wdata(wdata_dds),  //input [31:0] wdata_dds
      .dout(dout),  //output [15:0] dout
      .out_valid(outvalid)  //output out_valid
  );


  //==============写入相位增量到DDS核，更新DAC输出波形的频率============
  reg [31:0] pinc_last;
  always @(posedge sampling_clock or negedge rst) begin
    if (!rst) begin
      wr_dds <= 0;
      waddr_dds <= 0;
      wdata_dds <= 0;
      pinc_last <= 0;
    end
    //如果相位增量发生变化，则写入DDS核
    else if (pinc != pinc_last) begin
      wr_dds <= 1;
      waddr_dds <= 16'h0030;
      wdata_dds <= pinc;
      pinc_last <= pinc;
    end
    else begin
      wr_dds <= 0;
      waddr_dds <= 0;
      wdata_dds <= 0;
    end
  end


  //=================DDS输出波形的12位幅度调节=======================
  // 将16位的DDS输出数据 (dout) 与12位幅度调节系数 (ampl) 相乘，得到28位的乘法结果 (dds_amplified_output)
  // 将dds_amplified_output右移12位，以重新缩放为16位的幅度调节后的输出数据 (dout_final)
  // dds_amplified_output是有符号数，为了适应单极性DAC，需要将最高位取反转换为无符号数
  wire [27:0] dds_amplified_output;
  DDS_Ampl_Multiplier dds_ampl_mult (
      .mul_a(dout),  // input [15:0] mul_a，DDS输出的原始波形数据
      .mul_b(ampl),  // input [11:0] mul_b，幅度调节系数
      .product(dds_amplified_output)  // output [27:0] product，乘法后的结果
  );
  wire [15:0] dout_final;
  assign dout_final = {!dds_amplified_output[27], dds_amplified_output[26:0]} >> 12;


  //================通过SPI将波形数据传输到DAC========================
  wire [15:0] data_recv;  //仅发送，故接收空置
  wire spi_start;
  assign spi_start = !sampling_clock;  //SPI传输启动信号，通过将采样时钟取反得到
  spi_master_transceiver #(
      .DATA_WIDTH(16)
  ) spi (
      .clk(spi_clock),
      .rst(rst),

      .spi_start(spi_start),
      .data_send(dout_final),
      .data_recv(data_recv),

      .spi_cs  (dac_cs),
      .spi_sclk(dac_sclk),
      .spi_mosi(dac_mosi),
      .spi_miso(1'b0)
  );

endmodule


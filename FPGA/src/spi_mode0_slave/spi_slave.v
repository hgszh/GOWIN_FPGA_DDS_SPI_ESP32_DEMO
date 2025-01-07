// 源自: https://github.com/nandland/spi-slave/tree/master，仅支持SPI模式0的测试
// 修改内容：
// 1. 修正SPI模式0问题：确保SPI时钟下降沿改变数据，上升沿采样数据。
// 2. 将同步SPI信号的逻辑整合到模块spi_sync_and_decode中。
//    - 这样做的原因是i_Clk频率较高，更容易准确捕获片选信号(CS)。
//    - 但由于spi_sync_and_decode模块逻辑较复杂，会增加i_Clk的负载。
//    因此在spi_sync_and_decode模块中，将SPI数据同步到一个中间频率时钟域，减少对主时钟(i_Clk)的负载。

module SPI_Slave #(
    parameter DATA_WIDTH = 'd16,
    parameter SPI_MODE   = 0
) (
    // 控制/数据信号
    input                       i_Rst_L,    // FPGA复位信号，低电平有效
    input                       i_Clk,      // FPGA时钟
    output reg                  o_RX_DV,    // 数据有效脉冲（1个时钟周期）
    output reg [DATA_WIDTH-1:0] o_RX_Byte,  // 通过MOSI接收到的字节
    input                       i_TX_DV,    // 数据有效脉冲，用于写入待发送的数据
    input      [DATA_WIDTH-1:0] i_TX_Byte,  // 待通过MISO发送的字节

    // SPI接口
    input  i_SPI_Clk,
    output o_SPI_MISO,
    input  i_SPI_MOSI,
    input  i_SPI_CS_n   // 低电平有效
);

  // SPI接口（均在SPI时钟域下运行）
  wire w_CPOL;  // 时钟极性
  wire w_CPHA;  // 时钟相位
  wire w_SPI_Clk;  // 根据设置，时钟取反或不取反
  wire w_SPI_MISO_Mux;

  localparam COUNTER_WIDTH = $clog2(DATA_WIDTH);  // 计算计数变量的位宽
  reg [COUNTER_WIDTH:0] r_RX_Bit_Count;
  reg [COUNTER_WIDTH:0] r_TX_Bit_Count;

  reg [ DATA_WIDTH-1:0] r_Temp_RX_Byte;
  reg [ DATA_WIDTH-1:0] r_RX_Byte;
  //reg r_RX_Done, r2_RX_Done, r3_RX_Done;
  reg [ DATA_WIDTH-1:0] r_TX_Byte;
  reg r_SPI_MISO_Bit, r_Preload_MISO;

  // CPOL
  assign w_CPOL = (SPI_MODE == 2) | (SPI_MODE == 3);
  // CPHA
  assign w_CPHA = (SPI_MODE == 1) | (SPI_MODE == 3);
  assign w_SPI_Clk = w_CPHA ? ~i_SPI_Clk : i_SPI_Clk;

  //对输入信号去毛刺
  reg [2:0] cnt;
  always @(posedge i_Clk or negedge i_Rst_L) begin
    if (~i_Rst_L) cnt <= 4'b0;
    else begin
      if (~i_SPI_CS_n) cnt <= 4'h0;
      else if (cnt < 3'd7) cnt <= cnt + 1'b1;
    end
  end
  wire i_SPI_CS;
  assign i_SPI_CS = cnt[2] & cnt[1] & cnt[0];

  //读取MOSI上数据
  always @(posedge w_SPI_Clk or posedge i_SPI_CS) begin
    if (i_SPI_CS) begin
      r_RX_Bit_Count <= 1'b0;
      // r_RX_Done      <= 1'b0;
      o_RX_DV        <= 1'b0;
    end
    else begin
      r_RX_Bit_Count <= r_RX_Bit_Count + 1'b1;

      // 对数据左移，同时，更新LSB
      r_Temp_RX_Byte <= {r_Temp_RX_Byte[DATA_WIDTH-1-1:0], i_SPI_MOSI};

      if (r_RX_Bit_Count == DATA_WIDTH - 1) begin
        //r_RX_Done <= 1'b1;
        o_RX_DV   <= 1'b1;
        //r_RX_Byte <= {r_Temp_RX_Byte[DATA_WIDTH-1-1:0], i_SPI_MOSI};
        o_RX_Byte <= {r_Temp_RX_Byte[DATA_WIDTH-1-1:0], i_SPI_MOSI};
      end
      else if (r_RX_Bit_Count == 3'b010) begin
        // r_RX_Done <= 1'b0;
        o_RX_DV <= 1'b1;
      end
    end
  end

  // 从SPI时钟域跨到FPGA主时钟域
  /*always @(posedge i_Clk or negedge i_Rst_L) begin
    if (~i_Rst_L) begin
      r2_RX_Done <= 1'b0;
      r3_RX_Done <= 1'b0;
      o_RX_DV    <= 1'b0;
      o_RX_Byte  <= 'b0;
    end
    else begin
      r2_RX_Done <= r_RX_Done;
      r3_RX_Done <= r2_RX_Done;

      if (r3_RX_Done == 1'b0 && r2_RX_Done == 1'b1) // 上升沿
      begin
        o_RX_DV   <= 1'b1;  // 数据有效脉冲，1个时钟周期
        o_RX_Byte <= r_RX_Byte;
      end
      else begin
        o_RX_DV <= 1'b0;
      end
    end
  end*/

  // 发送数据到MISO
  always @(negedge w_SPI_Clk or posedge i_SPI_CS) begin
    if (i_SPI_CS) begin
      r_Preload_MISO <= 1'b1;
      r_TX_Bit_Count <= DATA_WIDTH - 1 - 1;  //计数变量需减去预加载的MSB
    end
    else begin
      r_Preload_MISO <= 1'b0;
      r_TX_Bit_Count <= r_TX_Bit_Count - 1'b1;
      r_SPI_MISO_Bit <= r_TX_Byte[r_TX_Bit_Count];
    end
  end

  // 当数据有效脉冲出现，寄存待发送的数据
  always @(posedge i_Clk or negedge i_Rst_L) begin
    if (~i_Rst_L) begin
      r_TX_Byte <= 'b0;
    end
    else begin
      if (i_TX_DV) begin
        r_TX_Byte <= i_TX_Byte;
      end
    end
  end

  // 当预加载选择器为高电平时，将MISO预加载为发送数据的MSB
  // 否则只发送正常的MISO数据
  assign w_SPI_MISO_Mux = r_Preload_MISO ? r_TX_Byte[DATA_WIDTH-1] : r_SPI_MISO_Bit;

  // 当CS为高电平时，MISO处于高阻态。允许多个从设备进行通信
  assign o_SPI_MISO = i_SPI_CS ? 1'bZ : w_SPI_MISO_Mux;

endmodule


module spi_sync_and_decode (
    input  wire          rst,
    input  wire          clk_spi,    // 外部 SPI 时钟
    input  wire          clk_fpga,   // FPGA 时钟
    input  wire          rx_valid,   // SPI 时钟域的接收有效信号
    input  wire [48-1:0] data_recv,  // SPI 时钟域接收到的数据
    output reg  [48-1:0] data_send,  // FPGA 时钟域发送的数据
    output reg  [32-1:0] pinc,       // DDS 相位增量
    output reg  [12-1:0] ampl        // DDS 输出幅度
);

  localparam DDS_PINC_SET = 8'hAA, DDS_AMPL_SET = 8'hBA, COMM_CHECK = 8'hCC;
  localparam COMM_STATUS_SUCCESS = 8'h55, COMM_STATUS_FAIL = 8'hAA;

  reg rx_valid_sync1, rx_valid_sync2;
  reg [ 7:0] command;
  reg [31:0] data_pinc;
  reg [11:0] data_ampl;
  reg [ 7:0] crc_calc;

  // FPGA 时钟域逻辑
  always @(posedge clk_fpga or negedge rst) begin
    if (~rst) begin
      rx_valid_sync1 <= 1'b0;
      rx_valid_sync2 <= 1'b0;
      data_send      <= {COMM_STATUS_FAIL, 40'b0};  // 默认发送失败状态
      pinc           <= 32'b0;  // 默认值
      ampl           <= 12'b0;  // 默认值
    end
    else begin
      // 同步 rx_valid 信号
      rx_valid_sync1 <= rx_valid;
      rx_valid_sync2 <= rx_valid_sync1;

      // 检测 rx_valid 的上升沿
      if (rx_valid_sync1 && !rx_valid_sync2) begin

        // 提取指令和数据
        command   <= data_recv[47:40];
        data_pinc <= data_recv[39:8];
        data_ampl <= data_recv[19:8];

        // 如果主机请求ACK，则仅回复ACK，不执行其余逻辑
        if (command == COMM_CHECK) begin
          data_send <= data_send;
        end
        //如果是普通指令，则做相应处理
        else begin
          // 计算 CRC
          crc_calc <= crc(8'b0, data_recv[39:8]);
          if (crc_calc != data_recv[7:0]) begin
            // CRC 校验失败
            data_send <= {COMM_STATUS_FAIL, 40'b0};
          end
          else begin
            // 处理指令
            case (command)
              DDS_PINC_SET: begin
                pinc      <= data_pinc;  // 更新频率控制字
                data_send <= {COMM_STATUS_SUCCESS, 40'b0};
              end
              DDS_AMPL_SET: begin
                ampl      <= data_ampl;  // 更新幅度控制字
                data_send <= {COMM_STATUS_SUCCESS, 40'b0};
              end
              default: begin
                data_send <= {COMM_STATUS_FAIL, 40'b0};  // 无效指令
              end
            endcase
          end
        end
      end
    end
  end

  // THIS IS GENERATED VERILOG CODE.
  // https://bues.ch/h/crcgen
  // 
  // This code is Public Domain.
  // Permission to use, copy, modify, and/or distribute this software for any
  // purpose with or without fee is hereby granted.
  // 
  // THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  // WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  // MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  // SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
  // RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
  // NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
  // USE OR PERFORMANCE OF THIS SOFTWARE.

  // CRC polynomial coefficients: x^8 + x^2 + x + 1
  //                              0x7 (hex)
  // CRC width:                   8 bits
  // CRC shift direction:         left (big endian)
  // Input word width:            32 bits

function automatic [7:0] crc;
  input [7:0] crc_in;
  input [31:0] data;
  reg d0, d1, d2, d3, d4, d5, d6, d7;
  reg d8, d9, d10, d11, d12, d13, d14, d15;
  reg d16, d17, d18, d19, d20, d21, d22, d23;
  reg d24, d25, d26, d27, d28, d29, d30, d31;
  reg [7:0] crc_temp;  // 临时存储中间结果

  begin
    // 将数据位赋值给局部变量
    d0 = data[0];  d1 = data[1];  d2 = data[2];  d3 = data[3];
    d4 = data[4];  d5 = data[5];  d6 = data[6];  d7 = data[7];
    d8 = data[8];  d9 = data[9];  d10 = data[10]; d11 = data[11];
    d12 = data[12]; d13 = data[13]; d14 = data[14]; d15 = data[15];
    d16 = data[16]; d17 = data[17]; d18 = data[18]; d19 = data[19];
    d20 = data[20]; d21 = data[21]; d22 = data[22]; d23 = data[23];
    d24 = data[24]; d25 = data[25]; d26 = data[26]; d27 = data[27];
    d28 = data[28]; d29 = data[29]; d30 = data[30]; d31 = data[31];
    // 使用并行计算公式优化每一位的 CRC 逻辑
    crc_temp[0] = crc_in[4] ^ crc_in[6] ^ crc_in[7] ^ d0 ^ d6 ^ d7 ^ d8 ^
                  d12 ^ d14 ^ d16 ^ d18 ^ d19 ^ d21 ^ d23 ^ d28 ^ d30 ^ d31;
    crc_temp[1] = crc_in[0] ^ crc_in[4] ^ crc_in[5] ^ crc_in[6] ^ 
                  d0 ^ d1 ^ d6 ^ d9 ^ d12 ^ d13 ^ d14 ^ d15 ^ d16 ^ d17 ^ 
                  d18 ^ d20 ^ d21 ^ d22 ^ d23 ^ d24 ^ d28 ^ d29 ^ d30;
    crc_temp[2] = crc_in[0] ^ crc_in[1] ^ crc_in[4] ^ crc_in[5] ^ 
                  d0 ^ d1 ^ d2 ^ d6 ^ d8 ^ d10 ^ d12 ^ d13 ^ d15 ^ d17 ^ 
                  d22 ^ d24 ^ d25 ^ d28 ^ d29;
    crc_temp[3] = crc_in[1] ^ crc_in[2] ^ crc_in[5] ^ crc_in[6] ^ 
                  d1 ^ d2 ^ d3 ^ d7 ^ d9 ^ d11 ^ d13 ^ d14 ^ d16 ^ d18 ^ 
                  d23 ^ d25 ^ d26 ^ d29 ^ d30;
    crc_temp[4] = crc_in[0] ^ crc_in[2] ^ crc_in[3] ^ crc_in[6] ^ crc_in[7] ^ 
                  d2 ^ d3 ^ d4 ^ d8 ^ d10 ^ d12 ^ d14 ^ d15 ^ d17 ^ d19 ^ 
                  d24 ^ d26 ^ d27 ^ d30 ^ d31;
    crc_temp[5] = crc_in[1] ^ crc_in[3] ^ crc_in[4] ^ crc_in[7] ^ 
                  d3 ^ d4 ^ d5 ^ d9 ^ d11 ^ d13 ^ d15 ^ d16 ^ d18 ^ d20 ^ 
                  d25 ^ d27 ^ d28 ^ d31;
    crc_temp[6] = crc_in[2] ^ crc_in[4] ^ crc_in[5] ^ 
                  d4 ^ d5 ^ d6 ^ d10 ^ d12 ^ d14 ^ d16 ^ d17 ^ d19 ^ d21 ^ 
                  d26 ^ d28 ^ d29;
    crc_temp[7] = crc_in[3] ^ crc_in[5] ^ crc_in[6] ^ 
                  d5 ^ d6 ^ d7 ^ d11 ^ d13 ^ d15 ^ d17 ^ d18 ^ d20 ^ d22 ^ 
                  d27 ^ d29 ^ d30;
    crc = crc_temp;
  end
endfunction


endmodule

//mode 0
module spi_master_transceiver #(
    parameter DATA_WIDTH = 'd16
) (
    input                       clk,
    input                       rst,
    input                       spi_start,  //传输启动握手脉冲
    input      [DATA_WIDTH-1:0] data_send,
    output reg [DATA_WIDTH-1:0] data_recv,
    input                       spi_miso,
    output reg                  spi_mosi,
    output reg                  spi_sclk,
    output reg                  spi_cs
);

  reg [           1:0] state;
  reg [DATA_WIDTH-1:0] data_temp;  //暂存发送的数据
  localparam COUNTER_WIDTH = $clog2(DATA_WIDTH);  // 计算计数变量的位宽
  reg [COUNTER_WIDTH-1:0] bit_cnt_send;
  reg [COUNTER_WIDTH-1:0] bit_cnt_recv;
  localparam IDLE = 2'd0, SEND = 2'd1, RECEIVE = 2'd2;

  always @(posedge clk or negedge rst) begin
    if (!rst) begin
      state        <= 1'b0;
      data_recv    <= 1'b0;
      bit_cnt_recv <= DATA_WIDTH - 1;
      bit_cnt_send <= DATA_WIDTH - 1;
      spi_cs       <= 1'b1;
      spi_mosi     <= 1'b0;
      spi_sclk     <= 1'b0;
    end
    else begin
      case (state)
        IDLE: begin
          spi_cs <= 1'b1;  //片选拉高
          spi_sclk <= 1'b0;  //时钟拉低
          spi_mosi <= 1'b0;
          state <= IDLE;
          if (spi_start) begin
            data_temp <= data_send;  //先暂存要发送的数据，避免其中途变更
            state <= SEND;  //转至下降沿发送状态
          end
        end
        SEND: begin
          spi_cs <= 1'b0;
          spi_sclk <= 1'b0;  //下降沿
          spi_mosi <= data_temp[bit_cnt_send];
          bit_cnt_send <= bit_cnt_send - 1'b1;  //发送位计数器递减
          state <= RECEIVE;  //转至上升沿接收状态
        end
        RECEIVE: begin
          spi_sclk                <= 1'b1;  //上升沿
          data_recv[bit_cnt_recv] <= spi_miso;
          bit_cnt_recv            <= bit_cnt_recv - 1'b1;  //接收位计数器递减
          state                   <= SEND;  //转至下降沿发送状态
          if (bit_cnt_recv == 4'd0) begin
            bit_cnt_recv <= DATA_WIDTH - 1;
            bit_cnt_send <= DATA_WIDTH - 1;
            state        <= IDLE;  //转至空闲状态
          end
        end
      endcase
    end
  end

endmodule

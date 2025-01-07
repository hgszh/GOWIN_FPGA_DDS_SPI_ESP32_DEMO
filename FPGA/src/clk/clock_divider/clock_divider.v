module clock_divider #(
    parameter DIV_FACTOR = 10
) (
    input  wire clk_in,
    input  wire reset_n,
    output reg  clk_out
);
  reg [$clog2(DIV_FACTOR)-1:0] counter;

  always @(posedge clk_in or negedge reset_n) begin
    if (!reset_n) begin
      counter <= 0;
      clk_out <= 0;
    end
    else begin
      if (counter == DIV_FACTOR - 1) begin
        counter <= 0;
        clk_out <= ~clk_out;
      end
      else begin
        counter <= counter + 1;
      end
    end
  end
endmodule

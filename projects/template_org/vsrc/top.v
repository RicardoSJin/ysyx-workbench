module top(
  input clk,
  input rst,
  input a,
  input b,
  output reg f
);

  always @(posedge clk) begin
    if(!rst) f <= a ^ b;
  end

endmodule


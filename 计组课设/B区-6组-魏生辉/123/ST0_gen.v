module ST0_gen (
    input wire SST,
    output reg ST,
    input wire T_clk_3,
    input wire W_1,
    input wire W_2,
    input wire clr,
    input wire[2:0] SW
);
    always @(negedge clr,negedge T_clk_3) begin
		if (!clr) ST <= 1'b0;
        else if(SST) ST <= 1'b1;
        else if (SW == 3'b100 && W_2) ST <= 1'b0;
        else ST <= ST;
    end
endmodule
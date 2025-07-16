module controller (
    input wire [2:0]SW,
    input wire [7:0]IR,
    input wire T_clk_3,
    input wire CLR,
    input wire [2:0]W_clk,
    input wire c,
    input wire z,
    output wire SBUS,//
    output reg[3:0] SEL,
    output wire SELCTL,//
    output wire DRW,//
    output wire STOP,//
    output wire MBUS,//
    output wire LAR,//
    output wire ARINC,//
    output wire SHORT,//
    output wire MEMW,//
    output wire LIR, //
    output wire PCINC, //
    output reg [3:0]S,
    output wire CIN, //
    output wire ABUS, //
    output wire LDZ,//
    output wire LDC, //
    output wire PCADD, //
    output wire LPC, //
    output wire LONG,
    output wire M
);
    
	
    wire CONFLICT;
    wire LONG_I;
    reg CONFLICT_REG;
    reg LONG_ENTERED;
    wire POP;
    assign CONFLICT = (IR[7:4] == 4'b1001) | ( (IR[7:4] == 4'b0111) & c ) | ( (IR[7:4] == 4'b1000) & z ) | (LONG_I & !W_clk[0]);
	assign LONG_I = (IR[7:4] == 4'b0101) | (IR[7:4] == 7'b0110);
    wire SST0;
    wire ST0;
    ST0_gen ST_Gen(.SST(SST0), .ST(ST0), .T_clk_3(T_clk_3),.W_1(W_clk[0]),.W_2(W_clk[1]),.SW(SW),.clr(CLR));

	wire is_WORKING;
	assign is_WORKING = W_clk[0] | W_clk[1] | W_clk[2];
    wire is_IR;
    assign is_IR = (SW == 3'd0) & is_WORKING;
    wire SW_MEM;
    assign SW_MEM = (SW == 3'b010) | (SW == 3'b001);
	
	assign POP = (CONFLICT & !CONFLICT_REG) | (LONG_I & !LONG_ENTERED & W_clk[0]);
	
	
    assign LPC = (is_IR & (IR[7:4] == 4'b1001)) | ((SW == 3'b101) & W_clk[0]);
    assign PCADD = is_IR & (( (IR[7:4] == 4'b0111) & (c==1)  )|  ((IR[7:4] == 4'b1000) & (z==1) ) );
    assign LDC = is_IR & !POP & ( (IR[7:4] == 4'b0001) | (IR[7:4] == 4'b0100) | (IR[7:4] == 4'b0010));
    assign LDZ = !POP & is_IR & ( (IR[7:4] == 4'b0001) | (IR[7:4] == 4'b0100) | (IR[7:4] == 4'b0010) | (IR[7:4] == 4'b0011) | (IR[7:4] == 4'b1100) );
    assign ABUS = is_IR & (    IR[7:6] == 2'b00 | ((IR[7:4] == 4'b0101 | IR[7:4] == 4'b0110 ) & W_clk[1]) | (IR[7:4] == 4'b0111) | (IR[7:4] == 4'b1001) | (IR[7:4] == 4'b1010) | (IR[7:4] == 4'b1011) | (IR[7:4] == 4'b1100) | (IR[7:4] == 4'b1101)   | (W_clk[2] & IR[7:4] == 4'b0110));
    assign CIN = is_IR & !POP & (IR[7:4] == 4'b0001);
    assign PCINC = is_IR  & (!CONFLICT_REG) & (!LONG_ENTERED);
    assign LIR = PCINC;
    assign MEMW =  is_IR & W_clk[2] & (IR[7:4] == 4'b0110) | ((SW == 3'b001) & ST0 & W_clk[0] );
    assign SHORT = (SW_MEM & W_clk[0]) | ((SW == 3'b101) & W_clk[0]);
    assign ARINC =  SW_MEM & ST0 & W_clk[0];
    assign LAR = (( SW_MEM & !ST0 & W_clk[0] ) | (is_IR & (IR[7:4] == 4'b0101|IR[7:4] == 4'b0110) & W_clk[1]));
    assign MBUS = ((SW == 3'b010) & ST0 & W_clk[0]) | (is_IR & (IR[7:4] == 4'b0101) & W_clk[2]);
    assign STOP = (is_IR & (IR[7:4] == 4'b1110)) | SELCTL | ((SW == 3'b101) & W_clk[0]);
    assign DRW = (SW == 3'b100) | ( is_IR & ( (IR[7:4] == 4'b0001) | (IR[7:4] == 4'b0010) | (IR[7:4] == 4'b0011) | (IR[7:4] == 4'b0100) | (IR[7:4] == 4'b1011) | (IR[7:4] == 4'b1101) ) )  | (W_clk[2] & is_IR & (IR[7:4] == 4'b0101) );
    assign SELCTL = !is_IR;
    assign SBUS = (SW == 3'b100) | (SW == 3'b001) | ((SW == 3'b010) & !ST0) | (SW == 3'b101);
    assign SST0 = (SW_MEM & !ST0 & W_clk[0]) | ((SW == 3'b100) & !ST0 & W_clk[1]);
    assign LONG = is_IR & ( (IR[7:4] == 4'b0101) | (IR[7:4] == 4'b0110) ) & W_clk[1];
    assign M = is_IR &  ( ( (IR[7:4] == 4'b0011) | (IR[7:4] == 4'b0101 & W_clk[1]) | (IR[7:4] == 4'b0110) | (IR[7:4] == 4'b1001) | (IR[7:4] == 4'b1010) | (IR[7:4] == 4'b1011) | (IR[7:4] == 4'b1101)  ) );
    
    always @(negedge CLR,posedge T_clk_3) begin
		if(!CLR) CONFLICT_REG <= 1'b0;
		else if(CONFLICT & !CONFLICT_REG) begin
			CONFLICT_REG <= 1'b1;
		end
		else if(LONG_I & !LONG_ENTERED & W_clk[0]) begin
			LONG_ENTERED <= 1'b1;
		end
		else begin
			CONFLICT_REG <= 1'b0;
			LONG_ENTERED <= 1'b0;
		end
    
    end
    
    
    
    always @(*) begin
        if(SW == 3'b100) begin
            case ({W_clk[1:0],ST0})
                3'b010: SEL = 4'b0011;
                3'b100: SEL = 4'b0100;
                3'b011: SEL = 4'b1001;
                3'b101: SEL = 4'b1110;
                default SEL = 4'b0000;
            endcase
        end
        else if(SW == 3'b011) begin
            case (W_clk[1:0])
                2'b01: SEL = 4'b0001;
                2'b10: SEL = 4'b1011;
                default SEL = 4'b0000;
            endcase
        end
        else SEL = 4'b0000;
        if (is_IR) begin
            case (IR[7:4])
                4'b0001: S = 4'b1001;
                4'b0010: S = 4'b0110;
                4'b0011: S = 4'b1011;
                4'b0100: S = 4'b0000;
                4'b0101: S = W_clk[2] ? 4'b0000 : 4'b1010;
                4'b0110: S = W_clk[2] ? 4'b1010 : 4'b1111;
                4'b1001: S = 4'b1111;
                4'b1010: S = 4'b1010;
                4'b1011: S = 4'b1010;
                4'b1100: S = 4'b0110;
                4'b1101: S = 4'b0000;
                default: S = 4'b0000;
            endcase
        end
        else begin
            S = 4'b0000;
        end
    end
endmodule
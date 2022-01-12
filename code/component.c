/*
 * Designer:   name, student id, email address
*/

#include "minicpu.h"

/* ALU */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned* ALUresult, char* Zero)
{
	if (ALUControl == 0x0)//add
	{
		*ALUresult = A + B;
	}
	if (ALUControl == 0x1)//sub
	{
		*ALUresult = A - B;
		if (*ALUresult == 0)
		{
			*Zero = 1;
		}
	}
	if (ALUControl == 0x2)//if A < B, Z = 1; otherwise, Z = 0 (A and B are signed integers)
	{
		if ((signed)A < (signed)B)
		{
			*ALUresult = 1;
		}
		else
		{
			*ALUresult = 0;
		}
	}
	if (ALUControl == 0x3)//if A < B, Z = 1; otherwise, Z = 0 (A and B are unsigned integers)
	{
		if (A < B)
		{
			*ALUresult = 1;
		}
		else
		{
			*ALUresult = 0;
		}
	}
	if (ALUControl == 0x4)//Z = A AND B
	{
		*ALUresult = A & B;
	}
	if (ALUControl == 0x5)//Z = A OR B
	{
		*ALUresult = A | B;
	}
	if (ALUControl == 0x6)//Shift B left by 16 bits
	{
		*ALUresult = B << 16;
	}
	if (ALUControl == 0x7)//Z = NOR(A,B)
	{
		*ALUresult = ~(A | B);
	}
}

/* instruction fetch */
int instruction_fetch(unsigned PC, unsigned* Mem, unsigned* instruction)
{
	if (PC % 4 == 0) //Return 1 if an invalid instruction is encountered; otherwise, return 0.
	{
		*instruction = Mem[PC >> 2];
	}
	else 
	{
		return 1;
	}

	return 0;
}


/* instruction partition */
void instruction_partition(unsigned instruction, unsigned* op, unsigned* r1, unsigned* r2, unsigned* r3, unsigned* funct, unsigned* offset, unsigned* jsec)
{
	*op = instruction >> 26;

	*r1 = (instruction >> 21) - ((instruction >> 26) << 5);

	*r2 = (instruction >> 16) - ((instruction >> 21) << 5);

	*r3 = (instruction >> 11) - ((instruction >> 16) << 5);

	*funct = (instruction)-((instruction >> 6) << 6);

	*offset = (instruction)-((instruction >> 16) << 16);

	*jsec = (instruction)-((instruction >> 26) << 26);

}



/* instruction decode */
int instruction_decode(unsigned op, struct_controls* controls)
{
	if (op == 0x0) {//R-format
		controls->ALUOp = 7;
		controls->ALUSrc = 0;
		controls->Branch = 0;
		controls->Jump = 0;
		controls->MemRead = 2;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		controls->RegDst = 1;
		controls->RegWrite = 1;
	}
	else if (op == 0x8) {//I-format 8(00 1000) addi
		controls->ALUOp = 0;
		controls->ALUSrc = 1;
		controls->Branch = 0;
		controls->Jump = 0;
		controls->MemRead = 2;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		controls->RegDst = 0;
		controls->RegWrite = 1;
	}
	else if (op == 0x2b) {//I-format 43(10 1011) sw
		controls->ALUOp = 0;
		controls->ALUSrc = 1;
		controls->Branch = 0;
		controls->Jump = 0;
		controls->MemRead = 2;
		controls->MemtoReg = 2;
		controls->MemWrite = 1;
		controls->RegDst = 2;
		controls->RegWrite = 0;
	}
	else if (op == 0x23) {//I-format 35(10 0011) lw
		controls->ALUOp = 0;
		controls->ALUSrc = 1;
		controls->Branch = 0;
		controls->Jump = 0;
		controls->MemRead = 1;
		controls->MemtoReg = 1;
		controls->MemWrite = 0;
		controls->RegDst = 0;
		controls->RegWrite = 1;

	}
	else if (op == 0x0f) {//I-format 15(00 1111) lui
		controls->ALUOp = 6;
		controls->ALUSrc = 1;
		controls->Branch = 0;
		controls->Jump = 0;
		controls->MemRead = 2;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		controls->RegDst = 0;
		controls->RegWrite = 1;

	}
	else if (op == 0xa) {//I-format 10(00 1010) slti
		controls->ALUOp = 2;
		controls->ALUSrc = 1;
		controls->Branch = 0;
		controls->Jump = 0;
		controls->MemRead = 2;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		controls->RegDst = 0;
		controls->RegWrite = 1;

	}
	else if (op == 0xb) {//I-format 11 (00 1011) sltiu
		controls->ALUOp = 3;
		controls->ALUSrc = 1;
		controls->Branch = 0;
		controls->Jump = 0;
		controls->MemRead = 2;
		controls->MemtoReg = 0;
		controls->MemWrite = 0;
		controls->RegDst = 0;
		controls->RegWrite = 1;

	}
	else if (op == 0x4) {//I-format 4 (00 0004) beq
		controls->ALUOp = 1;
		controls->ALUSrc = 0;
		controls->Branch = 1;
		controls->Jump = 0;
		controls->MemRead = 2;
		controls->MemtoReg = 2;
		controls->MemWrite = 0;
		controls->RegDst = 2;
		controls->RegWrite = 0;

	}
	else if (op == 0x2) {//J-format 2 (00 0010) J loop
		controls->ALUOp = 0;
		controls->ALUSrc = 2;
		controls->Branch = 2;
		controls->Jump = 1;
		controls->MemRead = 2;
		controls->MemtoReg = 2;
		controls->MemWrite = 0;
		controls->RegDst = 2;
		controls->RegWrite = 0;
	}

	else return 1;	//invalid instruction
	return 0;
}

/* Read Register */
void read_register(unsigned r1, unsigned r2, unsigned* Reg, unsigned* data1, unsigned* data2)
{
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
void sign_extend(unsigned offset, unsigned* extended_value)
{
	//when shift right 15 bits, 
	//if it is 0 it is a positive number ,otherwise it is a negetive number    
	if (offset >> 15 == 0)
	{
		*extended_value = offset;
	}
	else
	{
		//unsigned num = 0xffff;
		//*extended_value = (num << 16) + offset;
		*extended_value = 0xffff0000 | offset;
	}
}

/* ALU operations */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned* ALUresult, char* Zero)
{
	switch (ALUOp) {

		//addition or don't care
	case 0:
		if (ALUSrc == 1) ALU(data1, extended_value, 0x0, ALUresult, Zero);//addi sw lw
		if (ALUSrc == 2) ALU(data1, extended_value, 0x0, ALUresult, Zero);//j-loop
		break;

		//subtraction
	case 1:
		ALU(data1, data2, 0x1, ALUresult, Zero);//beq
		break;

		//set less than
	case 2:
		ALU(data2, extended_value, 0x2, ALUresult, Zero);//slti
		break;

		//set less than unsigned
	case 3:
		ALU(data2, extended_value, 0x3, ALUresult, Zero);//sltiu
		break;

		//and 
	case 4:

		break;

		//or
	case 5:

		break;

		//shift es=xtend_value 16 bits
	case 6:
		ALU(0, extended_value, 0x6, ALUresult, Zero);//lui
		break;

		// R-type
	case 7:
		// funct = 0x20 = 32
		if (funct == 0x20)ALU(data1, data2, 0x0, ALUresult, Zero);	//add

		// funct = 0x22 = 34
		else if (funct == 0x22)ALU(data1, data2, 0x1, ALUresult, Zero);	//sub

		// funct = 0x24 = 36
		else if (funct == 0x24)ALU(data1, data2, 0x4, ALUresult, Zero);	//and

		// funct = 0x25 = 37
		else if (funct == 0x25)ALU(data1, data2, 0x5, ALUresult, Zero);	//or

		// funct = 0x2a = 42
		else if (funct == 0x2a)ALU(data1, data2, 0x2, ALUresult, Zero);	//slt

		// funct = 0x2b = 43
		else if (funct == 0x2b)ALU(data1, data2, 0x3, ALUresult, Zero);	//sltu

		//invalid funct
		else return 1;
		break;

	default:
		return 1;		//invalid ALUop
	}
	return 0;
}

/* Read / Write Memory */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned* memdata, unsigned* Mem)
{

	if (MemWrite == 1 || MemRead == 1)//lw or sw 
	{
		
		if(ALUresult % 4 != 0)//Jumping to an address that is not word-aligned (being multiple of 4)
		{
			return 1;
		}
		if (ALUresult  > 0xffff)////Accessing data or jump to address that is beyond the memory.
		{
			return  1;
		}
	}


	if (MemRead == 1)
	{
		*memdata = Mem[ALUresult >> 2];
	}
	if (MemWrite == 1)
	{
		Mem[ALUresult >> 2] = data2;
	}
	if (MemRead == 2)
	{

	}
	return 0;
}


/* Write Register */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned* Reg)
{
	if (RegWrite == 1)
	{
		if (RegDst == 0)
		{
			//write Reg[r2];
			if (MemtoReg == 0)
			{
				Reg[r2] = ALUresult;
			}
			else
			{
				Reg[r2] = memdata;
			}
		}
		else
		{
			//write Reg[r3];
			if (MemtoReg == 0)
			{
				Reg[r3] = ALUresult;
			}
			else
			{
				Reg[r3] = memdata;
			}
		}
	}
}

/* PC update */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned* PC)
{
	*PC += 4;

	if (Jump == 1)
	{
		*PC = (jsec << 2) + ((*PC >> 28) << 28);
		//0100 0000 0000 0000 0000 0000 0000 0000
	}
	else
	{
		if (Branch == 1 && Zero == 1)
		{
			*PC = *PC + (extended_value << 2);
		}
	}
}

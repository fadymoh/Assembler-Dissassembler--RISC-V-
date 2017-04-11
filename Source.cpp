#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
#include <string.h>
#include <bitset>

using namespace std;

int regs[32] = { 0 };
unsigned int pc = 0x0;

char memory[8 * 1024];	// only 8KB of memory located at address 0
bool done = false;
int get_imm(string y) {
	int multiplier = 1;
	int imm = 0;
	int index = y.length() - 1;
	while (true) {
		if (y[index] != ' ')
		if (y[index] == '-') {
			imm = ~imm + 1; //two's complement
			break;
		}
		else {
			imm += multiplier*(int(y[index]) - '0');
			multiplier *= 16;
		}
		else
			break;
		index--;
	}
	return imm;
}
/*int get_jump(string y, string x, int index)
{
string line, letters;
int imm = 0, position;
ifstream temp;
bool above = true;
temp.open("assembler.txt");
while (index != y.length())
{
if (y[index] != ' ')
letters += y[index++];
else
index++;
}
while (!temp.eof())
{
getline(temp, line);
position = line.find(letters);
if (position == string::npos)
imm += 4;
else
{
position = line.find(x);
if (position == string::npos)
break;
else above = false;
}
}
if (above)
{
imm = imm - pc;
}
else
imm = imm - pc + 4;
return imm;
}*/
int calculate(string y, int& position, char stopping_char)
{
	position++;
	int found = y.find(stopping_char, position);
	position = found;
	int reg = 0;
	int val = 0;
	int multiplier = 1;
	if (stopping_char == 'x')
		found = y.length();
	found--;
	while (true) { //converting from characters to hexa
		if (y[found] != ' ' && y[found] != 'x' && y[found] != '\n') {
			val = int(y[found]);
			if (val >= 0x61 && val <= 0x66) val = val - 87;
			else val = val - '0';
			reg += val * multiplier;
			multiplier *= 16;
		}
		else break;
		found--;
	}
	return reg;
}
void assembler_function(string x, string y, ifstream &assembler, ofstream &outfile)
{
	unsigned int opcode = 0, rd = 0, func3 = 0, rs1 = 0, rs2 = 0, func7 = 0;
	int binary = 0;
	int comas = 0;
	if (x == "add" || x == "sub" || x == "sll" || x == "slt" || x == "sltu"
		|| x == "xor" || x == "srl" || x == "sra" || x == "or" || x == "and")// R Instructions
	{
		opcode = 0x33;
		if (x == "add" || x == "sub") {
			func3 = 0x0;
			if (x == "add")
				func7 = 0;
			else
				func7 = 0x20;
		}
		else if (x == "sll") {
			func3 = 1;
			func7 = 0;
		}
		else if (x == "slt") {
			func3 = 2;
			func7 = 0;
		}
		else if (x == "sltu") {
			func3 = 3;
			func7 = 0;
		}
		else if (x == "xor") {
			func3 = 4;
			func7 = 0;
		}
		else if (x == "srl" || x == "sra") {
			func3 = 5;
			if (x == "srl")
				func7 = 0;
			else
				func7 = 0x20;
		}
		else if (x == "or") {
			func3 = 6;
			func7 = 0;
		}
		else if (x == "and") {
			func3 = 7;
			func7 = 0;
		}
		rd = calculate(y, comas, ',');
		rs1 = calculate(y, comas, ',');
		rs2 = calculate(y, comas, 'x');
		cout << dec << rd << dec << rs1 << dec << rs2 << endl;
		binary = opcode & 0x0000007F;
		binary = binary | (rd << 7);
		binary = binary | (func3 << 12);
		binary = binary | (rs1 << 15);
		binary = binary | (rs2 << 20);
		binary = binary | (func7 << 25);
	//	outfile << bitset<32>(binary);
		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "addi" || x == "slli" || x == "slti" || x == "sltiu"
		|| x == "xori" || x == "srli" || x == "srai" || x == "ori" || x == "andi") //I instructions
	{
		opcode = 0x13;
		if (x == "addi")
			func3 = 0;
		else if (x == "slti")
			func3 = 2;
		else if (x == "sltiu")
			func3 = 3;
		else if (x == "xori")
			func3 = 4;
		else if (x == "ori")
			func3 = 6;
		else if (x == "andi")
			func3 = 7;
		else if (x == "slli") {
			func3 = 1;
			func7 = 0;
		}
		else if (x == "srli") {
			func3 = 5;
			func7 = 0;
		}
		else if (x == "srai") {
			func3 = 5;
			func7 = 0x20;
		}
		rd = calculate(y, comas, ',');
		rs1 = calculate(y, comas, ',');
		int imm = calculate(y, comas, 'x');
		binary = opcode & 0x0000007F;
		binary = binary | (rd << 7);
		binary = binary | (func3 << 12);
		binary = binary | (rs1 << 15);
		if (x == "slli" || x == "srli" || x == "srai")
		{
			binary = binary | ((imm & 0x0000001F) << 20);
			binary = binary | (func7 << 25);
		}
		else
			binary = binary | (imm << 20);
		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
		//outfile << bitset<32>(binary);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "sb" || x == "sh" || x == "sw") //store instructions
	{
		opcode = 0x23;
		rs2 = calculate(y, comas, ',');
		comas = y.find('(');
		rs1 = calculate(y, comas, ')');
		comas = y.find(',');
		int imm = calculate(y, comas, '(');
		if (x == "sb")
			func3 = 0;
		else if (x == "sh")
			func3 = 1;
		else if (x == "sw")
			func3 = 2;
		binary = opcode & 0x0000007F;
		binary = binary | ((imm << 7) && 0x00000F80);
		binary = binary | (func3 << 12);
		binary = binary | (rs1 << 15);
		binary = binary | (rs2 << 20);
		binary = binary | (imm << 25);
		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
		//bitset<32> x;
		//x = bitset<32>(binary);
		//string hi = x.to_string();
		//outfile << stoi(hi);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "beq" || x == "bne" || x == "blt" || x == "bge" || x == "bltu" || x == "bgeu") //branch instruction
	{
		opcode = 0x63;
		rs1 = calculate(y, comas, ',');
		comas = y.find(',');
		rs2 = calculate(y, comas, ',');
		int imm = calculate(y, comas, 'x');
		if (x == "beq")
			func3 = 0;
		else if (x == "bne")
			func3 = 1;
		else if (x == "blt")
			func3 = 4;
		else if (x == "bge")
			func3 = 5;
		else if (x == "bltu")
			func3 = 6;
		else if (x == "bgeu")
			func3 = 7;
		binary = opcode & 0x0000007F;
		binary = binary | ((imm & 0x0000400) << 7);
		binary = binary | (((imm & 0x0000001E) >> 1) << 8);
		binary = binary | (func3 << 12);
		binary = binary | (rs1 << 15);
		binary = binary | (rs2 << 20);
		binary = binary | (((imm & 0x000007E0) >> 5) << 25) | (((imm & 0x00000800) >> 11) << 30);
		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
	//	outfile << bitset<32>(binary);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "jalr") //jalr rd, rs1, label
	{
		opcode = 0x67;
		func3 = 0;
		rd = calculate(y, comas, ',');
		rs1 = calculate(y, comas, ',');
		int imm = calculate(y, comas, 'x');
		binary = opcode & 0x0000007F;
		binary = binary | (rd << 7);
		binary = binary | (func3 << 12);
		binary = binary | (rs1 << 15);
		binary = binary | ((imm & 0x0000FFF) << 20);
		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
		//outfile << bitset<32>(binary);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "jal" || x == "j" || x == "jr") //jal rd label
	{
		opcode = 0x6F;
		rd = calculate(y, comas, ',');
		int imm = calculate(y, comas, 'x');
		binary = opcode & 0x0000007F;
		binary = binary | ((rd & 0x0000001F) << 7);
		binary = binary | ((imm & 0x000FF000));
		binary = binary | (((imm & 0x00000800) >> 11) << 20);
		binary = binary | (((imm & 0x000007FE) >> 1) << 21);
		binary = binary | (((imm & 0x00080000) >> 19) << 31);

		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
		//outfile << bitset<32>(binary);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "auipc") //auipc
	{
		opcode = 0x17;
		rd = calculate(y, comas, ',');
		int imm = calculate(y, comas, 'x');
		binary = opcode & 0x0000007F;
		binary = binary | (imm & 0xFFFFF000);
		outfile.write(reinterpret_cast<const char*> (&binary), 32);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "lui") //lui
	{
		opcode = 0x37;
		rd = calculate(y, comas, ',');
		int imm = calculate(y, comas, 'x');
		binary = opcode & 0x0000007F;
		binary = binary | (imm << 7);
		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
		//outfile << bitset<32>(binary);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "lb" || x == "lh" || x == "lw" || x == "lbu" || x == "lhu") { //lw rd, offset(xbase) rs1 is base and offset is the imm
		opcode = 0x03;
		rd = calculate(y, comas, ',');
		comas = y.find('(');
		rs1 = calculate(y, comas, ')');
		comas = y.find(',');
		int imm = calculate(y, comas, '(');
		if (x == "lb")
			func3 = 0;
		else if (x == "lh")
			func3 = 1;
		else if (x == "lw")
			func3 = 2;
		else if (x == "lbu")
			func3 = 4;
		else if (x == "lhu")
			func3 = 5;
		binary = opcode & 0x0000007F;
		binary = binary | (rd << 7);
		binary = binary | (func3 << 12);
		binary = binary | (rs1 << 15);
		binary = binary | (imm << 20);
		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
		//outfile << bitset<32>(binary);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else if (x == "ECALL") {
		opcode = 0x73;
		binary = opcode & 0x00000073;
		//outfile.write(reinterpret_cast<const char*> (&binary), 32);
	//	outfile << bitset<32>(binary);
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << binary << "\t" << x << y << endl;
	}
	else cout << "\t Unknown instruction or Label " << endl;
}

void emitError(char *s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) {
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}
void instDecExec(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;
	unsigned int instPC = pc - 4;

	regs[0] = 0;
	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	funct7 = (instWord >> 25) & 0x00007F;

	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0)); //last bit is used for sign extension
	S_imm = ((funct7 << 7) | rd) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	U_imm = (instWord & 0xFFFFF000);
	B_imm = ((rd & 0x1E)) | ((funct7 & 0x3F) << 5) | ((rd & 0x1) << 11) | (((instWord >> 31) ? 0xFFFFF000 : 0x0));
	J_imm = ((instWord & 0x7FE00000) >> 20) | ((instWord >> 20 & 0x1) << 11) | ((instWord >> 12 & 0x7F) << 12) | (((instWord >> 31) ? 0xFFF80000 : 0x0));

	printPrefix(instPC, instWord);

	if (instWord == 0) {
		done = true;
		return;
	}
	if (opcode == 0x33) {	// R Instructions
		switch (funct3) {
		case 0:
			if (funct7 == 32) {
				cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				regs[rd] = regs[rs1] - regs[rs2];
			}
			else {
				cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				regs[rd] = regs[rs1] + regs[rs2];
			}
			break;
		case 1:	cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] << regs[rs2];
			break;
		case 2:	cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			if (regs[rs1] < regs[rs2])
				regs[rd] = 1;
			else
				regs[rd] = 0;
			break;
		case 3:	cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			if (regs[rs1] == 0)
			{
				if (regs[rs2] != 0)
					regs[rd] = 1;
				else
					regs[rd] = 0;
			}
			else {
				if ((unsigned int)(regs[rs1]) < (unsigned int)(regs[rs2]))
					regs[rd] = 1;
				else
					regs[rd] = 0;
			}
			break;
		case 4:	cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = (unsigned int)regs[rs1] ^ (unsigned int)regs[rs2];
			break;
		case 5:
			if (funct7 != 32) {
				cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				regs[rd] = ((unsigned int)regs[rs1]) >> regs[rs2];
			}
			else {
				cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				regs[rd] = regs[rs1] >> (regs[rs2]);
			}
			break;
		case 6:	cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = (unsigned int)regs[rs1] | (unsigned int)regs[rs2];
			break;
		case 7:	cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = (unsigned int)regs[rs1] & (unsigned int)regs[rs2];
			break;
		default:
			cout << "\tUnkown R Instruction \n";
		}
	}
	else if (opcode == 0x73) {
		cout << "\t" << "ECALL" << endl;
		if (regs[17] == 10)
			done = true;
		else if (regs[17] == 5)
			cin >> regs[17];
		else if (regs[17] == 1)
			cout << dec << regs[10] << endl;
		else if (regs[17] == 4) {
			int temp = regs[17];
			while (memory[temp] != '0') {
				cout << memory[temp];
				temp++;
			}
			cout << endl;
		}
	}
	else if (opcode == 0x13) {	// I instructions
		switch (funct3) {
		case 0:	cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = regs[rs1] + (int)I_imm;
			break;
		case 1:	cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = regs[rs1] << (unsigned int)rs2;
			break;
		case 2:	cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if (regs[rs1]< (int)I_imm)
				regs[rd] = 1;
			else
				regs[rd] = 0;
			break;
		case 3:	cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			if ((unsigned int)regs[rs1]<  I_imm || regs[rs1] == 0)
				regs[rd] = 1;
			else
				regs[rd] = 0;
			break;
		case 4:	cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = (unsigned int)regs[rs1] ^ (int)I_imm;
			break;
		case 5:
			if (funct7 == 0) {
				cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
				regs[rd] = ((unsigned int)regs[rs1]) >> (unsigned int)rs2;
			}
			else {
				cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
				regs[rd] = (int)regs[rs1] >> rs2;
			}
			break;
		case 6:	cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = (unsigned int)regs[rs1] | (int)I_imm;
			break;
		case 7:	cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = ((unsigned int)regs[rs1]) & (int)I_imm;
			break;
		default:
			cout << "\tUnkown I Instruction \n";
		}
	}
	else if (opcode == 0x37) //LUI
	{
		cout << "\tLUI\tx" << rd << ", " << hex << "0x" << (int)U_imm << "\n";
		regs[rd] = U_imm;
	}
	else if (opcode == 0x17) // AUIPC
	{
		cout << "\tAUIPC\tx" << rd << ", " << hex << "0x" << (int)U_imm << "\n";
		regs[rd] = U_imm + instPC;
	}
	else if (opcode == 0x6F) // JAL
	{
		cout << "\tJAL\tx" << rd << ", " << hex << "0x" << (int)J_imm << "\n";
		regs[rd] = pc; //check to see if the zero register will work or not
		pc += (int)J_imm - 4;
	}
	else if (opcode == 0x67) // JALR
	{
		cout << "\tJALR\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
		regs[rd] = pc + 4; //to look at the next instruction
		pc = (I_imm + regs[rs1]) & 0xFFFFFFFE;
	}
	else if (opcode == 0x63) // Branch Instructions
	{
		switch (funct3) {
		case 0:
			cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
			if (regs[rs1] == regs[rs2])
				pc += (int)B_imm - 4;
			break;
		case 1:
			cout << "\tBNE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
			if (regs[rs1] != regs[rs2])
				pc += (int)B_imm - 4;
			break;
		case 4:
			cout << "\tBLT\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
			if ((int)regs[rs1]<(int)regs[rs2])
				pc += (int)B_imm - 4;
			break;
		case 5:
			cout << "\tBGE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
			if ((int)regs[rs1] >= (int)regs[rs2])
				pc += (int)B_imm - 4;
			break;
		case 6:
			cout << "\tBLTU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
			if (regs[rs1]<regs[rs2])
				pc += (int)B_imm - 4;
			break;
		case 7:
			cout << "\tBGEU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
			if (regs[rs1] >= regs[rs2])
				pc += (int)B_imm - 4;
			break;
		default:
			cout << "UNKOWN SB INSTRUCTION " << endl;
		}
	}
	else if (opcode == 0x3) //LOADING INSTRUCTIONS
	{
		switch (funct3) {
		case 0:
			cout << "\tLB\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = 0;
			regs[rd] = (unsigned char)memory[regs[rs1] + (int)I_imm] | ((((unsigned char)memory[regs[rs1] + (int)I_imm] >> 7) ? 0xFFFFFF00 : 0x0));
			break;
		case 1:
			cout << "\tLH\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = 0;
			regs[rd] = ((unsigned char)memory[regs[rs1] + (int)I_imm]) | ((unsigned char)memory[regs[rs1] + (int)I_imm + 1] << 8) |
				(((((unsigned char)memory[regs[rs1] + (int)I_imm + 1]) >> 15) ? 0xFFFF0000 : 0x0)); //for sign extension 
			break;
		case 2:
			cout << "\tLW\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = ((unsigned char)memory[regs[rs1] + (int)I_imm]) | (((unsigned char)memory[regs[rs1] + (int)I_imm + 1]) << 8)
				| (((unsigned char)memory[regs[rs1] + (int)I_imm + 2]) << 16)
				| (((unsigned char)memory[regs[rs1] + (int)I_imm + 3]) << 24);
			break;
		case 4:
			cout << "\tLBU\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = 0;
			regs[rd] = (unsigned char)memory[regs[rs1] + (int)I_imm];
			break;
		case 5:
			cout << "\tLHU\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = 0;
			regs[rd] = ((unsigned char)memory[regs[rs1] + (int)I_imm]) | ((unsigned char)memory[regs[rs1] + (int)I_imm + 1] << 8);
			break;
		default:
			cout << "Unknown Load Instruction" << endl;
		}
	}
	else if (opcode == 0x23) // STORE INSTRUCTIONS
	{
		switch (funct3) {
		case 0:
			cout << "\tSB\tx" << rs2 << ", " << hex << "0x" << (int)S_imm << "(x" << rs1 << ")" << "\n";
			memory[regs[rs1] + S_imm] = regs[rs2];
			break;
		case 1:
			cout << "\tSH\tx" << rs2 << ", " << hex << "0x" << (int)S_imm << "(x" << rs1 << ")" << "\n";
			memory[regs[rs1] + S_imm] = regs[rs2];
			memory[regs[rs1] + S_imm + 1] = regs[rs2] >> 8;
			break;
		case 2:
			cout << "\tSW\tx" << rs2 << ", " << hex << "0x" << (int)S_imm << "(x" << rs1 << ")" << "\n";
			memory[regs[rs1] + S_imm] = regs[rs2];
			memory[regs[rs1] + S_imm + 1] = regs[rs2] >> 8;
			memory[regs[rs1] + S_imm + 2] = regs[rs2] >> 16;
			memory[regs[rs1] + S_imm + 3] = regs[rs2] >> 24;
			break;
		default:
			cout << "Unkown Store Instruction \n" << endl;
		}
	}
	else {
		cout << "\tUnkown Instruction \n";
	}
}

int main(int argc, char *argv[]) {

	unsigned int instWord = 0;
	ifstream inFile, assembler;
	string  name = "assembler.txt", x, y;
	assembler.open(name.c_str());
	ofstream outfile("out.bin", ios::out | ios::binary);
	if (outfile.is_open())
		cout << " could not open the output" << endl;
	if (!assembler.is_open())
		cout << " could not open assembler" << endl;
	char input;
	bool assemble;
	if (argc<1) emitError("use: rv32i_sim <machine_code_file_name>\n");
	inFile.open(argv[1], ios::in | ios::binary | ios::ate);
	cout << "If you want to assemble type A\n If you want to disassembly type D\n ANY OTHER INPUT WILL TERMINATE THE PROGRAM!\n ";
	cin >> input;
	if (input == 'a' || input == 'A')
		assemble = true;
	else
	if (input == 'D' || input == 'd')
		assemble = false;
	else
		return 0;
	if (!assemble) {
		if (inFile.is_open()) {
			int fsize = inFile.tellg();
			inFile.seekg(0, inFile.beg);
			if (!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");
			while (!done) {
				instWord = (unsigned char)memory[pc] | (((unsigned char)memory[pc + 1]) << 8) | (((unsigned char)memory[pc + 2]) << 16) | (((unsigned char)memory[pc + 3]) << 24);
				pc += 4;
				instDecExec(instWord);
			}
			// dump the registers
			for (int i = 0; i < 32; i++)
				cout << "x" << dec << i << ": \t" << "0x" << hex << std::setfill('0') << std::setw(8) << regs[i] << "\n";
		}
		else emitError("Cannot access input file\n");
	}
	else
	if (assembler.is_open()) {
		while (!assembler.eof()) {
			assembler >> x;
			getline(assembler, y);
			pc += 4;
			assembler_function(x, y, assembler, outfile);
		}
	}
	else
		cout << " COULD NOT OPEN THE FILE" << endl;
	system("pause");
	return 0;
}
#pragma once

#include <cstdint>
#include <random>

// renamed unsigned integers that represent bits for clarity.
typedef uint8_t bit8;
typedef uint16_t bit16;
typedef uint32_t bit32;

/*
	CPU REGISTERS: TOTAL 16, labeled V0 to VF
		
		VF is a flag register that holds info about an operation result

	MEMORY ADDRESS SPACE: TOTAL 4096 BYTES, indexed 0x000 (0) to 0xFFF (4097)

		0x000-0x1FF: Originally reserved for the CHIP-8 interpreter; emulator will never write to or read from this area. Except for…

		0x050-0x0A0: Storage space for the 16 built-in characters (0 through F)

		0x200-0xFFF: Instructions from the ROM will be stored starting at 0x200, and anything left after the ROM’s space is free to use.
*/

class Chip8
{
public:
	//default constructor
	Chip8();

	//load ROM into memory from 
	void loadRom(char const* fileName);

	//fetch opcode, decode, next execute
	void cycle();

	//public so they can be accessed by the Display class
	bit8 keypad[16]{};
	bit32 video[64 * 32]{};
private:
	//These functions will dereference the pointer to the opcode functions for their table.
	//For example, when opcode=0x00E0, table0[(0x00E0 & 0x000F)] = table0[(0x0)], which returns a pointer to Chip8::OP_00E0
	//These tables are used because many opcodes can be grouped by their starting values: 00, 8xy, Ex, or Fx
	void Table0();
	void Table8();
	void TableE();
	void TableF();

	// Following opcode implementations are based from http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf

	// Do nothing
	void OP_NULL();

	// CLS - clears display by setting video buffer to zeroes
	void OP_00E0();

	// RET - returns from subroutine. The interpreter sets the program counter to the address at the top of the stack,
	// then subtracts 1 from the stack pointer
	void OP_00EE();

	// JP address - Jump to location nnn. The interpreter sets the program counter to nnn
	void OP_1nnn();

	// CALL address - Call subroutine at nnn. The interpreter increments the stack pointer, then puts the current PC on the top
	// of the stack. The PC is then set to nnn
	void OP_2nnn();

	// SE Vx, byte - Skip next instruction if Vx = kk. The interpreter compares register Vx to kk, and if they are equal,
	// increments the program counter by 2
	void OP_3xkk();

	// SNE Vx, byte - Skip next instruction if Vx != kk. The interpreter compares register Vx to kk, and if they are not equal,
	// increments the program counter by 2
	void OP_4xkk();

	// SE Vx, Vy - Skip next instruction if Vx = Vy. The interpreter compares register Vx to register Vy, and if they are equal,
	//increments the program counter by 2
	void OP_5xy0();

	// LD Vx, byte - Set Vx = kk. The interpreter puts the value kk into register Vx
	void OP_6xkk();

	// ADD Vx, byte - Set Vx = Vx + kk. Adds the value kk to the value of register Vx, then stores the result in Vx
	void OP_7xkk();

	// LD Vx, Vy - Set Vx = Vy. Stores the value of register Vy in register Vx
	void OP_8xy0();

	// OR Vx, Vy - Set Vx = Vx OR Vy. Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx
	void OP_8xy1();

	// AND Vx, Vy - Set Vx = Vx AND Vy. Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx
	void OP_8xy2();

	// XOR Vx, Vy - Set Vx = Vx XOR Vy. Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx
	void OP_8xy3();

	// ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry. The values of Vx and Vy are added together. If the result is greater
	// than 8 bits (i.e., >255) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx
	void OP_8xy4();

	// SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow. If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is
	// subtracted from Vx, and the results stored in Vx.
	void OP_8xy5();

	// SHR Vx - Set Vx = Vx bitshifted right 1. If the least-significant (rightmost) bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2
	void OP_8xy6();

	// SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow. If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is
	// subtracted from Vy, and the results stored in Vx.
	void OP_8xy7();

	// SHL Vx - Set Vx = Vx bitshifted left 1. If the most-significant (leftmost) bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
	void OP_8xyE();

	// SNE Vx, Vy - Skip next instruction if Vx != Vy. The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
	void OP_9xy0();

	// LD I, address
	void OP_Annn();

	// JP V0, address
	void OP_Bnnn();

	// RND Vx, byte
	void OP_Cxkk();

	// DRW Vx, Vy, height
	void OP_Dxyn();

	// SKP Vx
	void OP_Ex9E();

	// SKNP Vx
	void OP_ExA1();

	// LD Vx, DT
	void OP_Fx07();

	// LD Vx, K
	void OP_Fx0A();

	// LD DT, Vx
	void OP_Fx15();

	// LD ST, Vx
	void OP_Fx18();

	// ADD I, Vx
	void OP_Fx1E();

	// LD F, Vx
	void OP_Fx29();

	// LD B, Vx
	void OP_Fx33();

	// LD [I], Vx
	void OP_Fx55();

	// LD Vx, [I]
	void OP_Fx65();

	std::default_random_engine randGen;
	std::uniform_int_distribution<bit8> randByte;

	bit8 registers[16]{};
	bit8 memory[4096]{};
	bit16 index{};
	bit16 pc{};
	bit16 stack{};
	bit8 sp{};
	bit8 delayTimer{};
	bit8 soundTimer{};
	bit16 opcode;

	//Initialize tables of opcode function pointers - need typedef so it doesn't look stupid :)
	//By default, these tables will be filled by reference to OP_NULL, which does nothing
	//Actual opcodes are added to the tables in Chip8.cpp
	typedef void (Chip8::*OpRef)();
	OpRef table[0xF + 1]{ &Chip8::OP_NULL };
	OpRef table0[0xE + 1]{ &Chip8::OP_NULL };	//master table: will contain opcodes that do not start with the aforementioned letters/numbers, 
	OpRef table8[0xE + 1]{ &Chip8::OP_NULL };	//alongside references to the 4 functions that direct to other 4 tables
	OpRef tableE[0xE + 1]{ &Chip8::OP_NULL };	//reminder: 1 is added to the hex value because the max array index is 1-size,
	OpRef tableF[0x65 + 1]{ &Chip8::OP_NULL };  //and the last digit of the hex value corresponds with the last digit of the opcode.
};


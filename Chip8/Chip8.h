#pragma once

#include <cstdint>
#include <random>
#include <SFML/Graphics.hpp>

const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;


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
	void loadROM(std::string file);

	//fetch opcode, decode, next execute
	void cycle();

	//public so they can be accessed by the Display class
	uint8_t keypad[KEY_COUNT]{};
	uint8_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};
	
	uint16_t getOpcode();
	uint16_t getProgramCounter();
	uint16_t getIndex();
	uint8_t getStackPointer();
	uint8_t getDelayTimer();
	uint8_t* getRegisters();
	uint16_t* getStack();

private:
	//Initialize tables of opcode function pointers - need typedef so it doesn't look stupid :)
	//By default, these tables will be filled by reference to OP_NULL, which does nothing
	//Actual opcodes are added to the tables in Chip8.cpp
	typedef void (Chip8::* OpRef)();
	OpRef table[0xF + 1]{ &Chip8::OP_NULL };
	OpRef table0[0xE + 1]{ &Chip8::OP_NULL };	//master table: will contain opcodes that do not start with the aforementioned letters/numbers, 
	OpRef table8[0xE + 1]{ &Chip8::OP_NULL };	//alongside references to the 4 functions that direct to other 4 tables
	OpRef tableE[0xE + 1]{ &Chip8::OP_NULL };	//reminder: 1 is added to the hex value because the max array index is 1-size,
	OpRef tableF[0x65 + 1]{ &Chip8::OP_NULL };  //and the last digit of the hex value corresponds with the last digit of the opcode.

	std::default_random_engine randGen;
	std::uniform_int_distribution<> randByte;

	uint8_t registers[REGISTER_COUNT]{};
	uint8_t memory[MEMORY_SIZE]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[STACK_LEVELS]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint16_t opcode;

	//These functions will dereference the pointer to the opcode functions for their table.
	//For example, when opcode=0x00E0, table0[(0x00E0 & 0x000F)] = table0[(0x0)], which returns a pointer to Chip8::OP_00E0
	//These tables are used because many opcodes can be grouped by their starting values: 00, 8xy, Ex, or Fx
	void Table0();
	void Table8();
	void TableE();
	void TableF();

	// Following opcode implementations are based from
    // http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf

// Do nothing
	void OP_NULL();
	// CLS
	void OP_00E0();
	// RET
	void OP_00EE();
	// JP address
	void OP_1nnn();
	// CALL address
	void OP_2nnn();
	// SE Vx, byte
	void OP_3xkk();
	// SNE Vx, byte
	void OP_4xkk();
	// SE Vx, Vy
	void OP_5xy0();
	// LD Vx, byte
	void OP_6xkk();
	// ADD Vx, byte
	void OP_7xkk();
	// LD Vx, Vy
	void OP_8xy0();
	// OR Vx, Vy
	void OP_8xy1();
	// AND Vx, Vy
	void OP_8xy2();
	// XOR Vx, Vy
	void OP_8xy3();
	// ADD Vx, Vy
	void OP_8xy4();
	// SUB Vx, Vy
	void OP_8xy5();
	// SHR Vx
	void OP_8xy6();
	// SUBN Vx, Vy
	void OP_8xy7();
	// SHL Vx
	void OP_8xyE();
	// SNE Vx, Vy
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
};


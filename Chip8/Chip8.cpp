#include "Chip8.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <random>
#include <SFML/Graphics.hpp>
#include <iomanip>

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

uint8_t fontset[80] = 
	{
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	pc = 0x200;
	randByte = std::uniform_int_distribution<>(0, 225U);

	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xkk;
	table[0x4] = &Chip8::OP_4xkk;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xkk;
	table[0x7] = &Chip8::OP_7xkk;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxkk;
	table[0xD] = &Chip8::OP_Dxyn;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;
	table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;
	table8[0x0] = &Chip8::OP_8xy0;
	table8[0x1] = &Chip8::OP_8xy1;
	table8[0x2] = &Chip8::OP_8xy2;
	table8[0x3] = &Chip8::OP_8xy3;
	table8[0x4] = &Chip8::OP_8xy4;
	table8[0x5] = &Chip8::OP_8xy5;
	table8[0x6] = &Chip8::OP_8xy6;
	table8[0x7] = &Chip8::OP_8xy7;
	table8[0xE] = &Chip8::OP_8xyE;
	tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;
	tableF[0x07] = &Chip8::OP_Fx07;
	tableF[0x0A] = &Chip8::OP_Fx0A;
	tableF[0x15] = &Chip8::OP_Fx15;
	tableF[0x18] = &Chip8::OP_Fx18;
	tableF[0x1E] = &Chip8::OP_Fx1E;
	tableF[0x29] = &Chip8::OP_Fx29;
	tableF[0x33] = &Chip8::OP_Fx33;
	tableF[0x55] = &Chip8::OP_Fx55;
	tableF[0x65] = &Chip8::OP_Fx65;
}

void Chip8::loadROM(std::string file) 
{
	std::cout << "Reading..." << std::endl;
	// open binary stream from end
	std::ifstream rom(file, std::ios::binary | std::ios::ate);

	if (rom.is_open())
	{
		std::streampos size = rom.tellg(); //get size from position, which will be at the end
		char* buffer = new char[size]; //initialize buffer using size

		rom.seekg(0, std::ios::beg);
		rom.read(buffer, size);
		rom.close();

		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i]; //load to memory
		}
		std::cout << "Loaded ROM into memory..." << std::endl;
		delete[] buffer; buffer = nullptr;
	} 
	else
	{
		std::cerr << "Could not open file." << std::endl;
	}

}

void Chip8::cycle()
{
	//opcodes are split across two memory addresses
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	pc += 2;

	((*this).*(table[(opcode & 0xF000u) >> 12u]))(); // get first hex digit of opcode to reference table
	
	if (delayTimer > 0)
	{
		--delayTimer;
	}
	if (soundTimer > 0)
	{
		--soundTimer;
	}
}

uint16_t Chip8::getOpcode()
{
	return opcode;
}

uint16_t Chip8::getProgramCounter()
{
	return pc;
}

uint8_t* Chip8::getRegisters()
{
	return registers;
}

uint16_t Chip8::getIndex()
{
	return index;
}

uint8_t Chip8::getStackPointer()
{
	return sp;
}

uint8_t Chip8::getDelayTimer()
{
	return delayTimer;
}


uint16_t* Chip8::getStack()
{
	return stack;
}

	// Following opcode implementations are based from
	// http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf
	// https://austinmorlan.com/posts/chip8_emulator/#source-code - used this to fix the many opcode bugs I found

void Chip8::OP_NULL()
{}

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
	--sp;
	pc = stack[sp];
}

void Chip8::OP_1nnn()
{
	uint16_t addr = opcode & 0x0FFFu;
	pc = addr;
}

void Chip8::OP_2nnn()
{
	uint16_t addr = opcode & 0x0FFFu;
	stack[sp] = pc;
	pc = addr;
	++sp;
}

void Chip8::OP_3xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	if (registers[Vx] == byte)
	{
		pc += 2;
	}
}

void Chip8::OP_4xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	if (registers[Vx] != byte)
	{
		pc += 2;
	}
}

void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] == registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] = byte;
}

void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] += byte;
}

void Chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint16_t sum = registers[Vx] + registers[Vy];
	if (sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);
	registers[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
	registers[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_Annn()
{
	uint16_t addr = opcode & 0x0FFFu;
	index = addr;
}

void Chip8::OP_Bnnn()
{
	uint16_t addr = opcode & 0x0FFFu;
	pc = registers[0] + addr;
}

void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	uint8_t x = registers[Vx] % VIDEO_WIDTH;
	uint8_t y = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < 8; ++j)
		{
			uint8_t spritePixel = memory[index + i] & (0x80u >> j);
			uint8_t* screenPixel = &video[(y + i) * VIDEO_WIDTH + (x + j)];
			if (spritePixel)
			{
				if (*screenPixel == 0xFF)
				{
					registers[0xF] = 1;
				}
				*screenPixel ^= 0xFF;
			}
		}
	}
}

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	if (keypad[registers[Vx]])
	{
		pc += 2;
	}
}

void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	if (!keypad[registers[Vx]])
	{
		pc += 2;
	}
}

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	bool unPressed = false;
	for (int i = 0; i < 16; ++i)
	{
		if (keypad[i])
		{
			registers[Vx] = i;
			break;
		}
		unPressed = true;
	}
	if(unPressed)
	{
		pc -= 2;
	}
}

void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	delayTimer = registers[Vx];
}

void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index += registers[Vx];
}

void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index = FONTSET_START_ADDRESS + (5 * registers[Vx]);
}

void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];
	memory[index + 2] = value % 10;
	value /= 10;
	memory[index + 1] = value % 10;
	value /= 10;
	memory[index] = value % 10;
}

void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
}

void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index + i];
	}
}

void Chip8::Table0()
{
	((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
	((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
	((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
	((*this).*(tableF[opcode & 0x00FFu]))();
}


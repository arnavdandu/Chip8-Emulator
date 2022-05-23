#include "Chip8.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <random>
#include <SFML/Graphics.hpp>

bit8 fontset[80] = 
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

Chip8::Chip8() : mt(rd())
{
	pc = 0x200;
	randByte = std::uniform_int_distribution<>(0, 225U);

	for (unsigned int i = 0; i < 80; ++i)
	{
		memory[0x50 + i] = fontset[i];
	}

	initializeTable();
}

void Chip8::loadROM(char const* file) 
{

	// open binary stream from end
	std::ifstream rom(file, std::ios::binary | std::ios::ate);

	if (rom.is_open())
	{
		std::streampos size = rom.tellg(); //get size from position, which will be at the end
		char* buffer = new char[size]; //initialize buffer using size

		rom.seekg(std::ios::beg);
		rom.read(buffer, size);
		rom.close();

		for (long i = 0; i < size; ++i)
		{
			memory[0x200 + i] = buffer[i]; //load to memory
		}

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
	opcode = memory[pc] >> 8u | memory[pc + 1];
	pc += 2;

	((*this).*(table[opcode & 0xF000u >> 12u]))(); // get first hex digit of opcode to reference table
	
	if (delayTimer > 0)
	{
		--delayTimer;
	}
	if (soundTimer > 0)
	{
		--soundTimer;
	}
}

bit8 Chip8::Vx()
{
	return opcode & 0x0F00u >> 8u;
}

bit8 Chip8::Vy()
{
	return opcode & 0x00F0u >> 4u;
}

void Chip8::OP_NULL()
{
	return;
}

// CLS
void Chip8::OP_00E0()
{
	memset(video, 0, 64 * 32);
}

// RET
void Chip8::OP_00EE()
{
	--sp;
	pc = stack[sp];
}

// JP addr
void Chip8::OP_1nnn()
{
	pc = opcode & 0x0FFFu; // address codes are 12bit; take first 12 from opcode
}

// CALL addr
void Chip8::OP_2nnn()
{
	stack[sp] = pc;
	pc = opcode & 0x0FFFu;
	++sp;
}

// SE Vx, byte
void Chip8::OP_3xkk()
{
	if (registers[Vx()] == (opcode & 0x00FFu))
	{
		pc += 2;
	}
}

// SNE Vx, byte
void Chip8::OP_4xkk()
{
	if (registers[Vx()] != (opcode & 0x00FFu))
	{
		pc += 2;
	}
}

// SE Vx, Vy
void Chip8::OP_5xy0()
{
	if (registers[Vx()] == registers[Vy()])
	{
		pc += 2;
	}
}

// LD Vx, byte
void Chip8::OP_6xkk()
{
	registers[Vx()] = (opcode & 0x00FFu);
}

// ADD Vx, byte
void Chip8::OP_7xkk()
{
	registers[Vx()] += (opcode & 0x00FFu);
}

// LD Vx, Vy 
void Chip8::OP_8xy0()
{
	registers[Vx()] = registers[Vy()];
}

// OR Vx, Vy 
void Chip8::OP_8xy1()
{
	registers[Vx()] |= registers[Vy()];
}

// AND Vx, Vy
void Chip8::OP_8xy2()
{
	registers[Vx()] &= registers[Vy()];
}

// XOR Vx, Vy
void Chip8::OP_8xy3()
{
	registers[Vx()] ^= registers[Vy()];
}

// ADD Vx, Vy
void Chip8::OP_8xy4()
{
	bit16 sum = registers[Vx()] + registers[Vy()];
	registers[0xF] = (sum > 0xFFu) ? 1 : 0;
	registers[Vx()] = sum;
}

// SUB Vx, Vy
void Chip8::OP_8xy5()
{
	registers[0xF] = (registers[Vx()] > registers[Vy()]) ? 1 : 0;
	registers[Vx()] -= registers[Vy()];
}

// SHR Vx
void Chip8::OP_8xy6()
{
	registers[0xF] = (registers[Vx()] & 0x1u);
	registers[Vx()] >>= 1;
}

// SUBN Vx, Vy
void Chip8::OP_8xy7()
{
	registers[0xF] = (registers[Vx()] < registers[Vy()]) ? 1 : 0;
	registers[Vx()] = registers[Vy()] - registers[Vx()];
}

// SHL Vx, Vy
void Chip8::OP_8xyE()
{
	registers[0xF] = (registers[Vx()] & 0x80u) >> 7u;
	registers[Vx()] <<= 1;
}

// SNE Vx, Vy
void Chip8::OP_9xy0()
{
	if (registers[Vx()] != registers[Vy()])
		pc += 2;
}

// LD I, addr
void Chip8::OP_Annn()
{
	index = opcode & 0x0FFFu;
}

// JP V0, addr
void Chip8::OP_Bnnn()
{
	pc = registers[0] + opcode & 0x0FFFu;
}

// RND Vx, byte
void Chip8::OP_Cxkk()
{
	registers[Vx()] = randByte(mt) & 0x00FFu;
}

// DRW Vx, Vy, height
void Chip8::OP_Dxyn()
{
	bit8 x = registers[Vx()] % 64;
	bit8 y = registers[Vy()] % 32;

	registers[0xF] = 0;
	for (unsigned int i = 0; i < (opcode % 0x000F); i+=4)
	{
		for (unsigned int j = 0; j < 32; j+=4)
		{
			bit8 spritePixel = memory[index + i];
			sf::Uint8* screenPixel = &video[(y + i) * 64 + (x + j)];
			sf::Uint8* screenPixel1 = &video[(y + i) * 64 + (x + j)+1];
			sf::Uint8* screenPixel2 = &video[(y + i) * 64 + (x + j)+2];
			sf::Uint8* screenPixel3 = &video[(y + i) * 64 + (x + j)+3];
			// if spritePixel is active here, indicate collision with screenPixel
			if (spritePixel)
			{
				if (*screenPixel == 0xFF)
				{
					registers[0xF] = 1; // indicate collision using VF
				}
				// XOR to flip pixel
				*screenPixel ^= 0xFF;
				*screenPixel1 ^= 0xFF;
				*screenPixel2 ^= 0xFF;
				*screenPixel3 ^= 0xFF;
			}
		}
	}

}

// SKP Vx
void Chip8::OP_Ex9E()
{
	if (keypad[registers[Vx()]])
	{
		pc += 2;
	}
}

// SKNP Vx
void Chip8::OP_ExA1()
{
	if (!keypad[registers[Vx()]])
	{
		pc += 2;
	}
}

// LD Vx, DT
void Chip8::OP_Fx07()
{
	registers[Vx()] = delayTimer;
}

// LD Vx, k
void Chip8::OP_Fx0A()
{
	bool clicked = false;
	for (unsigned int i = 0; i < 16; ++i)
	{
		if (keypad[i])
		{
			clicked = true;
			registers[Vx()] = i;
			break;
		}
	}
	if (!clicked)
	{
		pc -= 2;
	}
}

// LD DT, Vx
void Chip8::OP_Fx15()
{
	delayTimer = registers[Vx()];
}

// LD ST, Vx
void Chip8::OP_Fx18()
{
	soundTimer = registers[Vx()];
}

// ADD I, Vx
void Chip8::OP_Fx1E()
{
	index += registers[Vx()];
}

// LD F, Vx
void Chip8::OP_Fx29()
{
	index = 0x50 + (5 * registers[Vx()]);
}

// LD B, Vx
void Chip8::OP_Fx33()
{
	memory[index] = (Vx() % 1000) / 100; // hundreds
	memory[index + 1] = (Vx() % 100) / 10; // tens
	memory[index + 2] = Vx() % 10; // ones
}

// LD [I], Vx
void Chip8::OP_Fx55()
{
	for (bit8 i = 0; i < Vx(); ++i)
	{
		memory[index + i] = registers[i];
	}
	index += Vx() + 1;
}

// LD Vx, [I]
void Chip8::OP_Fx65()
{
	for (bit8 i = 0; i < Vx(); ++i)
	{
		registers[i] = memory[index + i];
	}
	index += Vx() + 1;
}

void Chip8::Table0()
{
	((*this).*(table0[opcode % 0x000Fu]))();
}

void Chip8::Table8()
{
	((*this).*(table8[opcode % 0x000Fu]))();
}

void Chip8::TableE()
{
	((*this).*(tableE[opcode % 0x000Fu]))();
}

void Chip8::TableF()
{
	((*this).*(tableF[opcode % 0x000Fu]))();
}

void Chip8::initializeTable()
{
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

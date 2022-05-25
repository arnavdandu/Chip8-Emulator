#include "Display.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

// References: https://www.sfml-dev.org/tutorials/2.5/

Display::Display(const char* name, int texW, int texH, float windowScale)
	: scale(windowScale)
{
	table[0x0] = &Display::Table0;
	table[0x1] = &Display::OP_1nnn;
	table[0x2] = &Display::OP_2nnn;
	table[0x3] = &Display::OP_3xkk;
	table[0x4] = &Display::OP_4xkk;
	table[0x5] = &Display::OP_5xy0;
	table[0x6] = &Display::OP_6xkk;
	table[0x7] = &Display::OP_7xkk;
	table[0x8] = &Display::Table8;
	table[0x9] = &Display::OP_9xy0;
	table[0xA] = &Display::OP_Annn;
	table[0xB] = &Display::OP_Bnnn;
	table[0xC] = &Display::OP_Cxkk;
	table[0xD] = &Display::OP_Dxyn;
	table[0xE] = &Display::TableE;
	table[0xF] = &Display::TableF;
	table0[0x0] = &Display::OP_00E0;
	table0[0xE] = &Display::OP_00EE;
	table8[0x0] = &Display::OP_8xy0;
	table8[0x1] = &Display::OP_8xy1;
	table8[0x2] = &Display::OP_8xy2;
	table8[0x3] = &Display::OP_8xy3;
	table8[0x4] = &Display::OP_8xy4;
	table8[0x5] = &Display::OP_8xy5;
	table8[0x6] = &Display::OP_8xy6;
	table8[0x7] = &Display::OP_8xy7;
	table8[0xE] = &Display::OP_8xyE;
	tableE[0x1] = &Display::OP_ExA1;
	tableE[0xE] = &Display::OP_Ex9E;
	tableF[0x07] = &Display::OP_Fx07;
	tableF[0x0A] = &Display::OP_Fx0A;
	tableF[0x15] = &Display::OP_Fx15;
	tableF[0x18] = &Display::OP_Fx18;
	tableF[0x1E] = &Display::OP_Fx1E;
	tableF[0x29] = &Display::OP_Fx29;
	tableF[0x33] = &Display::OP_Fx33;
	tableF[0x55] = &Display::OP_Fx55;
	tableF[0x65] = &Display::OP_Fx65;

	if (!font.loadFromFile("consola.ttf"))
	{
		std::cerr << "Could not load font" << std::endl;
	}
	window.create(sf::VideoMode(texW * scale + 15 * scale, texH * scale), name);
	if (!texture.create(texW, texH))
	{
		std::cerr << "Could not create texture" << std::endl;
	}
	sprite.setScale(sf::Vector2f(scale, scale));
	window.clear(sf::Color::Black);
	window.display();
}

void Display::updateDisplay(const uint8_t* video, const uint16_t op,  const uint16_t pc, const uint16_t i,
	const uint8_t sp, const uint8_t dt, const uint8_t* registers, const uint16_t* stack)
{
	opcode = op;
	index = i;
	delayTimer = dt;
	window.clear(sf::Color::Black);
	std::stringstream stream;

	// generate debug details
	stream << "OPCODE: " << "0x" << std::hex << opcode << std::endl;
	stream << ((*this).*(table[(opcode & 0xF000u) >> 12u]))() << std::endl;
	stream << "PRGM CNTR: " << "0x" << std::setw(3) <<
		std::setfill('0') << std::hex << pc << std::endl;
	stream << "INDEX: " << "0x" << std::setw(3) <<
		std::setfill('0') << std::hex << index << std::endl;
	stream << "STACK PNTER: " << "0x" << std::setw(2) <<
		std::setfill('0') << std::hex << (int)sp << std::endl;
	stream << std::endl;

	stream << "REG:         STACK:" << std::endl;

	// draw pixels from video array
	for (int i = 0; i < 64 * 32 * 4; i+=4)
	{
		if (video[i/4] == 0xFF)
		{
			pixels[i] = 0xFF;
			pixels[i+1] = 0xFF;
			pixels[i+2] = 0xFF;
			pixels[i+3] = 0xFF;
		}
		else 
		{
			pixels[i] = 0x00;
			pixels[i + 1] = 0x00;
			pixels[i + 2] = 0x00;
			pixels[i + 3] = 0x00;
		}
	}

	// draw register and stack indicators
	for (int i = 0; i < 16; ++i)
	{
		stream << "V" << std::hex << i << ": " << "0x" << std::setw(2) <<
			std::setfill('0') << std::hex << (int)registers[i];
		stream << "     " << std::hex << i << ": " << "0x" << std::setw(4) <<
			std::setfill('0') << std::hex << (int)stack[i] << std::endl;

		registerIndicators[i].setRadius(scale/2.5);
		registerIndicators[i].setOutlineColor(sf::Color::White);
		registerIndicators[i].setOutlineThickness(2.0f);
		
		if (registerIndicators[i].getFillColor() != sf::Color::Red)
		{
			registerIndicators[i].setFillColor(sf::Color::Black);
		}
		
		if (registers[0xF] != 0)
		{
			registerIndicators[0xF].setFillColor(sf::Color::Red);
		}
		else
		{
			registerIndicators[0xF].setFillColor(sf::Color::Black);
		}
		
		registerIndicators[i].setPosition(sf::Vector2f(64.0f * scale + scale * 0.5, i * scale * 1.16 + 8.8 * scale));
		
		stackIndicators[i].setRadius(scale / 2.5);
		stackIndicators[i].setOutlineColor(sf::Color::White);
		stackIndicators[i].setOutlineThickness(2.0f);
		if (stack[i] != 0x0000)
		{
			stackIndicators[i].setFillColor(sf::Color::Red);
		}
		else
		{
			stackIndicators[i].setFillColor(sf::Color::Black);
		}
		stackIndicators[i].setPosition(sf::Vector2f(64.0f * scale + scale * 7.8, i * scale * 1.16 + 8.8 * scale));
		
		window.draw(registerIndicators[i]);
		window.draw(stackIndicators[i]);
	}

	sf::Text debug(stream.str(), font);
	debug.setCharacterSize(scale);
	debug.setPosition(sf::Vector2f(64.0f * scale + 2 * scale, 10.0f));
	debug.setFillColor(sf::Color::White);
	texture.update(pixels);
	sprite.setTexture(texture);
	window.draw(sprite);
	window.draw(debug);
	window.display();
	for (int i = 0; i < 16; ++i)
	{
		registerIndicators[i].setFillColor(sf::Color::Black);
	}
}

// https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Keyboard.php

bool Display::processInput(uint8_t* keys)
{
	bool quit = false;
	sf::Event event;

    // while there are pending events...
    while (window.pollEvent(event))
    {
        // check the type of the event...
        switch (event.type)
        {
            // key pressed
			case sf::Event::KeyPressed:
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
				{
					quit = true;
				} break;
				case sf::Keyboard::Key::X:
				{
					keys[0] = 1;
				} break;

				case sf::Keyboard::Key::Num1:
				{
					keys[1] = 1;
				} break;

				case sf::Keyboard::Key::Num2:
				{
					keys[2] = 1;
				} break;

				case sf::Keyboard::Key::Num3:
				{
					keys[3] = 1;
				} break;

				case sf::Keyboard::Key::Q:
				{
					keys[4] = 1;
				} break;

				case sf::Keyboard::Key::W:
				{
					keys[5] = 1;
				} break;

				case sf::Keyboard::Key::E:
				{
					keys[6] = 1;
				} break;

				case sf::Keyboard::Key::A:
				{
					keys[7] = 1;
				} break;

				case sf::Keyboard::Key::S:
				{
					keys[8] = 1;
				} break;

				case sf::Keyboard::Key::D:
				{
					keys[9] = 1;
				} break;

				case sf::Keyboard::Key::Z:
				{
					keys[0xA] = 1;
				} break;

				case sf::Keyboard::Key::C:
				{
					keys[0xB] = 1;
				} break;

				case sf::Keyboard::Key::Num4:
				{
					keys[0xC] = 1;
				} break;

				case sf::Keyboard::Key::R:
				{
					keys[0xD] = 1;
				} break;

				case sf::Keyboard::Key::F:
				{
					keys[0xE] = 1;
				} break;

				case sf::Keyboard::Key::V:
				{
					keys[0xF] = 1;
				} break;
				}
			} break;

			case sf::Event::KeyReleased:
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
				{
					quit = true;
				} break;
				case sf::Keyboard::Key::X:
				{
					keys[0] = 0;
				} break;

				case sf::Keyboard::Key::Num1:
				{
					keys[1] = 0;
				} break;

				case sf::Keyboard::Key::Num2:
				{
					keys[2] = 0;
				} break;

				case sf::Keyboard::Key::Num3:
				{
					keys[3] = 0;
				} break;

				case sf::Keyboard::Key::Q:
				{
					keys[4] = 0;
				} break;

				case sf::Keyboard::Key::W:
				{
					keys[5] = 0;
				} break;

				case sf::Keyboard::Key::E:
				{
					keys[6] = 0;
				} break;

				case sf::Keyboard::Key::A:
				{
					keys[7] = 0;
				} break;

				case sf::Keyboard::Key::S:
				{
					keys[8] = 0;
				} break;

				case sf::Keyboard::Key::D:
				{
					keys[9] = 0;
				} break;

				case sf::Keyboard::Key::Z:
				{
					keys[0xA] = 0;
				} break;

				case sf::Keyboard::Key::C:
				{
					keys[0xB] = 0;
				} break;

				case sf::Keyboard::Key::Num4:
				{
					keys[0xC] = 0;
				} break;

				case sf::Keyboard::Key::R:
				{
					keys[0xD] = 0;
				} break;

				case sf::Keyboard::Key::F:
				{
					keys[0xE] = 0;
				} break;

				case sf::Keyboard::Key::V:
				{
					keys[0xF] = 0;
				} break;
				}
			} break;

			default:
				break;
		} 
	}
	return quit;
}

std::string Display::Table0()
{
	return ((*this).*(table0[opcode & 0x000Fu]))();
}

std::string Display::Table8()
{
	return ((*this).*(table8[opcode & 0x000Fu]))();
}

std::string Display::TableE()
{
	return ((*this).*(tableE[opcode & 0x000Fu]))();
}

std::string Display::TableF()
{
	return ((*this).*(tableF[opcode & 0x00FFu]))();
}

// Do nothing
std::string Display::OP_NULL() { return ""; }
// CLS
std::string Display::OP_00E0()
{
	return "CLS";
}
// RET
std::string Display::OP_00EE()
{
	return "RET";
}
// JP address
std::string Display::OP_1nnn()
{
	uint16_t addr = (opcode & 0x0FFF);
	std::stringstream stream;
	stream << "JP 0x" << std::setw(3) << std::setfill('0') << std::hex << (int)addr;
	return stream.str();
}

// CALL address
std::string Display::OP_2nnn()
{
	uint16_t addr = (opcode & 0x0FFF);
	std::stringstream stream;
	stream << "CALL 0x" << std::setw(3) << std::setfill('0') << std::hex << (int)addr;
	return stream.str();
}
// SE Vx, byte
std::string Display::OP_3xkk()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t byte = (opcode & 0x00FF);
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SE V" << std::hex << (int)Vx;
	stream << ", " << std::setw(2) << std::setfill('0') << std::hex << (int)byte;
	return stream.str();
}
// SNE Vx, byte
std::string Display::OP_4xkk()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t byte = (opcode & 0x00FF);
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SNE V" << std::hex << (int)Vx;
	stream << ", " << std::setw(2) << std::setfill('0') << std::hex << (int)byte;
	return stream.str();
}
// SE Vx, Vy
std::string Display::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SE V" << std::hex << (int)Vx;
	stream << ", V" << std::hex << (int)Vy;
	return stream.str();
}
// LD Vx, byte
std::string Display::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	uint8_t byte = (opcode & 0x00FF);
	std::stringstream stream;
	stream << "LD V" << std::hex << (int)Vx;
	stream << ", " << std::setw(2) << std::setfill('0') << std::hex << (int)byte;
	return stream.str();
}
// ADD Vx, byte
std::string Display::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t byte = (opcode & 0x00FF);
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "ADD V" << std::hex << (int)Vx;
	stream << ", " << std::setw(2) << std::setfill('0') << std::hex << (int)byte;
	return stream.str();
}
// LD Vx, Vy
std::string Display::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "LD V" << std::hex << (int)Vx;
	stream << ", V" << std::setw(1) << std::setfill('0') << std::hex << (int)Vy;
	return stream.str();
}
// OR Vx, Vy
std::string Display::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "OR V" << std::hex << (int)Vx;
	stream << ", V" << std::setw(1) << std::setfill('0') << std::hex << (int)Vy;
	return stream.str();
}
// AND Vx, Vy
std::string Display::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "AND V" << std::hex << (int)Vx;
	stream << ", V" << std::setw(1) << std::setfill('0') << std::hex << (int)Vy;
	return stream.str();
}
// XOR Vx, Vy
std::string Display::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "XOR V" << std::hex << (int)Vx;
	stream << ", V" << std::hex << (int)Vy;
	return stream.str();
}
// ADD Vx, Vy
std::string Display::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "ADD V" << std::hex << (int)Vx;
	stream << ", V" << std::hex << (int)Vy;
	return stream.str();
}
// SUB Vx, Vy
std::string Display::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SUB V" << std::hex << (int)Vx;
	stream << ", V" << std::hex << (int)Vy;
	return stream.str();
}
// SHR Vx
std::string Display::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SHR V" << std::hex << (int)Vx;
	return stream.str();
}
// SUBN Vx, Vy
std::string Display::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SUBN V" << std::hex << (int)Vx;
	stream << ", V" << std::hex << (int)Vy;
	return stream.str();
}
// SHL Vx
std::string Display::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SHL V" << std::hex << (int)Vx;
	return stream.str();
}
// SNE Vx, Vy
std::string Display::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SNE V" << std::hex << (int)Vx;
	stream << ", V" << std::hex << (int)Vy;
	return stream.str();
}
// LD I, address
std::string Display::OP_Annn()
{
	uint16_t addr = (opcode & 0x0FFF);
	std::stringstream stream;
	stream << "LD " << std::hex << (int)index;
	stream << ", 0x" << std::setw(3) << std::setfill('0') << std::hex << (int)addr;
	return stream.str();
}
// JP V0, address
std::string Display::OP_Bnnn()
{
	uint16_t addr = (opcode & 0x0FFF);
	std::stringstream stream;
	stream << "JP V0, " << std::setw(3) << std::setfill('0') << std::hex << (int)addr;
	return stream.str();
}
// RND Vx, byte
std::string Display::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00);
	uint8_t byte = (opcode & 0x00FF);
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "RND V" << std::hex << (int)Vx;
	stream << ", " << std::setw(2) << std::setfill('0') << std::hex << (int)byte;
	return stream.str();
}
// DRW Vx, Vy, height
std::string Display::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	uint8_t Vy = (opcode & 0x00F0) >> 4u;
	uint8_t height = (opcode & 0x000F);
	std::stringstream stream;
	stream << "DRW V" << std::hex << (int)Vx;
	stream << ", V" << std::hex << (int)Vy;
	stream << ", " << (int)height;
	return stream.str();
}
// SKP Vx
std::string Display::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	//registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SKP V" << std::hex << (int)Vx;
	return stream.str();
}
// SKNP Vx
std::string Display::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	//registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "SKNP V" << std::hex << (int)Vx;
	return stream.str();
}
// LD Vx, DT
std::string Display::OP_Fx07()
{

	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "LD V" << std::hex << (int)Vx;
	stream << ", " << delayTimer;
	return stream.str();
}
// LD Vx, K
std::string Display::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "LD V" << std::hex << (int)Vx;
	stream << ", K";
	return stream.str();
}
// LD DT, Vx
std::string Display::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	std::stringstream stream;
	stream << "LD " << std::setw(2) << std::setfill('0') << std::hex << (int)delayTimer;
	stream << ", V" << std::hex << (int)Vx;
	return stream.str();
}
// LD ST, Vx
std::string Display::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	std::stringstream stream;
	stream << "LD ST";
	stream << ", V" << std::hex << (int)Vx;
	return stream.str();
}
// ADD I, Vx
std::string Display::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	std::stringstream stream;
	stream << "ADD " << std::setw(3) << std::setfill('0') << std::hex << (int)index;
	stream << ", V" << (int)Vx;
	return stream.str();
}
// LD F, Vx
std::string Display::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	std::stringstream stream;
	stream << "LD F";
	stream << ", V" << std::hex << (int)Vx;
	return stream.str();
}
// LD B, Vx
std::string Display::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	std::stringstream stream;
	stream << "LD B";
	stream << ", V" << std::hex << (int)Vx;
	return stream.str();
}
// LD [I], Vx
std::string Display::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	std::stringstream stream;
	stream << "LD 0x" << std::setw(3) << std::setfill('0') << std::hex << (int)index;
	stream << ", V" << std::hex << (int)Vx;
	return stream.str();
}
// LD Vx, [I]
std::string Display::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8u;
	registerIndicators[(int)Vx].setFillColor(sf::Color::Red);
	std::stringstream stream;
	stream << "LD V" << std::hex << (int)Vx;
	stream << ", 0x" << std::setw(3) << std::setfill('0') << std::hex << (int)index;
	return stream.str();
}
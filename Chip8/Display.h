#pragma once
#include <SFML/Graphics.hpp>

class Display
{
public:
	Display(const char* name, int texW, int texH, float windowScale);
	void updateDisplay(const uint8_t* video, const uint16_t opcode, const uint16_t pc, const uint16_t i,
		const uint8_t sp, const uint8_t dt, const uint8_t* registers, const uint16_t* stack);
	bool processInput(uint8_t* keys);
	
private:
	uint16_t opcode;
	uint16_t index;
	uint8_t delayTimer;
	sf::RenderWindow window;
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Uint8* pixels = new sf::Uint8[64 * 32 * 4]{};
	sf::Font font;
	sf::CircleShape registerIndicators[16];
	sf::CircleShape stackIndicators[16];
	float scale;

	std::string Table0();
	std::string Table8();
	std::string TableE();
	std::string TableF();

	// Do nothing
	std::string OP_NULL();
	// CLS
	std::string OP_00E0();
	// RET
	std::string OP_00EE();
	// JP address
	std::string OP_1nnn();
	// CALL address
	std::string OP_2nnn();
	// SE Vx, byte
	std::string OP_3xkk();
	// SNE Vx, byte
	std::string OP_4xkk();
	// SE Vx, Vy
	std::string OP_5xy0();
	// LD Vx, byte
	std::string OP_6xkk();
	// ADD Vx, byte
	std::string OP_7xkk();
	// LD Vx, Vy
	std::string OP_8xy0();
	// OR Vx, Vy
	std::string OP_8xy1();
	// AND Vx, Vy
	std::string OP_8xy2();
	// XOR Vx, Vy
	std::string OP_8xy3();
	// ADD Vx, Vy
	std::string OP_8xy4();
	// SUB Vx, Vy
	std::string OP_8xy5();
	// SHR Vx
	std::string OP_8xy6();
	// SUBN Vx, Vy
	std::string OP_8xy7();
	// SHL Vx
	std::string OP_8xyE();
	// SNE Vx, Vy
	std::string OP_9xy0();
	// LD I, address
	std::string OP_Annn();
	// JP V0, address
	std::string OP_Bnnn();
	// RND Vx, byte
	std::string OP_Cxkk();
	// DRW Vx, Vy, height
	std::string OP_Dxyn();
	// SKP Vx
	std::string OP_Ex9E();
	// SKNP Vx
	std::string OP_ExA1();
	// LD Vx, DT
	std::string OP_Fx07();
	// LD Vx, K
	std::string OP_Fx0A();
	// LD DT, Vx
	std::string OP_Fx15();
	// LD ST, Vx
	std::string OP_Fx18();
	// ADD I, Vx
	std::string OP_Fx1E();
	// LD F, Vx
	std::string OP_Fx29();
	// LD B, Vx
	std::string OP_Fx33();
	// LD [I], Vx
	std::string OP_Fx55();
	// LD Vx, [I]
	std::string OP_Fx65();

	typedef std::string (Display::* OpRef)();
	OpRef table[0xF + 1]{ &Display::OP_NULL };
	OpRef table0[0xE + 1]{ &Display::OP_NULL };
	OpRef table8[0xE + 1]{ &Display::OP_NULL };
	OpRef tableE[0xE + 1]{ &Display::OP_NULL };
	OpRef tableF[0x65 + 1]{ &Display::OP_NULL };
};


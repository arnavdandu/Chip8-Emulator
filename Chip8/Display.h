#pragma once
#include <SFML/Graphics.hpp>

class Display
{
public:
	Display(const char* name, int texW, int texH, float scale);
	void updateDisplay(const uint32_t* video);
	bool processInput(uint8_t* keys);
	
private:
	sf::RenderWindow window;
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Uint8* pixels = new sf::Uint8[64 * 32 * 4]{};
};


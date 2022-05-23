#pragma once
#include <SFML/Graphics.hpp>

class Display
{
public:
	Display(const char* name, int winW, int winH, int texW, int texH);
	void updateDisplay(const sf::Uint8* video);
	bool processInput(uint8_t* keys);
private:
	sf::RenderWindow window;
	sf::Texture texture;
	sf::Sprite sprite;
};


#pragma once
#include <SFML/Graphics.hpp>

class Display
{
public:
	Display(const char* name, int texW, int texH, float scale);
	void updateDisplay(const uint32_t* video);
	bool processInput(uint8_t* keys);
	
private:
};


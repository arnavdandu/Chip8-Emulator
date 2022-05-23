#include "Display.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// References: https://www.sfml-dev.org/tutorials/2.5/

Display::Display(const char* name, int winW, int winH, int texW, int texH)
	: window(sf::VideoMode(winW, winH), name) 
{
	if (!texture.create(texW, texH))
	{
		std::cerr << "Could not load initial texture" << std::endl;
		return;
	}
	sprite = sf::Sprite(texture);
	window.draw(sprite);
}

void Display::updateDisplay(const sf::Uint8* video)
{
	texture.update(video);
}

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

			default:
				break;
		} 
	}
	return quit;
}


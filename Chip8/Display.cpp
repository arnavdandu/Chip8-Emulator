#include "Display.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// References: https://www.sfml-dev.org/tutorials/2.5/

sf::RenderWindow window;
sf::Texture texture;
sf::Sprite sprite;
sf::Uint8* pixels = new sf::Uint8[64 * 32 * 4]{};

Display::Display(const char* name, int winW, int winH, int texW, int texH)
{
	window.create(sf::VideoMode(winW, winH), name);
	if (!texture.create(texW, texH))
	{
		std::cerr << "Could not create texture" << std::endl;
	}
	sprite.setScale(sf::Vector2f(10.0, 10.0));
	window.clear(sf::Color::Black);
	window.display();
}

void Display::updateDisplay(const uint32_t* video)
{
	window.clear(sf::Color::Black);
	for (int i = 0; i < 64 * 32 * 4; i+=4)
	{
		if (video[i/4] == 0xFFFFFFFF)
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
	texture.update(pixels);
	sprite.setTexture(texture);
	window.draw(sprite);
	window.display();
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


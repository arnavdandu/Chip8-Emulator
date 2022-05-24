#include "Chip8.h"
#include "Display.h"
#include <chrono>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

	Display display("CHIP-8 Emulator", 64, 32, videoScale);

	Chip8 chip8;
	chip8.loadROM(romFilename);

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = display.processInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.cycle();
			display.updateDisplay(chip8.video);
		}
	}

	return 0;
}
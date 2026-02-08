#include <iostream>
#include "chip8.hpp"

int main()
{
	Chip8* emu = new Chip8;
	std::cout << "Hello World!\n";
	std::cout << static_cast<int>(emu->generate_random_byte()) << std::endl;
}
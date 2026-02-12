#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>
#include <vector>

class Chip8 {
private:
	// Constants
	static constexpr unsigned int ROM_START_ADDRESS = 0x200;
	static constexpr unsigned int FONTSET_START_ADDRESS = 0x050;
	static constexpr unsigned int VIDEO_HEIGHT = 64;
	static constexpr unsigned int VIDEO_WIDTH = 32;
	static constexpr unsigned int MEM_SIZE = 4096;
	static constexpr unsigned int BUF_LEN = 16;
	static constexpr unsigned int PIXEL_ON = 0xFFFFFFFF;

	static const int   FONTSET_SIZE = 80;
	uint8_t fontset[FONTSET_SIZE] =
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

	// Random
	std::mt19937 gen;
	std::uniform_int_distribution<int> dist;

public:
	// Registers
	std::array<uint8_t, BUF_LEN> registers{};	// General Purpose Registers
	uint16_t index{};							// I register
	uint16_t pc{};								// Program Counter
	uint8_t  sp{};								// Stack Pointer
	uint8_t  delay_timer{};						// DT register
	uint8_t  sound_timer{};						// ST register

	// Regions
	std::array<uint8_t, MEM_SIZE> memory{};
	std::array<uint16_t, BUF_LEN> stack{};
	std::array<uint8_t, BUF_LEN> keypad{};
	std::array<std::array<uint32_t, VIDEO_WIDTH>, VIDEO_HEIGHT> video{};

	// Op
	uint16_t opcode{};

	// Constructor
	Chip8();

	// Interface
	void     load_ROM(const char* filename);
	uint8_t  generate_random_byte();

	// Helpful bitmasks
	uint16_t extract_nnn(uint16_t opcode);
	uint8_t  extract_n(uint16_t opcode);
	uint8_t  extract_x(uint16_t opcode);
	uint8_t  extract_y(uint16_t opcode);
	uint8_t  extract_kk(uint16_t opcode);

	// Peripherals
	const char get_current_key();
	const char wait_keypress();

	// Instructions
	void OP_00E0();
	void OP_00EE();

	void OP_1nnn();
	void OP_2nnn();
	void OP_3xkk();
	void OP_4xkk();
	void OP_5xy0();
	void OP_6xkk();
	void OP_7xkk();
	void OP_Annn();
	void OP_Bnnn();
	void OP_Cxkk();
	void OP_Dxyn();

	void OP_8xy0();
	void OP_8xy1();
	void OP_8xy2();
	void OP_8xy3();
	void OP_8xy4();
	void OP_8xy5();
	void OP_8xy6();
	void OP_8xy7();
	void OP_8xyE();
	void OP_9xy0();

	void OP_Ex9E();
	void OP_ExA1();
	void OP_Fx07();
	void OP_Fx0A();
	void OP_Fx15();
	void OP_Fx18();
	void OP_Fx1E();
	void OP_Fx29();
	void OP_Fx33();
	void OP_Fx55();
	void OP_Fx65();
};
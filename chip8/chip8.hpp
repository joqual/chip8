#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <functional>
#include <random>
#include <unordered_map>
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

	// Function tables
	using Operation = void (Chip8::*)();

	std::array<Operation, BUF_LEN> ops_table{
		&Chip8::dispatch0, &Chip8::OP_1nnn, &Chip8::OP_2nnn, &Chip8::OP_3xkk,
		&Chip8::OP_4xkk, &Chip8::OP_5xy0, &Chip8::OP_6xkk, &Chip8::OP_7xkk,
		&Chip8::dispatch8, &Chip8::OP_9xy0, &Chip8::OP_Annn, &Chip8::OP_Bnnn,
		&Chip8::OP_Cxkk, &Chip8::OP_Dxyn, &Chip8::dispatchE, &Chip8::dispatchF,
	};

	std::unordered_map<uint8_t, Operation> ops_0{
		{0xE0, &Chip8::OP_00E0},
		{0xEE, &Chip8::OP_00EE},
	};

	std::unordered_map<uint8_t, Operation> ops_8{
		{0x0, &Chip8::OP_8xy0},
		{0x1, &Chip8::OP_8xy1},
		{0x2, &Chip8::OP_8xy2},
		{0x3, &Chip8::OP_8xy3},
		{0x4, &Chip8::OP_8xy4},
		{0x5, &Chip8::OP_8xy5},
		{0x6, &Chip8::OP_8xy6},
		{0x7, &Chip8::OP_8xy7},
		{0xE, &Chip8::OP_8xyE},
	};

	std::unordered_map<uint8_t, Operation> ops_E{
		{0x9E, &Chip8::OP_Ex9E},
		{0xA1, &Chip8::OP_ExA1},
	};

	std::unordered_map<uint8_t, Operation> ops_F{
		{0x07, &Chip8::OP_Fx07},
		{0x0A, &Chip8::OP_Fx0A},
		{0x15, &Chip8::OP_Fx15},
		{0x18, &Chip8::OP_Fx18},
		{0x1E, &Chip8::OP_Fx1E},
		{0x29, &Chip8::OP_Fx29},
		{0x33, &Chip8::OP_Fx33},
		{0x55, &Chip8::OP_Fx55},
		{0x65, &Chip8::OP_Fx65},
	};

	// Dispatching helper functions
	void dispatch0() { std::invoke(ops_0[opcode & 0xFF], *this); }
	void dispatch8() { std::invoke(ops_8[opcode & 0x0F], *this); }
	void dispatchE() { std::invoke(ops_E[opcode & 0xFF], *this); }
	void dispatchF() { std::invoke(ops_F[opcode & 0xFF], *this); }

public:
	// Registers
	std::array<uint8_t, BUF_LEN> registers{};	// General Purpose Registers
	uint16_t index{};							// I register
	uint16_t pc{ ROM_START_ADDRESS };			// Program Counter
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
	void	 cycle();
	void	 decode_execute();

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
	void OP_Annn();
	void OP_Bnnn();
	void OP_Cxkk();
	void OP_Dxyn();

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

	void OP_NOP() {};
};
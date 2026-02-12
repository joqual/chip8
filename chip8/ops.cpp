#include "chip8.hpp"


void Chip8::OP_00E0() {
	for (auto row : video) {
		row.fill(0);
	}
}

void Chip8::OP_00EE() {
	--sp;
	pc = stack[sp];
}

void Chip8::OP_1nnn() {
	uint16_t address = extract_nnn(opcode);
	pc = address;
}

void Chip8::OP_2nnn() {
	uint16_t address = extract_nnn(opcode);

	stack[sp] = pc;
	++sp;
	pc = address;
}

void Chip8::OP_3xkk() {
	uint8_t Vx = extract_x(opcode);
	uint8_t byte = extract_kk(opcode);

	if (registers.at(Vx) == byte) {
		pc += 2;
	}
}

void Chip8::OP_4xkk() {
	uint8_t Vx = extract_x(opcode);
	uint8_t byte = extract_kk(opcode);

	if (registers.at(Vx) != byte) {
		pc += 2;
	}
}

void Chip8::OP_5xy0() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);

	if (registers.at(Vx) == registers.at(Vy)) {
		pc += 2;
	}
}

void Chip8::OP_6xkk() {
	uint8_t Vx = extract_x(opcode);
	uint8_t kk = extract_kk(opcode);
	registers.at(Vx) = kk;
}

void Chip8::OP_7xkk() {
	uint8_t Vx = extract_x(opcode);
	uint8_t kk = extract_kk(opcode);
	registers.at(Vx) += kk;
}

void Chip8::OP_8xy0() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);
	registers.at(Vx) = registers.at(Vy);
}

void Chip8::OP_8xy1() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);
	registers.at(Vx) |= registers.at(Vy);
}

void Chip8::OP_8xy2() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);
	registers.at(Vx) &= registers.at(Vy);
}

void Chip8::OP_8xy3() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);
	registers.at(Vx) ^= registers.at(Vy);
}

void Chip8::OP_8xy4() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);

	uint16_t sum = registers.at(Vx) + registers.at(Vy);
	registers[0xF] = (sum > 255U) ? 1 : 0;
	registers.at(Vx) = sum & 0xFF;
}

void Chip8::OP_8xy5() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);

	registers[0xF] = registers.at(Vx) > registers.at(Vy) ? 1 : 0;

	uint8_t diff = static_cast<uint8_t>(registers.at(Vx) - registers.at(Vy));
	registers.at(Vx) = diff;
}

void Chip8::OP_8xy6() {
	uint8_t Vx = extract_x(opcode);

	registers[0xF] = (registers.at(Vx) & 0x1) ? 1 : 0;

	registers.at(Vx) = registers.at(Vx) >> 1;
}


void Chip8::OP_8xy7() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);

	registers[0xF] = registers.at(Vy) > registers.at(Vx) ? 1 : 0;

	uint8_t diff = static_cast<uint8_t>(registers.at(Vy) - registers.at(Vx));
	registers.at(Vx) = diff;
}

void Chip8::OP_8xyE() {
	uint8_t Vx = extract_x(opcode);

	registers[0xF] = (registers.at(Vx) & 0x80) >> 8 ? 1 : 0;

	registers.at(Vx) = registers.at(Vx) << 1;
}

void Chip8::OP_9xy0() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);

	if (registers.at(Vx) != registers.at(Vy)) {
		pc += 2;
	}
}

void Chip8::OP_Annn() {
	uint16_t addr = extract_nnn(opcode);
	index = addr;
}

void Chip8::OP_Bnnn() {
	uint16_t addr = extract_nnn(opcode);
	pc += static_cast<uint16_t>(registers[0]) + addr;
}

void Chip8::OP_Cxkk() {
	uint8_t Vx = extract_x(opcode);
	uint8_t byte = extract_kk(opcode);
	registers.at(Vx) = generate_random_byte() & byte;
}

void Chip8::OP_Dxyn() {
	uint8_t Vx = extract_x(opcode);
	uint8_t Vy = extract_y(opcode);
	uint8_t height = extract_n(opcode);
	uint8_t width = 8; // We know sprites are 8 bits wide

	// Wrap if going beyond screen boundaries
	uint8_t x_start_pos = registers.at(Vx) % VIDEO_WIDTH;
	uint8_t y_start_pos = registers.at(Vy) % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (int row = 0; row < height; ++row) {
		uint8_t sprite_byte = memory[index + row];

		for (int col = 0; col < 8; ++col) {
			// Shift through each bit in the row, left to right
			uint8_t sprite_bit = sprite_byte & (0x80u >> col);
			uint8_t pixel_y = (y_start_pos + row) % VIDEO_HEIGHT;
			uint8_t pixel_x = (x_start_pos + col) % VIDEO_WIDTH;
			uint32_t* pixel = &video.at(pixel_y).at(pixel_x);

			// Sprite bit is on
			if (sprite_bit) {
				// Will erase pixel, set VF
				if (*pixel == PIXEL_ON) {
					registers[0xF] = 1;
				}

				// xor the pixel
				*pixel ^= PIXEL_ON;
			}
		}
	}
}

void Chip8::OP_Ex9E() {
	uint8_t Vx = extract_x(opcode);

	const char key = get_current_key();

	if (key == registers.at(Vx)) {
		pc += 2;
	}
}

void Chip8::OP_ExA1() {
	uint8_t Vx = extract_x(opcode);

	const char key = get_current_key();

	if (key != registers.at(Vx)) {
		pc += 2;
	}
}

void Chip8::OP_Fx07() {
	uint8_t Vx = extract_x(opcode);
	registers.at(Vx) = delay_timer;
}

void Chip8::OP_Fx0A() {
	uint8_t Vx = extract_x(opcode);
	uint8_t key = wait_keypress();
	registers.at(Vx) = key;
}

void Chip8::OP_Fx15() {
	uint8_t Vx = extract_x(opcode);
	delay_timer = registers.at(Vx);
}

void Chip8::OP_Fx18() {
	uint8_t Vx = extract_x(opcode);
	sound_timer = registers.at(Vx);
}

void Chip8::OP_Fx1E() {
	uint8_t Vx = extract_x(opcode);
	index += registers.at(Vx);
}

void Chip8::OP_Fx29() {
	uint8_t Vx = extract_x(opcode);
	index = registers.at(Vx);
}

void Chip8::OP_Fx33() {
	uint8_t Vx = extract_x(opcode);
	uint8_t value = registers.at(Vx);

	uint8_t hundreds = value / 100;
	uint8_t tens = (value - (hundreds * 100)) / 10;
	uint8_t	ones = value % 10;

	memory.at(index) = hundreds;
	memory.at(index + 1) = tens;
	memory.at(index + 2) = ones;
}

void Chip8::OP_Fx55() {
	// only copy up to MEMSIZE if OOB
	size_t max_copy = std::min(BUF_LEN, MEM_SIZE - index);

	for (size_t i = 0; i < max_copy; ++i) {
		memory.at(index + i) = registers.at(i);
	}
}

void Chip8::OP_Fx65() {
	// only copy up to MEMSIZE if OOB
	size_t max_copy = std::min(BUF_LEN, MEM_SIZE - index);

	for (size_t i = 0; i < max_copy; ++i) {
		registers.at(i) = memory.at(index + i);
	}
}
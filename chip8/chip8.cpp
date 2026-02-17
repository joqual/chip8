#include "chip8.hpp"

Chip8::Chip8() {
	// Init
	pc = ROM_START_ADDRESS;

	// Load fonts
	for (int i = 0; i < FONTSET_SIZE; ++i) {
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	// Init RNG
	using clock = std::chrono::system_clock;
	gen.seed(static_cast<unsigned int>(clock::now().time_since_epoch().count()));
	dist = std::uniform_int_distribution<int>(0, 255U);

	// TODO: Init SDL
}

uint8_t Chip8::generate_random_byte() {
	return dist(gen);
}

uint16_t Chip8::extract_nnn(uint16_t opcode) {
	return opcode & 0x0FFF;
}

uint8_t Chip8::extract_n(uint16_t opcode) {
	return opcode & 0x000F;
}

uint8_t Chip8::extract_x(uint16_t opcode) {
	return (opcode & 0x0F00) >> 8;;
}

uint8_t Chip8::extract_y(uint16_t opcode) {
	return (opcode & 0x00F0) >> 4;
}

uint8_t Chip8::extract_kk(uint16_t opcode) {
	return opcode & 0x00FF;
}

// Peripherals
const char Chip8::get_current_key() {
	for (auto key : keypad) {
		if (key) {
			return static_cast<char>(key);
		}
	}
	return '0';
}

const char Chip8::wait_keypress() {
	// TODO
	return 0;
}

void Chip8::load_ROM(const char* filename) {
	// Open file as binary stream, and move fptr to end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		// Get file size and allocate buf
		std::streamsize size = file.tellg();
		std::vector<char> buf(static_cast<size_t>(size));

		// Seek to beginning and fill buffer
		file.seekg(0, std::ios::beg);
		file.read(buf.data(), size);
		file.close();

		// Load ROM into Chip8
		for (int i = 0; i < size; ++i) {
			memory[ROM_START_ADDRESS + i] = buf[i];
		}
	}
}

void Chip8::cycle() {
	// Fetch instruction
	opcode = memory.at(pc) << 8u | memory.at(pc + 1);

	pc += 2;

	decode_execute();

	if (delay_timer > 0) {
		--delay_timer;
	}

	if (sound_timer) {
		--sound_timer;
	}
}

void Chip8::decode_execute() {
	std::invoke(ops_table[opcode >> 12], *this);
}

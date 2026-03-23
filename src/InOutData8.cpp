// InOutData8.cpp
#include "../include/InOutData8.hpp"

// Инициализация статической переменной
InOutData8::OutputFormat InOutData8::default_output_format = 
	InOutData8::OutputFormat::HEX_BIG_ENDIAN;

// -------------------------------------------------
// Конструкторы
// -------------------------------------------------
InOutData8::InOutData8() {
	clear();
}

InOutData8::InOutData8(const uint8_t data[1]) {
	if (data) {
		bytes[0] = data[0];
	} else {
		clear();
	}
}

InOutData8::InOutData8(uint8_t value) : packed(value) {
}

InOutData8::InOutData8(const std::vector<uint8_t>& data, 
					   size_t start_idx) {
	if (data.empty() || start_idx >= data.size()) {
		clear();
		return;
	}
	bytes[0] = data[start_idx];
}

InOutData8::InOutData8(const InOutData8& other) {
	bytes[0] = other.bytes[0];
}

InOutData8::InOutData8(InOutData8&& other) noexcept {
	bytes[0] = other.bytes[0];
	other.clear();
}

// -------------------------------------------------
// Операторы присваивания
// -------------------------------------------------
InOutData8& InOutData8::operator=(const InOutData8& other) {
	if (this != &other) {
		bytes[0] = other.bytes[0];
	}
	return *this;
}

InOutData8& InOutData8::operator=(InOutData8&& other) noexcept {
	if (this != &other) {
		bytes[0] = other.bytes[0];
		other.clear();
	}
	return *this;
}


// -------------------------------------------------
// Базовые методы
// -------------------------------------------------
void InOutData8::clear() {
	packed = 0;
}

uint8_t InOutData8::get_byte() const {
	return bytes[0];
}

void InOutData8::set_byte(uint8_t value) {
	bytes[0] = value;
}

uint8_t InOutData8::get_packed() const {
	return packed;
}

void InOutData8::set_packed(uint8_t value) {
	packed = value;
}

uint32_t InOutData8::get_word(int index) const {
	if (index == 0) return words[0];
	cerr << "InOutData8::get_word: Invalid index " << index << endl;
	return 0;
}

void InOutData8::set_word(int index, uint32_t value) {
	if (index == 0) {
		words[0] = value;
	} else {
		cerr << "InOutData8::set_word: Invalid index " << index << endl;
	}
}

// -------------------------------------------------
// Методы установки из байтовых массивов
// -------------------------------------------------
void InOutData8::set_from_bytes(const uint8_t* data, 
								size_t len, 
								size_t offset) {
	if (!data || len == 0) return;
	if (offset >= NUM_BYTES) return;
	
	size_t copy_len = std::min(len, static_cast<size_t>(NUM_BYTES) - offset);
	if (copy_len > 0) {
		bytes[offset] = data[0];
	}
}

// -------------------------------------------------
// Методы работы с векторами
// -------------------------------------------------
std::vector<uint8_t> InOutData8::to_vector() const {
	return std::vector<uint8_t>{bytes[0]};
}
// Реализация нестатического метода
void InOutData8::from_vector(const std::vector<uint8_t>& vec,
                             size_t start_idx) {
    if (vec.empty() || start_idx >= vec.size()) {
        clear();
        return;
    }
    bytes[0] = vec[start_idx];
}

// -------------------------------------------------
// Побитовые операции
// -------------------------------------------------
bool InOutData8::get_bit(int bit_index) const {
	if (bit_index < 0 || bit_index >= WIDTH) {
		cerr << "InOutData8::get_bit: Invalid bit index " << bit_index << endl;
		return false;
	}
	return (packed >> bit_index) & 0x01;
}

void InOutData8::set_bit(int bit_index, bool value) {
	if (bit_index < 0 || bit_index >= WIDTH) {
		cerr << "InOutData8::set_bit: Invalid bit index " << bit_index << endl;
		return;
	}
	
	if (value) {
		packed |= (1u << bit_index);
	} else {
		packed &= ~(1u << bit_index);
	}
}
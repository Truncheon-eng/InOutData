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
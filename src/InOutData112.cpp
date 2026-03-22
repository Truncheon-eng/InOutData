#include "../include/InOutData112.hpp"

// Инициализация статической переменной
InOutData112::OutputFormat InOutData112::default_output_format = 
	InOutData112::OutputFormat::HEX_BIG_ENDIAN;

// -------------------------------------------------
// Конструкторы
// -------------------------------------------------
InOutData112::InOutData112() {
	clear();
}

InOutData112::InOutData112(const uint32_t data[4]) {
	for (int i = 0; i < 3; i++) words[i] = data[i];
	w3 = data[3] & MASK_LAST_WORD;
}

InOutData112::InOutData112(uint32_t word0, uint32_t word1, 
						   uint32_t word2, uint32_t word3) 
	: w0(word0), w1(word1), w2(word2), w3(word3 & MASK_LAST_WORD) {}

InOutData112::InOutData112(const uint8_t* data, 
						   size_t len,
						   InitOrder order) {
	if (order == InitOrder::LITTLE_ENDIAN_IN) {
		set_from_bytes_little_endian(data, len);
	} else {
		set_from_bytes_big_endian(data, len);
	}
	apply_mask_internal();
}

InOutData112::InOutData112(const std::vector<uint8_t>& data,
						   InitOrder order) {
	if (order == InitOrder::LITTLE_ENDIAN_IN) {
		from_vector_little_endian(data);
	} else {
		from_vector_big_endian(data);
	}
	apply_mask_internal();
}

InOutData112::InOutData112(const InOutData112& other) {
	std::memcpy(bytes, other.bytes, NUM_BYTES);
	apply_mask_internal();
}

InOutData112::InOutData112(InOutData112&& other) noexcept {
	std::memcpy(bytes, other.bytes, NUM_BYTES);
	apply_mask_internal();
	other.clear();
}

// -------------------------------------------------
// Операторы присваивания
// -------------------------------------------------
InOutData112& InOutData112::operator=(const InOutData112& other) {
	if (this != &other) {
		std::memcpy(bytes, other.bytes, NUM_BYTES);
		apply_mask_internal();
	}
	return *this;
}

InOutData112& InOutData112::operator=(InOutData112&& other) noexcept {
	if (this != &other) {
		std::memcpy(bytes, other.bytes, NUM_BYTES);
		apply_mask_internal();
		other.clear();
	}
	return *this;
}

// -------------------------------------------------
// Базовые методы
// -------------------------------------------------
void InOutData112::clear() {
	std::memset(bytes, 0, NUM_BYTES);
}

void InOutData112::apply_mask_internal() {
	w3 &= MASK_LAST_WORD;
}

void InOutData112::apply_mask() {
	apply_mask_internal();
}

bool InOutData112::is_masked_correctly() const {
	return (w3 & ~MASK_LAST_WORD) == 0;
}
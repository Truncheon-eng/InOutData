// InOutData320.cpp
#include "../include/InOutData320.hpp"

// конструктор по умолчанию
InOutData320::InOutData320() {
	clear();
}

// конструктор на основе C-го массива
InOutData320::InOutData320(const uint32_t data[10]) {
	for (int i = 0; i < 10; i++) words[i] = data[i];
}

// конструктор на основе 10 32-ых слов
InOutData320::InOutData320(uint32_t word0, uint32_t word1, 
						   uint32_t word2, uint32_t word3,
						   uint32_t word4, uint32_t word5,
						   uint32_t word6, uint32_t word7,
						   uint32_t word8, uint32_t word9) 
	: w0(word0), w1(word1), w2(word2), w3(word3),
	  w4(word4), w5(word5), w6(word6), w7(word7),
	  w8(word8), w9(word9) {}

// конструктор с выбором порядка      
InOutData320::InOutData320(const uint8_t* data, 
						   size_t len,
						   InitOrder order) {
	if (order == InitOrder::LITTLE_ENDIAN_IN) {
		set_from_bytes_little_endian(data, len);
	} else {
		set_from_bytes_big_endian(data, len);
	}
}

// конструктор с выбором порядка на основе динамического массива
InOutData320::InOutData320(const std::vector<uint8_t>& data,
						   InitOrder order) {
	if (order == InitOrder::LITTLE_ENDIAN_IN) {
		from_vector_little_endian(data);
	} else {
		from_vector_big_endian(data);
	}
}

// конструктор копирования
InOutData320::InOutData320(const InOutData320& other) {
	std::memcpy(bytes, other.bytes, NUM_BYTES);
}

// конструктор перемещения
InOutData320::InOutData320(InOutData320&& other) noexcept {
	std::memcpy(bytes, other.bytes, NUM_BYTES);
	other.clear();
}

// -------------------------------------------------
// Операторы присваивания
// -------------------------------------------------
InOutData320& InOutData320::operator=(const InOutData320& other) {
	if (this != &other) {
		std::memcpy(bytes, other.bytes, NUM_BYTES);
	}
	return *this;
}

// -------------------------------------------------
// Операторы перемещающего присваивания
// -------------------------------------------------
InOutData320& InOutData320::operator=(InOutData320&& other) noexcept {
	if (this != &other) {
		std::memcpy(bytes, other.bytes, NUM_BYTES);
		other.clear();
	}
	return *this;
}

// -------------------------------------------------
// Методы очистки и доступа
// -------------------------------------------------

void InOutData320::clear() {
	std::memset(bytes, 0, NUM_BYTES);
}


void InOutData320::from_vector_little_endian(const std::vector<uint8_t>& vec, 
											 size_t start_idx) {
	if (vec.empty() || start_idx >= vec.size()) {
		clear();
		return;
	}
	
	size_t bytes_to_copy = std::min(
		static_cast<size_t>(NUM_BYTES),
		vec.size() - start_idx
	);
	
	std::memcpy(bytes, vec.data() + start_idx, bytes_to_copy);
	
	if (bytes_to_copy < NUM_BYTES) {
		std::memset(bytes + bytes_to_copy, 0, NUM_BYTES - bytes_to_copy);
	}
}

void InOutData320::from_vector_big_endian(const std::vector<uint8_t>& vec, 
										  size_t start_idx) {
	if (vec.empty() || start_idx >= vec.size()) {
		clear();
		return;
	}

	size_t bytes_to_copy = std::min(static_cast<size_t>(NUM_BYTES),
									vec.size() - start_idx
	);
	
	// Конвертируем big-endian в little-endian
	for (size_t i = 0; i < bytes_to_copy; i++) {
		bytes[bytes_to_copy - 1 - i] = vec[start_idx + i];
	}
	
	if (bytes_to_copy < NUM_BYTES) {
		std::memset(bytes + bytes_to_copy, 0, NUM_BYTES - bytes_to_copy);
	}
}

// установка последовательности в соотв. с little-endian нотацией
void InOutData320::set_from_bytes_little_endian(const uint8_t* data, 
												size_t len, 
												size_t offset) {
	if (!data || len == 0) return;
	if (offset >= static_cast<size_t>(NUM_BYTES)) return;
	
	size_t copy_len = std::min(len, static_cast<size_t>(NUM_BYTES) - offset);
	if (copy_len > 0) {
		std::memcpy(bytes + offset, data, copy_len);
	}
}

// установка последовательности в соотв. с big-endian нотацией
void InOutData320::set_from_bytes_big_endian(const uint8_t* data, 
											 size_t len, 
											 size_t offset) {
	if (!data || len == 0) return;
	if (offset >= static_cast<size_t>(NUM_BYTES)) return;
	
	size_t copy_len = std::min(len, static_cast<size_t>(NUM_BYTES) - offset);
	if (copy_len > 0) {
		convert_big_to_little_endian(bytes + offset, data, copy_len);
	}
}

void InOutData320::convert_big_to_little_endian(uint8_t* dest, 
												const uint8_t* src, 
												size_t len) {
	for (size_t i = 0; i < len; i++) {
		dest[i] = src[len - 1 - i];
	}
}

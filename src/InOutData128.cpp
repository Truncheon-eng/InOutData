#include "../include/InOutData128.hpp"

// -------------------------------------------------
// Конструкторы
// -------------------------------------------------
InOutData128::InOutData128() {
	clear();
}

InOutData128::InOutData128(const uint32_t data[4]) {
	for (int i = 0; i < NUM_WORDS; i++) words[i] = data[i];
}

InOutData128::InOutData128(uint32_t word0, uint32_t word1, 
						   uint32_t word2, uint32_t word3) 
	: w0(word0), w1(word1), 
	  w2(word2), w3(word3) {}

InOutData128::InOutData128(const uint8_t* data, size_t len) {
	set_from_bytes(data, len);
}

InOutData128::InOutData128(const std::vector<uint8_t>& data) {
	from_vector(data);
}

// Конструктор копирования
InOutData128::InOutData128(const InOutData128& other) {
	std::memcpy(bytes, other.bytes, NUM_BYTES);
}

// Move конструктор
InOutData128::InOutData128(InOutData128&& other) noexcept {
	std::memcpy(bytes, other.bytes, NUM_BYTES);
	other.clear();
}

// -------------------------------------------------
// Операторы присваивания
// -------------------------------------------------
InOutData128& InOutData128::operator=(const InOutData128& other) {
	if (this != &other) {
		std::memcpy(bytes, other.bytes, NUM_BYTES);
	}
	return *this;
}

InOutData128& InOutData128::operator=(InOutData128&& other) noexcept {
	if (this != &other) {
		std::memcpy(bytes, other.bytes, NUM_BYTES);
		other.clear();
	}
	return *this;
}

// -------------------------------------------------
// Методы очистки и доступа
// -------------------------------------------------
void InOutData128::clear() {
	std::memset(bytes, 0, NUM_BYTES);
}

// -------------------------------------------------
// методы установки байт/бит
// -------------------------------------------------

void InOutData128::set_from_bytes(const uint8_t* data, 
								  size_t len, 
								  size_t offset) {
	if (!data || len == 0) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " index error !\t" << NORMAL << endl;
		return;
	}
	if (offset >= static_cast<size_t>(NUM_BYTES)) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " index error !\t" << NORMAL << endl;
		return;
	}
	size_t copy_len = std::min(len, static_cast<size_t>(NUM_BYTES) - offset);
	if (copy_len > 0) {
		std::memcpy(bytes + offset, data, copy_len);
	}
}

// -------------------------------------------------
// Работа с векторами
// -------------------------------------------------

InOutData128& InOutData128::from_vector(const std::vector<uint8_t>& vec, 
								size_t start_idx,
								InitOrder order){
	if (order == InitOrder::LITTLE_ENDIAN_IN) {
		from_vector_little_endian(vec, start_idx);
	} else {
		from_vector_big_endian(vec, start_idx);
	}
	return *this;  // Возвращаем себя для цепочного вызова
}


void InOutData128::from_vector_big_endian(const std::vector<uint8_t>& vec, 
										  size_t start_idx) {
	check_vector_bounds(vec, start_idx, "from_vector_big_endian");
	
	// Конвертируем big-endian в little-endian
	for (size_t i = 0; i < NUM_BYTES; i++) {
		size_t vec_idx = start_idx + i;
		size_t mem_idx = NUM_BYTES - 1 - i;
		bytes[mem_idx] = vec[vec_idx];
	}
}

void InOutData128::from_vector_little_endian(const std::vector<uint8_t>& vec,
											 size_t start_idx) {
	check_vector_bounds(vec, start_idx, "from_vector_little_endian");
	
	// Прямое копирование
	std::memcpy(bytes, vec.data() + start_idx, NUM_BYTES);
}

bool InOutData128::check_vector_bounds(const std::vector<uint8_t>& vec,
									   size_t start_idx,
									   const char* method_name) const {
	if (vec.empty()) {
		throw std::invalid_argument(std::string(method_name) + 
								   ": Cannot initialize from empty vector");
	}
	
	if (start_idx >= vec.size()) {
		throw std::out_of_range(std::string(method_name) + 
							   ": start_idx (" + std::to_string(start_idx) + 
							   ") exceeds vector size (" + 
							   std::to_string(vec.size()) + ")");
	}
	
	if (vec.size() - start_idx < static_cast<size_t>(NUM_BYTES)) {
		throw std::length_error(std::string(method_name) + 
							   ": Vector has insufficient data (" + 
							   std::to_string(vec.size() - start_idx) + 
							   " bytes) to fill " + 
							   std::to_string(NUM_BYTES) + " bytes");
	}
	return true;
}
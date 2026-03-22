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

bool InOutData112::is_masked_correctly() const {
	return (w3 & ~MASK_LAST_WORD) == 0;
}

uint8_t InOutData112::get_byte(int index) const {
	if (index < 0 || index >= NUM_BYTES) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " byte index error! Index=" << index 
			 << ", valid range: 0-" << (NUM_BYTES-1) << NORMAL << endl;
		return 0;
	}
	return bytes[index];
}

void InOutData112::set_byte(int index, uint8_t value) {
	if (index < 0 || index >= NUM_BYTES) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " byte index error! Index=" << index 
			 << ", valid range: 0-" << (NUM_BYTES-1) << NORMAL << endl;
		return;
	}
	bytes[index] = value;
	
	// Если изменили байты 12 или 13, применяем маску
	if (index >= 12) {
		apply_mask_internal();
	}
}

uint32_t InOutData112::get_word(int index) const {
	if (index < 0 || index >= NUM_WORDS) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " word index error! Index=" << index 
			 << ", valid range: 0-" << (NUM_WORDS-1) << NORMAL << endl;
		return 0;
	}
	
	if (index == 3) {
		return w3 & MASK_LAST_WORD;
	}
	return words[index];
}

void InOutData112::set_word(int index, uint32_t value) {
	if (index < 0 || index >= NUM_WORDS) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " word index error! Index=" << index 
			 << ", valid range: 0-" << (NUM_WORDS-1) << NORMAL << endl;
		return;
	}
	
	if (index == 3) {
		w3 = value & MASK_LAST_WORD;
	} else {
		words[index] = value;
	}
}


uint32_t InOutData112::get_word_little_endian(int index) const {
	return get_word(index);
}

uint32_t InOutData112::get_word_big_endian(int index) const {
	if (index >= 0 && index < NUM_WORDS) {
		if (index == 3) {
			return swap_endian(w3 & MASK_LAST_WORD);
		}
		return swap_endian(words[index]);
	}
	return 0;
}

void InOutData112::set_word_little_endian(int index, uint32_t value) {
	set_word(index, value);
}

void InOutData112::set_word_big_endian(int index, uint32_t value) {
	if (index >= 0 && index < NUM_WORDS) {
		if (index == 3) {
			w3 = swap_endian(value) & MASK_LAST_WORD;
		} else {
			words[index] = swap_endian(value);
		}
	}
}

void InOutData112::set_all_words(uint32_t word0, uint32_t word1, 
								 uint32_t word2, uint32_t word3) {
	w0 = word0;
	w1 = word1;
	w2 = word2;
	w3 = word3 & MASK_LAST_WORD;
}

// -------------------------------------------------
// Методы установки из байтовых массивов
// -------------------------------------------------
void InOutData112::set_from_bytes(const uint8_t* data, 
								  size_t len, 
								  size_t offset) {
	set_from_bytes_little_endian(data, len, offset);
}

void InOutData112::set_from_bytes_little_endian(const uint8_t* data, 
												size_t len, 
												size_t offset) {
	if (!data || len == 0) return;
	if (offset >= static_cast<size_t>(NUM_BYTES)) return;
	
	size_t copy_len = std::min(len, static_cast<size_t>(NUM_BYTES) - offset);
	if (copy_len > 0) {
		std::memcpy(bytes + offset, data, copy_len);
		
		// Если затронули последние 2 байта, применяем маску
		if (offset + copy_len > 12) {
			apply_mask_internal();
		}
	}
}

void InOutData112::set_from_bytes_big_endian(const uint8_t* data, 
											 size_t len, 
											 size_t offset) {
	if (!data || len == 0) return;
	if (offset >= static_cast<size_t>(NUM_BYTES)) return;
	
	size_t copy_len = std::min(len, static_cast<size_t>(NUM_BYTES) - offset);
	if (copy_len > 0) {
		convert_big_to_little_endian(bytes + offset, data, copy_len);
		
		// Если затронули последние 2 байта, применяем маску
		if (offset + copy_len > 12) {
			apply_mask_internal();
		}
	}
}

// -------------------------------------------------
// Методы работы с векторами
// -------------------------------------------------
void InOutData112::from_vector(const std::vector<uint8_t>& vec, 
							   size_t start_idx) {
	from_vector_little_endian(vec, start_idx);
}

void InOutData112::from_vector_little_endian(const std::vector<uint8_t>& vec, 
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
	
	apply_mask_internal();
}

void InOutData112::from_vector_big_endian(const std::vector<uint8_t>& vec, 
										  size_t start_idx) {
	if (vec.empty() || start_idx >= vec.size()) {
		clear();
		return;
	}
	
	size_t bytes_to_copy = std::min(
		static_cast<size_t>(NUM_BYTES),
		vec.size() - start_idx
	);
	
	// Конвертируем big-endian в little-endian
	for (size_t i = 0; i < bytes_to_copy; i++) {
		bytes[bytes_to_copy - 1 - i] = vec[start_idx + i];
	}
	
	if (bytes_to_copy < NUM_BYTES) {
		std::memset(bytes + bytes_to_copy, 0, NUM_BYTES - bytes_to_copy);
	}
	
	apply_mask_internal();
}

uint32_t InOutData112::swap_endian(uint32_t value) {
	return ((value & 0x000000FF) << 24) |
		   ((value & 0x0000FF00) << 8) |
		   ((value & 0x00FF0000) >> 8) |
		   ((value & 0xFF000000) >> 24);
}
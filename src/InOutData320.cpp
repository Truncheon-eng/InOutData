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

// получение байта
uint8_t InOutData320::get_byte(int index) const {
	if (index < 0 || index >= NUM_BYTES) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " byte index error! Index=" << index 
			 << ", valid range: 0-" << (NUM_BYTES-1) << NORMAL << endl;
		return 0;
	}
	return bytes[index];
}

// установка байта
void InOutData320::set_byte(int index, 
							uint8_t value) {
	if (index < 0 || index >= NUM_BYTES) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " byte index error! Index=" << index 
			 << ", valid range: 0-" << (NUM_BYTES-1) << NORMAL << endl;
		return;
	}
	bytes[index] = value;
}


// получение 4 байтов (одного слова)
uint32_t InOutData320::get_word(int index) const { 
	if (index < 0 || index >= NUM_WORDS) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " word index error! Index=" << index 
			 << ", valid range: 0-" << (NUM_WORDS-1) << NORMAL << endl;
		return 0;
	}
	return words[index];
}

// установка 4 байтов (одного слова)
void InOutData320::set_word(int index, 
							uint32_t value) {
	if (index < 0 || index >= NUM_WORDS) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " word index error! Index=" << index 
			 << ", valid range: 0-" << (NUM_WORDS-1) << NORMAL << endl;
		return;
	}
	words[index] = value;
}


// изменение порядка байт с little-endian на big-endian

uint32_t InOutData320::swap_endian(uint32_t value) {
	return ((value & 0x000000FF) << 24) |
		   ((value & 0x0000FF00) << 8) |
		   ((value & 0x00FF0000) >> 8) |
		   ((value & 0xFF000000) >> 24);
}

// -------------------------------------------------
// Методы доступа к словам с указанием порядка
// -------------------------------------------------
uint32_t InOutData320::get_word_little_endian(int index) const {
	if (index < 0 || index >= NUM_WORDS) {
		cerr << RED << CROSS_MARK << __FUNCTION__ << " index error !\t" 
			 << NORMAL << endl;
		return 0;
	}
	return words[index];
}

uint32_t InOutData320::get_word_big_endian(int index) const {
	if (index < 0 || index >= NUM_WORDS) {
		cerr << RED << CROSS_MARK << __FUNCTION__ << " index error !\t" 
			<< NORMAL << endl;
		return 0;
	}
	return swap_endian(words[index]);
}

void InOutData320::set_word_little_endian(int index, 
										  uint32_t value) {
	if (index >= 0 && index < NUM_WORDS) {
		words[index] = value;
	}
}

void InOutData320::set_word_big_endian(int index, 
									   uint32_t value) {
	if (index >= 0 && index < NUM_WORDS) {
		words[index] = swap_endian(value);
	}
}

bool InOutData320::get_bit(int bit_index) const {
	if (bit_index < 0 || bit_index >= WIDTH) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " bit index error! Index=" << bit_index 
			 << ", valid range: 0-" << (WIDTH-1) << NORMAL << endl;
		return false;
	}

	int byte_idx = bit_index / 8;
	int bit_in_byte = bit_index % 8;
	return (bytes[byte_idx] >> bit_in_byte) & 0x01;
}

void InOutData320::set_bit(int bit_index, bool value) {
	if (bit_index < 0 || bit_index >= WIDTH) return;
	
	int byte_idx = bit_index / 8;
	int bit_in_byte = bit_index % 8;
	
	if (value) {
		bytes[byte_idx] |= (1u << bit_in_byte);
	} else {
		bytes[byte_idx] &= ~(1u << bit_in_byte);
	}
}

void InOutData320::set_default_output_format(OutputFormat format) {
	default_output_format = format;
}

// -------------------------------------------------
// Методы вывода
// -------------------------------------------------
void InOutData320::print(const char* name) const {
	std::cout << name << " InOutData320 (320 bits, LITTLE-ENDIAN):" << std::endl;
	
	std::cout << "  Memory words: { ";
	for (int i = 0; i < NUM_WORDS; i++) {
		std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') 
				  << words[i];
		if (i < 9) std::cout << ", ";
		if ((i + 1) % 4 == 0 && i < 9) std::cout << std::endl << "                ";
	}
	std::cout << " }" << std::dec << std::endl;
}

void InOutData320::print_bytes(const char* name) const {
	std::cout << name << " Bytes (little-endian):" << std::endl;
	std::cout << "  ";
	for (int i = 0; i < NUM_BYTES; i++) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') 
				  << static_cast<int>(bytes[i]);
		if (i < NUM_BYTES - 1) {
			std::cout << " ";
			if ((i + 1) % 16 == 0) std::cout << std::endl << "  ";
			else if ((i + 1) % 8 == 0) std::cout << "  ";
		}
	}
	std::cout << std::dec << std::endl;
}

void InOutData320::print_words_and_bytes(const char* name) const {
	std::cout << name << " Words and bytes:" << std::endl;
	for (int i = 0; i < NUM_WORDS; i++) {
		std::cout << "  w" << i << " = 0x" << std::hex << std::setw(8) 
				  << std::setfill('0') << words[i] << std::dec
				  << "  bytes[" << (i*4) << "-" << (i*4+3) << "] = ";
		for (int j = 0; j < NUM_BYTES; j++) {
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
					  << static_cast<int>(bytes[i*4 + j]);
			if (j < 3) std::cout << " ";
		}
		std::cout << std::dec << std::endl;
	}
}

void InOutData320::print_detailed(const char* name) const {
	std::cout << "\n=== " << name << " ===" << std::endl;
	
	std::cout << "Memory layout (little-endian):" << std::endl;
	std::cout << "  Bytes: ";
	for (int i = 0; i < NUM_BYTES; i++) {
		std::cout << "[" << std::setw(2) << i << "]=0x" 
				  << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(bytes[i]) << std::dec;
		if (i < NUM_BYTES - 1) std::cout << " ";
		if ((i + 1) % 8 == 0 && i < NUM_BYTES - 1) std::cout << std::endl << "         ";
	}
	std::cout << std::endl;
	
	std::cout << "\n32-bit words in memory:" << std::endl;
	for (int i = 0; i < NUM_WORDS; i++) {
		std::cout << "  word[" << i << "] = 0x" << std::hex << std::setw(8) 
				  << std::setfill('0') << get_word_little_endian(i) << std::dec
				  << "  (BE: 0x" << std::hex << std::setw(8) 
				  << get_word_big_endian(i) << ")" << std::dec << std::endl;
	}
	
	std::cout << "\nString representations:" << std::endl;
	std::cout << "  Hex BE:      " << to_hex_string(OutputFormat::HEX_BIG_ENDIAN) << std::endl;
	std::cout << "  Hex LE:      " << to_hex_string(OutputFormat::HEX_LITTLE_ENDIAN) << std::endl;
}

void InOutData320::print_raw_memory(const char* name) const {
	std::cout << name;
	if (strlen(name) > 0) std::cout << ": ";
	
	std::cout << "Raw bytes in memory: ";
	for (int i = 0; i < NUM_BYTES; i++) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(bytes[i]);
		if (i < NUM_BYTES - 1) std::cout << " ";
		if ((i + 1) % 16 == 0 && i < NUM_BYTES - 1) std::cout << std::endl << "                     ";
	}
	std::cout << std::dec << std::endl;
}

// -------------------------------------------------
// Строковые представления
// -------------------------------------------------
std::string InOutData320::to_hex_string() const {
	return to_hex_string(default_output_format);
}

std::string InOutData320::to_hex_string(OutputFormat format) const {
	std::stringstream ss;
	
	switch (format) {
		case OutputFormat::HEX_BIG_ENDIAN:
			ss << "0x";
			for (int i = NUM_BYTES - 1; i >= 0; i--) {
				ss << std::hex << std::setw(2) << std::setfill('0') 
				   << static_cast<int>(bytes[i]);
			}
			break;
			
		case OutputFormat::HEX_LITTLE_ENDIAN:
			ss << "0x";
			for (int i = 0; i < NUM_BYTES; i++) {
				ss << std::hex << std::setw(2) << std::setfill('0') 
				   << static_cast<int>(bytes[i]);
			}
			break;
			
		default:
			ss << "0x";
			for (int i = NUM_BYTES - 1; i >= 0; i--) {
				ss << std::hex << std::setw(2) << std::setfill('0') 
				   << static_cast<int>(bytes[i]);
			}
			break;
	}
	
	return ss.str();
}

std::string InOutData320::to_binary_string() const {
	return to_binary_string(
		(default_output_format == OutputFormat::HEX_BIG_ENDIAN || 
		 default_output_format == OutputFormat::HEX_LITTLE_ENDIAN) ?
		OutputFormat::BINARY_BIG_ENDIAN : default_output_format
	);
}

std::string InOutData320::to_binary_string(OutputFormat format) const {
	std::stringstream ss;
	
	switch (format) {
		case OutputFormat::BINARY_BIG_ENDIAN:
			for (int i = NUM_BYTES - 1; i >= 0; i--) {
				ss << std::bitset<8>(bytes[i]);
				if (i > 0) ss << " ";
				if ((NUM_BYTES - i) % 4 == 0 && i > 0) ss << std::endl;
			}
			break;
			
		case OutputFormat::BINARY_LITTLE_ENDIAN:
			for (int i = 0; i < NUM_BYTES; i++) {
				ss << std::bitset<8>(bytes[i]);
				if (i < NUM_BYTES - 1) ss << " ";
				if ((i + 1) % 4 == 0 && i < NUM_BYTES - 1) ss << std::endl;
			}
			break;
			
		default:
			for (int i = NUM_BYTES - 1; i >= 0; i--) {
				ss << std::bitset<8>(bytes[i]);
				if (i > 0) ss << " ";
				if ((NUM_BYTES - i) % 4 == 0 && i > 0) ss << std::endl;
			}
			break;
	}
	
	return ss.str();
}

std::string InOutData320::to_raw_string(bool as_hex) const {
	std::stringstream ss;
	if (as_hex) {
		ss << std::hex << std::setfill('0');
		for (int i = 0; i < NUM_BYTES; i++) {
			ss << std::setw(2) << static_cast<int>(bytes[i]);
			if (i < NUM_BYTES - 1) ss << " ";
			if ((i + 1) % 16 == 0 && i < NUM_BYTES - 1) ss << std::endl;
		}
	} else {
		for (int i = 0; i < NUM_BYTES; i++) {
			ss << std::bitset<8>(bytes[i]);
			if (i < NUM_BYTES - 1) ss << " ";
			if ((i + 1) % 4 == 0 && i < NUM_BYTES - 1) ss << std::endl;
		}
	}
	return ss.str();
}

void InOutData320::print_hex(const char* name) const {
	print_hex(default_output_format, name);
}

void InOutData320::print_hex(OutputFormat format, const char* name) const {
	std::cout << name;
	if (strlen(name) > 0) std::cout << ": ";
	std::cout << to_hex_string(format) << std::endl;
}

void InOutData320::print_binary(const char* name) const {
	print_binary(
		(default_output_format == OutputFormat::HEX_BIG_ENDIAN || 
		 default_output_format == OutputFormat::HEX_LITTLE_ENDIAN) ?
		OutputFormat::BINARY_BIG_ENDIAN : default_output_format,
		name
	);
}

void InOutData320::print_binary(OutputFormat format, const char* name) const {
	std::cout << name;
	if (strlen(name) > 0) std::cout << ": ";
	std::cout << to_binary_string(format) << std::endl;
}


std::array<uint32_t, 10> InOutData320::to_array() const {
	std::array<uint32_t, 10> arr;
	std::copy(words, words + NUM_WORDS, arr.begin());
	return arr;
}

void InOutData320::from_array(const std::array<uint32_t, 10>& arr) {
	std::copy(arr.begin(), arr.end(), words);
}
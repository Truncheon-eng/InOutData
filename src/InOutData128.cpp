// InOutData128.cpp
#include "InOutData128.hpp"

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

uint8_t InOutData128::get_byte(int index) const {
	if (index < 0 || index >= NUM_BYTES) return 0;
	return bytes[index];
}

void InOutData128::set_byte(int index, 
							uint8_t value) {
	if (index < 0 || index >= NUM_BYTES) {
		cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " index error !\t" << NORMAL << endl;
		return;
	};
	bytes[index] = value;
}

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

void InOutData128::set_from_bytes_big_endian(const uint8_t* data, 
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
		convert_big_to_little_endian(bytes + offset, data, copy_len);
	}
}

void InOutData128::convert_big_to_little_endian(uint8_t* dest, const uint8_t* src, size_t len) {
	for (size_t i = 0; i < len; i++) {
		dest[i] = src[len - 1 - i];
	}
}

void InOutData128::set_from_bytes_little_endian(const uint8_t* data, 
												size_t len, 
												size_t offset) {
	set_from_bytes(data, len, offset);
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

// -------------------------------------------------
// Методы для работы с векторами (явный порядок)
// -------------------------------------------------

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

std::vector<uint8_t> InOutData128::to_vector() const {
	return to_vector_little_endian();
}

std::vector<uint8_t> InOutData128::to_vector_little_endian() const {
	return std::vector<uint8_t>(bytes, bytes + NUM_BYTES);
}

std::vector<uint8_t> InOutData128::to_vector_big_endian() const {
	std::vector<uint8_t> result(NUM_BYTES);
	for (size_t i = 0; i < NUM_BYTES; i++) {
		result[NUM_BYTES - 1 - i] = bytes[i];
	}
	return result;
}

// -------------------------------------------------
// Побитовые операции
// -------------------------------------------------
bool InOutData128::get_bit(int bit_index) const {
	if (bit_index < 0 || bit_index >= width) {
	cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " index error !\t" << NORMAL << endl;	
		return false;
	}

	int byte_idx = bit_index / 8;
	int bit_in_byte = bit_index % 8;
	return (bytes[byte_idx] >> bit_in_byte) & 0x01;
}

void InOutData128::set_bit(int bit_index, bool value) {
	if (bit_index < 0 || bit_index >= width){
	cerr << RED << CROSS_MARK << __FUNCTION__ 
				<< " index error !\t" << NORMAL << endl;
	return;
	}

	int byte_idx = bit_index / 8;
	int bit_in_byte = bit_index % 8;
	
	if (value) {
		bytes[byte_idx] |= (1u << bit_in_byte);
	} else {
		bytes[byte_idx] &= ~(1u << bit_in_byte);
	}
}

// -------------------------------------------------
// Методы вывода
// -------------------------------------------------
void InOutData128::print(const char* name) const {
	std::cout << name << " InOutData128 (128 bits, LITTLE-ENDIAN):" << std::endl;
	
	// Little-endian представление (то что в памяти)
	std::cout << "  Memory words (word0 = bytes[0..3]): { ";
	for (int i = 0; i < NUM_WORDS; i++) {
		std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') 
				  << words[i];
		if (i < 3) std::cout << ", ";
	}
	std::cout << " }" << std::dec << std::endl;
	
	// Big-endian представление для сравнения
	std::cout << "  Logical words (word0 = старшие 32 бита): { ";
	for (int i = 3; i >= 0; i--) {
		std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') 
				  << words[i];
		if (i > 0) std::cout << ", ";
	}
	std::cout << " }" << std::dec << std::endl;
}

void InOutData128::print_bytes(const char* name) const {
	std::cout << name << " Bytes (little-endian):" << std::endl;
	for (int i = 0; i < 16; i++) {
		std::cout << "  [" << std::setw(2) << i << "] = 0x" 
				  << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(bytes[i]) << std::dec;
		if ((i + 1) % 4 == 0) std::cout << "  (word " << i/4 << ")";
		std::cout << std::endl;
	}
}

void InOutData128::print_words_and_bytes(const char* name) const {
	std::cout << name << " Words and bytes:" << std::endl;
	for (int i = 0; i < NUM_WORDS; i++) {
		std::cout << "  Word[" << i << "] = 0x" << std::hex << std::setw(8) 
				  << std::setfill('0') << words[i] << std::dec
				  << "  bytes " << (i*4) << "-" << (i*4+3) << ": ";
		for (int j = 0; j < NUM_WORDS; j++) {
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
					  << static_cast<int>(bytes[i*4 + j]);
			if (j < 3) std::cout << " ";
		}
		std::cout << std::dec << std::endl;
	}
}

// Новенькое
std::string InOutData128::to_raw_string(bool as_hex) const {
	std::stringstream ss;
	if (as_hex) {
		ss << std::hex << std::setfill('0');
		for (int i = 0; i < NUM_BYTES; i++) {
			ss << std::setw(2) << static_cast<int>(bytes[i]);
			if (i < NUM_BYTES - 1) ss << " ";
		}
	} else {
		for (int i = 0; i < NUM_BYTES; i++) {
			ss << std::bitset<8>(bytes[i]);
			if (i < NUM_BYTES - 1) ss << " ";
		}
	}
	return ss.str();
}

void InOutData128::print_raw_memory(const char* name) const {
	std::cout << name;
	if (strlen(name) > 0) std::cout << ": ";
	
	std::cout << "Raw bytes in memory: ";
	for (int i = 0; i < NUM_BYTES; i++) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(bytes[i]);
		if (i < NUM_BYTES - 1) std::cout << " ";
	}
	std::cout << std::dec << std::endl;
}

// Или для тестовых векторов:
void InOutData128::print_test_vector_format() const {
	std::cout << "Test vector format: ";
	for (int i = 0; i < NUM_BYTES; i++) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(bytes[i]);
		if (i < NUM_BYTES - 1) {
			std::cout << ", ";
			if ((i + 1) % 8 == 0) std::cout << std::endl << "                    ";
		}
	}
	std::cout << std::dec << std::endl;
}

void InOutData128::print_hex(const char* name) const {
	std::cout << name;
	if (strlen(name) > 0) std::cout << " ";
	std::cout << "Hex (default): " << to_hex_string() << std::endl;
}

void InOutData128::print_hex(OutputFormat format, const char* name) const {
	std::cout << name;
	if (strlen(name) > 0) std::cout << " ";
	
	switch (format) {
		case OutputFormat::HEX_BIG_ENDIAN:
			std::cout << "Hex (BE): ";
			break;
		case OutputFormat::HEX_LITTLE_ENDIAN:
			std::cout << "Hex (LE): ";
			break;
		default:
			std::cout << "Hex: ";
			break;
	}
	
	std::cout << to_hex_string(format) << std::endl;
}

void InOutData128::print_binary(const char* name) const {
	std::cout << name << " Binary (BE): " << to_binary_string() << std::endl;
	std::cout << name << " Binary (LE): " << to_binary_string_le() << std::endl;
}

void InOutData128::print_detailed(const char* name) const {
	std::cout << "\n=== " << name << " ===" << std::endl;
	
	// 1. Байты в памяти
	std::cout << "Memory layout (little-endian):" << std::endl;
	std::cout << "  Bytes: ";
	for (int i = 0; i < NUM_BYTES; i++) {
		std::cout << "[" << std::setw(2) << i << "]=0x" 
				  << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(bytes[i]) << std::dec;
		if (i < NUM_BYTES - 1) std::cout << " ";
		if ((i + 1) % 4 == 0 && i < NUM_BYTES - 1) std::cout << std::endl << "         ";
	}
	std::cout << std::endl;
	
	// 2. 32-битные слова в памяти
	std::cout << "32-bit words in memory:" << std::endl;
	for (int i = 0; i < NUM_WORDS; i++) {
		std::cout << "  word[" << i << "] = 0x" << std::hex << std::setw(8) 
				  << std::setfill('0') << get_word_little_endian(i) << std::dec
				  << "  (BE: 0x" << std::hex << std::setw(8) 
				  << get_word_big_endian(i) << ")" << std::dec << std::endl;
	}
	
	// 3. Различные строковые представления
	std::cout << "\nString representations:" << std::endl;
	std::cout << "  Hex BE:      " << to_hex_string(OutputFormat::HEX_BIG_ENDIAN) << std::endl;
	std::cout << "  Hex LE:      " << to_hex_string(OutputFormat::HEX_LITTLE_ENDIAN) << std::endl;
	std::cout << "  Binary BE:   " << to_binary_string(OutputFormat::BINARY_BIG_ENDIAN) << std::endl;
	std::cout << "  Binary LE:   " << to_binary_string(OutputFormat::BINARY_LITTLE_ENDIAN) << std::endl;
}

// -------------------------------------------------
// Строковые представления
// -------------------------------------------------
std::string InOutData128::to_hex_string(OutputFormat format) const {
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
			// Little-endian: младший байт первый (как в памяти)
			ss << "0x";
			for (int i = 0; i < NUM_BYTES; i++) {
				ss << std::hex << std::setw(2) << std::setfill('0') 
				   << static_cast<int>(bytes[i]);
			}
			break;
			
		default:
			// По умолчанию big-endian
			ss << "0x";
			for (int i = NUM_BYTES - 1; i >= 0; i--) {
				ss << std::hex << std::setw(2) << std::setfill('0') 
				   << static_cast<int>(bytes[i]);
			}
			break;
	}
	
	return ss.str();
}

std::string InOutData128::to_hex_string_le() const {
	std::stringstream ss;
	ss << "0x";
	// Little-endian представление
	for (int i = 0; i < NUM_BYTES; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') 
		   << static_cast<int>(bytes[i]);
	}
	return ss.str();
}

std::string InOutData128::to_binary_string(OutputFormat format) const {
	std::stringstream ss;
	
	switch (format) {
		case OutputFormat::BINARY_BIG_ENDIAN:
			// Big-endian: от старшего байта к младшему
			for (int i = NUM_BYTES - 1; i >= 0; i--) {
				ss << std::bitset<8>(bytes[i]);
				if (i > 0) ss << " ";
			}
			break;
			
		case OutputFormat::BINARY_LITTLE_ENDIAN:
			// Little-endian: от младшего байта к старшему
			for (int i = 0; i < NUM_BYTES; i++) {
				ss << std::bitset<8>(bytes[i]);
				if (i < NUM_BYTES - 1) ss << " ";
			}
			break;
			
		default:
			// По умолчанию big-endian
			for (int i = NUM_BYTES - 1; i >= 0; i--) {
				ss << std::bitset<8>(bytes[i]);
				if (i > 0) ss << " ";
			}
			break;
	}
	
	return ss.str();
}

std::string InOutData128::to_binary_string_le() const {
	std::stringstream ss;
	// Little-endian представление
	for (int i = 0; i < NUM_BYTES; i++) {
		ss << std::bitset<8>(bytes[i]);
		if (i < NUM_BYTES - 1) ss << " ";
	}
	return ss.str();
}

// -------------------------------------------------
// Операторы доступа
// -------------------------------------------------
uint32_t InOutData128::operator[](int index) const { 
	if (index >= 0 && index < NUM_WORDS) return words[index];
	cerr << RED << CROSS_MARK << __FUNCTION__ << " index error !\t" 
		 << NORMAL << endl;
	return 0;
}

uint32_t& InOutData128::operator[](int index) { 
	static uint32_t dummy = 0;
	if (index >= 0 && index < NUM_WORDS) return words[index];
	cerr << RED << CROSS_MARK << __FUNCTION__ << " index error !\t" 
		 << NORMAL << endl;
	return dummy;
}

// -------------------------------------------------
// Методы для удобной работы
// -------------------------------------------------
void InOutData128::set_word(int index, uint32_t value) { 
	if (index >= 0 && index < NUM_WORDS) words[index] = value;
	cerr << RED << CROSS_MARK << __FUNCTION__ 
			 << " index error !\t" << NORMAL << endl;
}

uint32_t InOutData128::get_word(int index) const { 
	if (index >= 0 && index < NUM_WORDS) return words[index];
	cerr << RED << CROSS_MARK << __FUNCTION__ << " index error !\t" 
		 << NORMAL << endl;
	return 0;
}

void InOutData128::set_fields(uint32_t word0, 
							  uint32_t word1, 
							  uint32_t word2, 
							  uint32_t word3) {
	w0 = word0; 
	w1 = word1; 
	w2 = word2; 
	w3 = word3;
}

// -------------------------------------------------
// Явные методы для работы с порядком байт
// -------------------------------------------------

uint32_t InOutData128::get_word_big_endian(int index) const {
	if (index < 0 || index >= NUM_WORDS) {
		cerr << RED << CROSS_MARK << __FUNCTION__ << " index error !\t" 
			 << NORMAL << endl;
		return 0;
	}
	return swap_endian(words[index]);
}

void InOutData128::set_word_big_endian(int index, 
									   uint32_t value) {
	if (index >= 0 && index < NUM_WORDS) {
		words[index] = swap_endian(value);
	} else {
		cerr << RED << CROSS_MARK << __FUNCTION__ << " index error !\t" 
			 << NORMAL << endl;
	}
}

uint32_t InOutData128::get_word_little_endian(int index) const {
	if (index < 0 || index >= NUM_WORDS) {
		cerr << "get_word_little_endian: Index error !" << endl;
		return 0;
	}
	return words[index];
}

void InOutData128::set_word_little_endian(int index, 
										  uint32_t value) {
	if (index >= 0 && index < NUM_WORDS) {
		words[index] = value;
	} else {
		cerr << RED << CROSS_MARK << __FUNCTION__ << " index error !\t" 
			<< NORMAL << endl;
	}
}

uint32_t InOutData128::bytes_to_word_big_endian(const uint8_t* bytes) {
	return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

uint32_t InOutData128::bytes_to_word_little_endian(const uint8_t* bytes) {
	return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

// -------------------------------------------------
// Конвертация в массивы
// -------------------------------------------------
std::array<uint32_t, 4> InOutData128::to_array() const {
	std::array<uint32_t, NUM_WORDS> arr;
	for (int i = 0; i < NUM_WORDS; i++) arr[i] = words[i];
	return arr;
}

void InOutData128::from_array(const std::array<uint32_t, NUM_WORDS>& arr) {
	for (int i = 0; i < NUM_WORDS; i++) words[i] = arr[i];
}

std::array<uint32_t, 4> InOutData128::to_array_big_endian() const {
	std::array<uint32_t, NUM_WORDS> arr;
	for (int i = 0; i < NUM_WORDS; i++) arr[i] = get_word_big_endian(i);
	return arr;
}

void InOutData128::from_array_big_endian(const std::array<uint32_t, 
										 NUM_WORDS>& arr) {
	for (int i = 0; i < NUM_WORDS; i++) set_word_big_endian(i, arr[i]);
}

// -------------------------------------------------
// Бинарные операторы
// -------------------------------------------------
InOutData128 InOutData128::operator&(const InOutData128& other) const {
	InOutData128 result;
	for (int i = 0; i < NUM_WORDS; i++) {
		result.words[i] = words[i] & other.words[i];
	}
	return result;
}

InOutData128 InOutData128::operator|(const InOutData128& other) const {
	InOutData128 result;
	for (int i = 0; i < NUM_WORDS; i++) {
		result.words[i] = words[i] | other.words[i];
	}
	return result;
}

InOutData128 InOutData128::operator^(const InOutData128& other) const {
	InOutData128 result;
	for (int i = 0; i < NUM_WORDS; i++) {
		result.words[i] = words[i] ^ other.words[i];
	}
	return result;
}

InOutData128 InOutData128::operator~() const {
	InOutData128 result;
	for (int i = 0; i < NUM_WORDS; i++) {
		result.words[i] = ~words[i];
	}
	return result;
}

InOutData128& InOutData128::operator&=(const InOutData128& other) {
	for (int i = 0; i < NUM_WORDS; i++) {
		words[i] &= other.words[i];
	}
	return *this;
}

InOutData128& InOutData128::operator|=(const InOutData128& other) {
	for (int i = 0; i < NUM_WORDS; i++) {
		words[i] |= other.words[i];
	}
	return *this;
}

InOutData128& InOutData128::operator^=(const InOutData128& other) {
	for (int i = 0; i < NUM_WORDS; i++) {
		words[i] ^= other.words[i];
	}
	return *this;
}

// -------------------------------------------------
// Сравнение
// -------------------------------------------------
bool InOutData128::operator==(const InOutData128& other) const {
	return std::memcmp(bytes, other.bytes, NUM_BYTES) == 0;
}

bool InOutData128::operator!=(const InOutData128& other) const {
	return !(*this == other);
}

// -------------------------------------------------
// Статические методы создания
// -------------------------------------------------
InOutData128 InOutData128::create_from_vector(const std::vector<uint8_t>& vec, 
									   size_t start_idx,
									   InitOrder order) {
	InOutData128 result;
	
	// Проверка, достаточно ли данных в векторе
	if (vec.size() - start_idx < NUM_BYTES) {
		throw std::out_of_range("Not enough data in vector to create InOutData128");
	}
	
	if (order == InitOrder::LITTLE_ENDIAN_IN) {
		result.from_vector_little_endian(vec, start_idx);
	} else {
		result.from_vector_big_endian(vec, start_idx);
	}
	return result;
}

InOutData128 InOutData128::from_vector_le(const std::vector<uint8_t>& vec, 
										  size_t start_idx) {
	InOutData128 result;
	result.from_vector_little_endian(vec, start_idx);
	return result;
}

InOutData128 InOutData128::from_vector_be(const std::vector<uint8_t>& vec, 
														 size_t start_idx) {
	InOutData128 result;
	result.from_vector_big_endian(vec, start_idx);
	return result;
}

InOutData128 InOutData128::from_hex_string(const std::string& hex_str) {
	InOutData128 result;
	
	if (hex_str.empty()) {
		return result;
	}
	
	std::string clean_str = hex_str;
	
	// Убираем префикс
	if (hex_str.size() >= 2 && hex_str.compare(0, 2, "0x") == 0) {
		clean_str = hex_str.substr(2);
	} else if (hex_str.size() >= 2 && hex_str.compare(0, 2, "0X") == 0) {
		clean_str = hex_str.substr(2);
	}
	
	constexpr size_t NUM_BYTES = 16;
	constexpr size_t HEX_CHARS_NEEDED = NUM_BYTES * 2;
	
	// Дополняем/обрезаем строку
	if (clean_str.length() < HEX_CHARS_NEEDED) {
		clean_str = std::string(HEX_CHARS_NEEDED - clean_str.length(), '0') + clean_str;
	} else if (clean_str.length() > HEX_CHARS_NEEDED) {
		clean_str = clean_str.substr(clean_str.length() - HEX_CHARS_NEEDED);
	}
	
	// Hex строка: "00112233445566778899AABBCCDDEEFF"
	// Мы хотим: word0 = FF EE DD CC (первые 4 байта в памяти)
	// Значит: последние 8 символов hex строки -> первые 4 байта в памяти
	
	// Заполняем байты в обратном порядке
	for (size_t i = 0; i < NUM_BYTES; i++) {
		// Берем с конца hex строки для первых байтов памяти
		size_t hex_pos = (NUM_BYTES - 1 - i) * 2;
		std::string byte_str = clean_str.substr(hex_pos, 2);
		
		try {
			result.bytes[i] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
		} catch (...) {
			result.bytes[i] = 0;
		}
	}
	
	return result;
}

// И метод for_hex_string_le (если hex уже в little-endian формате)
InOutData128 InOutData128::from_hex_string_le(const std::string& hex_str) {
	InOutData128 result;
	
	if (hex_str.empty()) {
		return result;
	}
	
	std::string clean_str = hex_str;
	
	if (hex_str.size() >= 2 && hex_str.compare(0, 2, "0x") == 0) {
		clean_str = hex_str.substr(2);
	} else if (hex_str.size() >= 2 && hex_str.compare(0, 2, "0X") == 0) {
		clean_str = hex_str.substr(2);
	}
	
	constexpr size_t NUM_BYTES = 16;
	constexpr size_t HEX_CHARS_NEEDED = NUM_BYTES * 2;
	
	if (clean_str.length() < HEX_CHARS_NEEDED) {
		clean_str = std::string(HEX_CHARS_NEEDED - clean_str.length(), '0') + clean_str;
	} else if (clean_str.length() > HEX_CHARS_NEEDED) {
		clean_str = clean_str.substr(clean_str.length() - HEX_CHARS_NEEDED);
	}
	
	// Little-endian hex: первый байт в строке = byte[0] в памяти
	for (size_t i = 0; i < NUM_BYTES; i++) {
		size_t hex_pos = i * 2;
		std::string byte_str = clean_str.substr(hex_pos, 2);
		
		try {
			result.bytes[i] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
		} catch (...) {
			result.bytes[i] = 0;
		}
	}
	
	return result;
}

InOutData128 InOutData128::from_words(uint32_t w0, uint32_t w1, 
									  uint32_t w2, uint32_t w3) {
	return InOutData128(w0, w1, w2, w3);
}

// -------------------------------------------------
// Статические фабричные методы с явным порядком
// -------------------------------------------------

InOutData128 InOutData128::from_words_little_endian(uint32_t w0, uint32_t w1, 
													uint32_t w2, uint32_t w3) {
	InOutData128 result;
	result.set_word_little_endian(0, w0);
	result.set_word_little_endian(1, w1);
	result.set_word_little_endian(2, w2);
	result.set_word_little_endian(3, w3);
	return result;
}

InOutData128 InOutData128::from_words_big_endian(uint32_t w0, uint32_t w1, 
												 uint32_t w2, uint32_t w3) {
	InOutData128 result;
		result.set_word_big_endian(0, w0);
		result.set_word_big_endian(1, w1);
		result.set_word_big_endian(2, w2);
		result.set_word_big_endian(3, w3);
	return result;
}

InOutData128 InOutData128::from_bytes(const uint8_t* bytes, size_t len) {
	return InOutData128(bytes, len);
}

InOutData128 InOutData128::from_bytes_big_endian(const uint8_t* bytes, size_t len) {
	InOutData128 result;
	result.set_from_bytes_big_endian(bytes, len);
	return result;
}

// -------------------------------------------------
// Утилиты для порядка байт
// -------------------------------------------------
uint32_t InOutData128::swap_endian(uint32_t value) {
	return ((value & 0x000000FF) << 24) |
		   ((value & 0x0000FF00) << 8) |
		   ((value & 0x00FF0000) >> 8) |
		   ((value & 0xFF000000) >> 24);
}

uint64_t InOutData128::swap_endian(uint64_t value) {
	return ((value & 0x00000000000000FFULL) << 56) |
		   ((value & 0x000000000000FF00ULL) << 40) |
		   ((value & 0x0000000000FF0000ULL) << 24) |
		   ((value & 0x00000000FF000000ULL) << 8) |
		   ((value & 0x000000FF00000000ULL) >> 8) |
		   ((value & 0x0000FF0000000000ULL) >> 24) |
		   ((value & 0x00FF000000000000ULL) >> 40) |
		   ((value & 0xFF00000000000000ULL) >> 56);
}

// -------------------------------------------------
// Для Verilator
// -------------------------------------------------
#ifdef VERILATOR
InOutData128::operator VlWide<4>() const {
	VlWide<4> result;
	for (int i = 0; i < NUM_WORDS; i++) {
		result[i] = words[i];
	}
	return result;
}
#endif

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
// InOutData128.hpp
#ifndef IN_OUT_DATA_128_HPP
#define IN_OUT_DATA_128_HPP

#include <iostream>
#include <stdint.h>
#include <iomanip>
#include <array>
#include <cstring>
#include <vector>
#include <bitset>
#include <sstream>
#include <stdexcept>

#include "Theme.hpp"

using std::cout;
using std::endl;
using std::cerr;

#ifdef VERILATOR
#include "verilated.h"
#endif

struct InOutData128 {
	static constexpr int width      = 128;
	static constexpr int NUM_WORDS  = 4;
	static constexpr int NUM_BYTES  = 16;
	
	union {
		uint32_t words[NUM_WORDS];
		uint8_t bytes[NUM_BYTES];
		
		// Структурированный доступ по словам
		struct {
			uint32_t w0;  // младшие 32 бита (байты 0-3)
			uint32_t w1;  // байты 4-7
			uint32_t w2;  // байты 8-11
			uint32_t w3;  // старшие 32 бита (байты 12-15)
		};
		
		// 64-битный доступ
		struct {
			uint64_t qword0;  // байты 0-7
			uint64_t qword1;  // байты 8-15
		} qwords;
	};
	
	enum class InitOrder {
		LITTLE_ENDIAN_IN,  // данные уже в little-endian (по умолчанию)
		BIG_ENDIAN_IN      // данные в big-endian, нужно конвертировать
	};

	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	InOutData128();
	explicit InOutData128(const uint32_t data[NUM_WORDS]);
	InOutData128(uint32_t word0, uint32_t word1, 
				 uint32_t word2, uint32_t word3);
	explicit InOutData128(const uint8_t* data, size_t len = NUM_BYTES);
	explicit InOutData128(const std::vector<uint8_t>& data);
	
	// Конструктор копирования
	InOutData128(const InOutData128& other);
	
	// Move конструктор
	InOutData128(InOutData128&& other) noexcept;
	
	// -------------------------------------------------
	// Операторы присваивания
	// -------------------------------------------------
	InOutData128& operator=(const InOutData128& other);
	InOutData128& operator=(InOutData128&& other) noexcept;
	
	// -------------------------------------------------
	// Методы очистки и доступа
	// -------------------------------------------------
	void clear();
	uint8_t get_byte(int index) const;
	void set_byte(int index, 
                  uint8_t value);
	
	// Прямое копирование из массива байт (little-endian)
	void set_from_bytes(const uint8_t* data, 
						size_t len, 
						size_t offset = 0);
	
	// Для работы с разным порядком байт
	void set_from_bytes_big_endian(const uint8_t* data, 
								   size_t len, 
								   size_t offset = 0);
	void set_from_bytes_little_endian(const uint8_t* data, 
									  size_t len, 
									  size_t offset = 0);

	static uint32_t bytes_to_word_big_endian(const uint8_t* bytes);
	static uint32_t bytes_to_word_little_endian(const uint8_t* bytes);

	// -------------------------------------------------
	// Методы работы с векторами
	// -------------------------------------------------
	// Deprecated !
	InOutData128& from_vector(const std::vector<uint8_t>& vec, 
					 size_t start_idx = 0,
					 InitOrder order = InitOrder::LITTLE_ENDIAN_IN);
	std::vector<uint8_t> to_vector() const;
	
	// Конвертация в вектор с указанием порядка
	std::vector<uint8_t> to_vector_little_endian() const;
	std::vector<uint8_t> to_vector_big_endian() const;

	// Методы для работы с векторами с явным порядком
	void from_vector_little_endian(const std::vector<uint8_t>& vec, 
								   size_t start_idx = 0);
	void from_vector_big_endian(const std::vector<uint8_t>& vec, 
								size_t start_idx = 0);
	
	// -------------------------------------------------
	// Побитовые операции
	// -------------------------------------------------
	bool get_bit(int bit_index) const;
	void set_bit(int bit_index, 
				 bool value);
	
	enum class OutputFormat {
		HEX_BIG_ENDIAN,
		HEX_LITTLE_ENDIAN,
		BINARY_BIG_ENDIAN,
		BINARY_LITTLE_ENDIAN
	};
	
	// Инициализация статической переменной
	OutputFormat default_output_format = OutputFormat::HEX_BIG_ENDIAN;

	// -------------------------------------------------
	// Методы вывода
	// -------------------------------------------------
	void print(const char *name = "") const;
	void print_bytes(const char* name = "") const;
	void print_words_and_bytes(const char* name = "") const;
	void print_hex(const char* name = "") const;
	void print_hex(OutputFormat format, const char* name) const;
	void print_binary(const char* name = "") const;
	void print_detailed(const char* name) const;

	std::string to_hex_string(OutputFormat format = OutputFormat::HEX_LITTLE_ENDIAN) const;        // big-endian представление
	std::string to_hex_string_le() const;     // little-endian представление
	std::string to_binary_string(OutputFormat format = OutputFormat::HEX_LITTLE_ENDIAN) const;
	std::string to_binary_string_le() const;
	
	std::string to_raw_string(bool as_hex = true) const;
	void print_raw_memory(const char* name) const ;
	void print_test_vector_format() const ;

	// -------------------------------------------------
	// Операторы доступа
	// -------------------------------------------------
	uint32_t operator[](int index) const;
	uint32_t& operator[](int index);
	
	// -------------------------------------------------
	// Методы для удобной работы
	// -------------------------------------------------
	void set_word(int index, uint32_t value);
	uint32_t get_word(int index) const;
	void set_fields(uint32_t word0, 
					uint32_t word1, 
					uint32_t word2, 
					uint32_t word3);

	// Явные методы для little-endian порядка
	uint32_t get_word_big_endian(int index) const;
	void set_word_big_endian(int index, 
							 uint32_t value);

	// Методы для big-endian порядка (для полноты)
	uint32_t get_word_little_endian(int index) const;
	void set_word_little_endian(int index, 
								uint32_t value);
	
	// -------------------------------------------------
	// Конвертация
	// -------------------------------------------------
	std::array<uint32_t, 4> to_array() const;
	void from_array(const std::array<uint32_t, 4>& arr);
	std::array<uint32_t, 4> to_array_big_endian() const;
	void from_array_big_endian(const std::array<uint32_t, 4>& arr);
	
	// -------------------------------------------------
	// Бинарные операторы
	// -------------------------------------------------
	InOutData128 operator&(const InOutData128& other) const;
	InOutData128 operator|(const InOutData128& other) const;
	InOutData128 operator^(const InOutData128& other) const;
	InOutData128 operator~() const;
	
	InOutData128& operator&=(const InOutData128& other);
	InOutData128& operator|=(const InOutData128& other);
	InOutData128& operator^=(const InOutData128& other);
	
	// -------------------------------------------------
	// Сравнение
	// -------------------------------------------------
	bool operator==(const InOutData128& other) const;
	bool operator!=(const InOutData128& other) const;
	
	// -------------------------------------------------
	// Статические методы создания
	// -------------------------------------------------
	static InOutData128 from_hex_string(const std::string& hex_str); // hex как big-endian
	static InOutData128 from_hex_string_le(const std::string& hex_str); // hex как little-endian
	static InOutData128 from_words(uint32_t w0, 
								   uint32_t w1, 
								   uint32_t w2, 
								   uint32_t w3);

	// Статические фабричные методы с явным указанием порядка
	static InOutData128 from_words_little_endian(uint32_t w0, 
												 uint32_t w1, 
												 uint32_t w2, 
												 uint32_t w3);

	static InOutData128 from_words_big_endian(uint32_t w0, 
											  uint32_t w1, 
											  uint32_t w2, 
											  uint32_t w3);

	static InOutData128 from_bytes(const uint8_t* bytes, 
								   size_t len = 16);
	static InOutData128 from_bytes_big_endian(const uint8_t* bytes, 
											size_t len = 16);
	
	// Статические фабричные методы из векторов
	static InOutData128 create_from_vector(const std::vector<uint8_t>& vec, 
	 							    size_t start_idx = 0,
	                                InitOrder order = InitOrder::LITTLE_ENDIAN_IN );

	static InOutData128 from_vector_le(const std::vector<uint8_t>& vec, 
								   size_t start_idx = 0);

	static InOutData128 from_vector_be(const std::vector<uint8_t>& vec, 
								   size_t start_idx = 0);

	// -------------------------------------------------
	// Утилиты для порядка байт
	// -------------------------------------------------
	static uint32_t swap_endian(uint32_t value);
	static uint64_t swap_endian(uint64_t value);
	
	// -------------------------------------------------
	// Для совместимости с Verilator
	// -------------------------------------------------
#ifdef VERILATOR
	operator VlWide<4>() const;
#endif
	
private:
	// Вспомогательный метод для преобразования big-endian в little-endian
	static void convert_big_to_little_endian(uint8_t* dest, 
											 const uint8_t* src, 
											 size_t len);
	bool check_vector_bounds(const std::vector<uint8_t>& vec,
							 size_t start_idx,
							 const char* method_name) const;
};

#endif // IN_OUT_DATA_128_HPP


// Вместо статических методов можно использовать конструкторы:
//InOutData128 data1(vec_le.data(), vec_le.size());  // little-endian по умолчанию
// Или
//InOutData128 data2;
//data2.from_vector_little_endian(vec_le);
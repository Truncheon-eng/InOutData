#ifndef IN_OUT_DATA_64_HPP
#define IN_OUT_DATA_64_HPP

#include <iostream>
#include <stdint.h>
#include <iomanip>
#include <array>
#include <cstring>
#include <vector>
#include <bitset>
#include <sstream>
#include <stdexcept>

#include "Definitions.hpp"

using std::cout;
using std::endl;
using std::cerr;

#ifdef VERILATOR
#include "verilated.h"
#endif

struct InOutData64 {
	static constexpr int WIDTH      = 64;
	static constexpr int NUM_WORDS  = 2;
	static constexpr int NUM_BYTES  = 8;
	
	union {
		uint32_t words[NUM_WORDS];
		uint8_t bytes[NUM_BYTES];
		
		// Структурированный доступ по словам
		struct {
			uint32_t w0;  // младшие 32 бита (байты 0-3)
			uint32_t w1;  // старшие 32 бита (байты 4-7)
		};
		
		// 64-битный доступ
		uint64_t qword;
	};
	
	enum class InitOrder {
		LITTLE_ENDIAN_IN,  // данные уже в little-endian (по умолчанию)
		BIG_ENDIAN_IN      // данные в big-endian, нужно конвертировать
	};

	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	InOutData64();
	explicit InOutData64(const uint32_t data[NUM_WORDS]);
	InOutData64(uint32_t word0, uint32_t word1, 
                InitOrder order = InitOrder::LITTLE_ENDIAN_IN);
	explicit InOutData64(const uint8_t* data, size_t len = NUM_BYTES);
	explicit InOutData64(const std::vector<uint8_t>& data);
	explicit InOutData64(uint64_t qword_value);
	
	// Конструктор копирования
	InOutData64(const InOutData64& other);
	
	// Move конструктор
	InOutData64(InOutData64&& other) noexcept;
	
	// -------------------------------------------------
	// Операторы присваивания
	// -------------------------------------------------
	InOutData64& operator=(const InOutData64& other);
	InOutData64& operator=(InOutData64&& other) noexcept;
	InOutData64& operator=(uint64_t qword_value);
	
	// -------------------------------------------------
	// Методы очистки и доступа
	// -------------------------------------------------
	void clear();
	uint8_t get_byte(int index) const;
	void set_byte(int index, uint8_t value);
	
	// Прямое копирование из массива байт
	void set_from_bytes(const uint8_t* data, size_t len, size_t offset = 0);
	void set_from_bytes_big_endian(const uint8_t* data, size_t len, size_t offset = 0);
	void set_from_bytes_little_endian(const uint8_t* data, size_t len, size_t offset = 0);

	static uint32_t bytes_to_word_big_endian(const uint8_t* bytes);
	static uint32_t bytes_to_word_little_endian(const uint8_t* bytes);
	
	// -------------------------------------------------
	// Работа с 64-битными значениями
	// -------------------------------------------------
	uint64_t get_qword() const;
	void set_qword(uint64_t value);
	uint64_t get_qword_big_endian() const;
	void set_qword_big_endian(uint64_t value);
	
	// -------------------------------------------------
	// Методы работы с векторами
	// -------------------------------------------------
	InOutData64& from_vector(const std::vector<uint8_t>& vec, 
							 size_t start_idx = 0,
							 InitOrder order = InitOrder::LITTLE_ENDIAN_IN);
	std::vector<uint8_t> to_vector() const;
	std::vector<uint8_t> to_vector_little_endian() const;
	std::vector<uint8_t> to_vector_big_endian() const;

	void from_vector_little_endian(const std::vector<uint8_t>& vec, 
								   size_t start_idx = 0);
	void from_vector_big_endian(const std::vector<uint8_t>& vec, 
								size_t start_idx = 0);
	
	// -------------------------------------------------
	// Побитовые операции
	// -------------------------------------------------
	bool get_bit(int bit_index) const;
	void set_bit(int bit_index, bool value);
	
	enum class OutputFormat {
		HEX_BIG_ENDIAN,
		HEX_LITTLE_ENDIAN,
		BINARY_BIG_ENDIAN,
		BINARY_LITTLE_ENDIAN
	};
	
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

	std::string to_hex_string(OutputFormat format = OutputFormat::HEX_LITTLE_ENDIAN) const;
	std::string to_hex_string_le() const;
	std::string to_binary_string(OutputFormat format = OutputFormat::BINARY_LITTLE_ENDIAN) const;
	std::string to_binary_string_le() const;
	
	std::string to_raw_string(bool as_hex = true) const;
	void print_raw_memory(const char* name) const;
	void print_test_vector_format() const;
	
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
	void set_fields(uint32_t word0, uint32_t word1);
	
	// Методы для порядка байт
	uint32_t get_word_big_endian(int index) const;
	void set_word_big_endian(int index, uint32_t value);
	uint32_t get_word_little_endian(int index) const;
	void set_word_little_endian(int index, uint32_t value);
	
	// -------------------------------------------------
	// Конвертация в массивы
	// -------------------------------------------------
	std::array<uint32_t, 2> to_array() const;
	void from_array(const std::array<uint32_t, 2>& arr);
	std::array<uint32_t, 2> to_array_big_endian() const;
	void from_array_big_endian(const std::array<uint32_t, 2>& arr);
	
	// -------------------------------------------------
	// Бинарные операторы
	// -------------------------------------------------
	InOutData64 operator&(const InOutData64& other) const;
	InOutData64 operator|(const InOutData64& other) const;
	InOutData64 operator^(const InOutData64& other) const;
	InOutData64 operator~() const;
	
	InOutData64& operator&=(const InOutData64& other);
	InOutData64& operator|=(const InOutData64& other);
	InOutData64& operator^=(const InOutData64& other);
	
	// Побитовые сдвиги
	InOutData64 operator<<(int shift) const;
	InOutData64 operator>>(int shift) const;
	InOutData64& operator<<=(int shift);
	InOutData64& operator>>=(int shift);
	
	// -------------------------------------------------
	// Сравнение
	// -------------------------------------------------
	bool operator==(const InOutData64& other) const;
	bool operator!=(const InOutData64& other) const;
	bool operator<(const InOutData64& other) const;
	bool operator>(const InOutData64& other) const;
	bool operator<=(const InOutData64& other) const;
	bool operator>=(const InOutData64& other) const;
	
	// -------------------------------------------------
	// Статические методы создания
	// -------------------------------------------------
	static InOutData64 from_hex_string(const std::string& hex_str);
	static InOutData64 from_hex_string_le(const std::string& hex_str);
	static InOutData64 from_words(uint32_t w0, uint32_t w1);
	static InOutData64 from_words_little_endian(uint32_t w0, uint32_t w1);
	static InOutData64 from_words_big_endian(uint32_t w0, uint32_t w1);
	static InOutData64 from_bytes(const uint8_t* bytes, size_t len = NUM_BYTES);
	static InOutData64 from_bytes_big_endian(const uint8_t* bytes, size_t len = NUM_BYTES);
	static InOutData64 from_vector_le(const std::vector<uint8_t>& vec, size_t start_idx = 0);
	static InOutData64 from_vector_be(const std::vector<uint8_t>& vec, size_t start_idx = 0);
	static InOutData64 from_qword(uint64_t qword);
	static InOutData64 from_qword_big_endian(uint64_t qword);
	
	// -------------------------------------------------
	// Утилиты для порядка байт
	// -------------------------------------------------
	static uint32_t swap_endian(uint32_t value);
	static uint64_t swap_endian(uint64_t value);
	
	// -------------------------------------------------
	// Для совместимости с Verilator
	// -------------------------------------------------
#ifdef VERILATOR
	operator VlWide<2>() const;
#endif
	
private:
	static void convert_big_to_little_endian(uint8_t* dest, const uint8_t* src, size_t len);
	bool check_vector_bounds(const std::vector<uint8_t>& vec,
							size_t start_idx,
							const char* method_name) const;
};

#endif // IN_OUT_DATA_64_HPP
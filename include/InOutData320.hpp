// InOutData320.hpp
#ifndef IN_OUT_DATA_320_HPP
#define IN_OUT_DATA_320_HPP

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

struct InOutData320 {
	static constexpr int WIDTH      = 320;
	static constexpr int NUM_WORDS  = 10;
	static constexpr int NUM_BYTES  = 40;
	
	// Данные хранятся в little-endian формате
	union {
		uint32_t words[NUM_WORDS];
		uint8_t bytes[NUM_BYTES];
		
		struct {
			uint32_t w0;
			uint32_t w1;
			uint32_t w2;
			uint32_t w3;
			uint32_t w4;
			uint32_t w5;
			uint32_t w6;
			uint32_t w7;
			uint32_t w8;
			uint32_t w9;
		};
	};
	
	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------

	enum class InitOrder {
		LITTLE_ENDIAN_IN,  // данные уже в little-endian (по умолчанию)
		BIG_ENDIAN_IN      // данные в big-endian, нужно конвертировать
	};

	InOutData320();
	explicit InOutData320(const uint32_t data[10]);
	InOutData320(uint32_t word0, uint32_t word1, 
				 uint32_t word2, uint32_t word3,
				 uint32_t word4, uint32_t word5,
				 uint32_t word6, uint32_t word7,
				 uint32_t word8, uint32_t word9);

	//Deprecated !
	// explicit InOutData320(const uint8_t* data, size_t len = 40);
	// explicit InOutData320(const std::vector<uint8_t>& data);
	
	// Конструктор из массива байт с указанием порядка
	explicit InOutData320(const uint8_t* data, 
						  size_t len = NUM_BYTES,
						  InitOrder order = InitOrder::LITTLE_ENDIAN_IN);
	
	// Конструктор из вектора байт с указанием порядка
	explicit InOutData320(const std::vector<uint8_t>& data,
						  InitOrder order = InitOrder::LITTLE_ENDIAN_IN);

	// Конструктор копирования
	InOutData320(const InOutData320& other);
	InOutData320(InOutData320&& other) noexcept;
	
	// -------------------------------------------------
	// Операторы присваивания
	// -------------------------------------------------
	InOutData320& operator=(const InOutData320& other);
	InOutData320& operator=(InOutData320&& other) noexcept;
	
	// -------------------------------------------------
	// Базовые методы
	// -------------------------------------------------
	void clear();
	
	// Доступ к байтам
	uint8_t get_byte(int index) const;
	void set_byte(int index, 
				  uint8_t value);
	
	// Доступ к словам (little-endian семантика)
	uint32_t get_word(int index) const;
	void set_word(int index, 
				  uint32_t value);
	
	// Доступ к словам с указанием порядка
	uint32_t get_word_little_endian(int index) const;
	uint32_t get_word_big_endian(int index) const;
	void set_word_little_endian(int index, 
								uint32_t value);
	void set_word_big_endian(int index, 
							 uint32_t value);
	
	// -------------------------------------------------
	// Методы установки из байтовых массивов
	// -------------------------------------------------
	// Универсальный метод (по умолчанию little-endian)
	void set_from_bytes(const uint8_t* data, 
						size_t len, 
						size_t offset = 0);
	
	// Явные методы с указанием порядка
	void set_from_bytes_little_endian(const uint8_t* data, 
									  size_t len, 
									  size_t offset = 0);
	void set_from_bytes_big_endian(const uint8_t* data, 
								   size_t len, 
								   size_t offset = 0);
	
	// -------------------------------------------------
	// Методы работы с векторами
	// -------------------------------------------------
	// Универсальные (по умолчанию little-endian)
	void from_vector(const std::vector<uint8_t>& vec, 
					 size_t start_idx = 0);
	std::vector<uint8_t> to_vector() const;
	
	// Явные с указанием порядка
	void from_vector_little_endian(const std::vector<uint8_t>& vec, 
								   size_t start_idx = 0);
	void from_vector_big_endian(const std::vector<uint8_t>& vec, 
								size_t start_idx = 0);
	std::vector<uint8_t> to_vector_little_endian() const;
	std::vector<uint8_t> to_vector_big_endian() const;
	
	// -------------------------------------------------
	// Побитовые операции
	// -------------------------------------------------
	bool get_bit(int bit_index) const;
	void set_bit(int bit_index, 
				 bool value);
	
	// -------------------------------------------------
	// Методы вывода
	// -------------------------------------------------
	enum class OutputFormat {
		HEX_BIG_ENDIAN,
		HEX_LITTLE_ENDIAN,
		BINARY_BIG_ENDIAN,
		BINARY_LITTLE_ENDIAN
	};
	
	static OutputFormat default_output_format;
	static void set_default_output_format(OutputFormat format);
	
	void print(const char* name = "") const;
	void print_bytes(const char* name = "") const;
	void print_words_and_bytes(const char* name = "") const;
	void print_detailed(const char* name = "") const;
	void print_raw_memory(const char* name = "") const;
	
	std::string to_hex_string() const;
	std::string to_hex_string(OutputFormat format) const;
	std::string to_binary_string() const;
	std::string to_binary_string(OutputFormat format) const;
	std::string to_raw_string(bool as_hex = true) const;
	
	void print_hex(const char* name = "") const;
	void print_hex(OutputFormat format, const char* name) const;
	void print_binary(const char* name = "") const;
	void print_binary(OutputFormat format, const char* name) const;
	
	// -------------------------------------------------
	// Конвертация в массивы
	// -------------------------------------------------
	std::array<uint32_t, 10> to_array() const;
	void from_array(const std::array<uint32_t, 10>& arr);
	
	// -------------------------------------------------
	// Операторы
	// -------------------------------------------------
	uint32_t operator[](int index) const;
	uint32_t& operator[](int index);
	
	InOutData320 operator&(const InOutData320& other) const;
	InOutData320 operator|(const InOutData320& other) const;
	InOutData320 operator^(const InOutData320& other) const;
	InOutData320 operator~() const;
	
	InOutData320& operator&=(const InOutData320& other);
	InOutData320& operator|=(const InOutData320& other);
	InOutData320& operator^=(const InOutData320& other);
	
	bool operator==(const InOutData320& other) const;
	bool operator!=(const InOutData320& other) const;

	// Сортировочные операторы

	bool operator<(const InOutData320& other) const ;

	bool operator>(const InOutData320& other) const ;

	bool operator<=(const InOutData320& other) const ;

	bool operator>=(const InOutData320& other) const;
	
	// -------------------------------------------------
	// Статические фабричные методы
	// -------------------------------------------------
	static InOutData320 from_hex_string(const std::string& hex_str);
	//Deprecated
	static InOutData320 from_hex_string_le(const std::string& hex_str);
	
	static InOutData320 from_words(uint32_t w0, uint32_t w1, 
								   uint32_t w2, uint32_t w3,
								   uint32_t w4, uint32_t w5,
								   uint32_t w6, uint32_t w7,
								   uint32_t w8, uint32_t w9);
	
	static InOutData320 from_words_little_endian(uint32_t w0, uint32_t w1, 
												 uint32_t w2, uint32_t w3,
												 uint32_t w4, uint32_t w5,
												 uint32_t w6, uint32_t w7,
												 uint32_t w8, uint32_t w9);
	
	static InOutData320 from_words_big_endian(uint32_t w0, uint32_t w1, 
											  uint32_t w2, uint32_t w3,
											  uint32_t w4, uint32_t w5,
											  uint32_t w6, uint32_t w7,
											  uint32_t w8, uint32_t w9);
	
	static InOutData320 from_bytes(const uint8_t* bytes, 
								   size_t len = NUM_BYTES,
								   InitOrder order = InitOrder::LITTLE_ENDIAN_IN);
	static InOutData320 from_bytes_little_endian(const uint8_t* bytes, 
												 size_t len = NUM_BYTES);
	static InOutData320 from_bytes_big_endian(const uint8_t* bytes, 
											  size_t len = NUM_BYTES);


	
	// -------------------------------------------------
	// Утилиты для порядка байт
	// -------------------------------------------------
	static uint32_t swap_endian(uint32_t value);
	static uint64_t swap_endian(uint64_t value);
	static uint32_t bytes_to_word_big_endian(const uint8_t* bytes);
	static uint32_t bytes_to_word_little_endian(const uint8_t* bytes);
	
	// -------------------------------------------------
	// Для совместимости с Verilator
	// -------------------------------------------------
#ifdef VERILATOR
	operator VlWide<10>() const;
#endif
	
private:
	// Вспомогательные методы
	// FIXME:Deprecated
	static void convert_big_to_little_endian(uint8_t* dest, 
											 const uint8_t* src, 
											 size_t len);
};

#endif // IN_OUT_DATA_320_HPP
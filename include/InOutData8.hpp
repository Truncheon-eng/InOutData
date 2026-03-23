// InOutData8.hpp
#ifndef IN_OUT_DATA_8_HPP
#define IN_OUT_DATA_8_HPP

#include <iostream>
#include <stdint.h>
#include <iomanip>
#include <array>
#include <cstring>
#include <vector>
#include <bitset>
#include <sstream>
#include <stdexcept>

#ifdef VERILATOR
#include "verilated.h"
#endif

#include "Theme.hpp"

using std::cerr;
using std::cout;
using std::endl;

struct InOutData8 {
	static constexpr int WIDTH      = 8;
	static constexpr int NUM_BYTES  = 1;
	static constexpr int NUM_WORDS  = 1;
	
	// Данные хранятся в little-endian формате
	union {
		uint32_t words[1];
		uint8_t bytes[1];
		
		struct {
			uint8_t b0;
		};
		
		uint8_t packed;  // Единое 8-битное значение
	};
	
	enum class InitOrder {
		LITTLE_ENDIAN_IN,  // данные уже в little-endian (по умолчанию)
		BIG_ENDIAN_IN      // данные в big-endian, нужно конвертировать
	};

	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	InOutData8();
	explicit InOutData8(const uint8_t data[1]);
	explicit InOutData8(uint8_t value);
	explicit InOutData8(const std::vector<uint8_t>& data, 
						size_t start_idx = 0);
	
	// Конструктор копирования и move
	InOutData8(const InOutData8& other);
	InOutData8(InOutData8&& other) noexcept;
	
	// -------------------------------------------------
	// Операторы присваивания
	// -------------------------------------------------
	InOutData8& operator=(const InOutData8& other);
	InOutData8& operator=(InOutData8&& other) noexcept;
	
	// -------------------------------------------------
	// Базовые методы
	// -------------------------------------------------
	void clear();
	
	// Доступ к данным
	uint8_t get_byte() const;
	void set_byte(uint8_t value);
	
	uint8_t get_packed() const;
	void set_packed(uint8_t value);
	
	uint32_t get_word(int index = 0) const;
	void set_word(int index, uint32_t value);
	
	void set_field(uint8_t byte0);

	// -------------------------------------------------
	// Методы установки из байтовых массивов
	// -------------------------------------------------
	void set_from_bytes(const uint8_t* data, 
						size_t len, 
						size_t offset = 0);
	//void set_from_bytes_little_endian(const uint8_t* data, 
	//								  size_t len, 
	//								  size_t offset = 0);
	//void set_from_bytes_big_endian(const uint8_t* data, 
	//							   size_t len, 
	//							   size_t offset = 0);
	
	// -------------------------------------------------
	// Методы работы с векторами
	// -------------------------------------------------
	void from_vector(const std::vector<uint8_t>& vec, 
	 						size_t start_idx = 0 );
	//void from_vector_little_endian(const std::vector<uint8_t>& vec, 
	//							   size_t start_idx = 0);
	//void from_vector_big_endian(const std::vector<uint8_t>& vec, 
	//							size_t start_idx = 0);
	
	std::vector<uint8_t> to_vector() const;
	//std::vector<uint8_t> to_vector_little_endian() const;
	//std::vector<uint8_t> to_vector_big_endian() const;
	
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
	void print_detailed(const char* name = "") const;
	void print_raw_memory(const char* name = "") const;
	
	std::string to_hex_string() const;
	std::string to_hex_string(OutputFormat format) const;
	std::string to_binary_string() const;
	std::string to_binary_string(OutputFormat format) const;
	std::string to_raw_string(bool as_hex = true) const;
	std::string to_bin_string() const;  // для обратной совместимости
	
	void print_hex(const char* name = "") const;
	void print_hex(OutputFormat format, const char* name) const;
	void print_binary(const char* name = "") const;
	void print_binary(OutputFormat format, const char* name) const;
	void print_bin(const char* name = "") const;  // для обратной совместимости
	
	// -------------------------------------------------
	// Конвертация в массивы
	// -------------------------------------------------
	std::array<uint8_t, 1> to_array() const;
	void from_array(const std::array<uint8_t, 1>& arr);
	
	// -------------------------------------------------
	// Операторы
	// -------------------------------------------------
	uint8_t operator[](int index) const;
	uint8_t& operator[](int index);
	
	InOutData8 operator&(const InOutData8& other) const;
	InOutData8 operator|(const InOutData8& other) const;
	InOutData8 operator^(const InOutData8& other) const;
	InOutData8 operator~() const;
	
	InOutData8& operator&=(const InOutData8& other);
	InOutData8& operator|=(const InOutData8& other);
	InOutData8& operator^=(const InOutData8& other);
	
	bool operator==(const InOutData8& other) const;
	bool operator!=(const InOutData8& other) const;
	
	// Операторы приведения и присваивания
	operator uint8_t() const;
	InOutData8& operator=(uint8_t value);
	
	// -------------------------------------------------
	// Статические фабричные методы
	// -------------------------------------------------
	static InOutData8 from_hex_string(const std::string& hex_str);
	//static InOutData8 from_hex_string_le(const std::string& hex_str);
	
	static InOutData8 from_byte(uint8_t value);
	static InOutData8 from_bytes(const uint8_t* bytes, 
								 size_t len = 1);
	static InOutData8 from_bytes_little_endian(const uint8_t* bytes, 
											   size_t len = 1);
	static InOutData8 from_bytes_big_endian(const uint8_t* bytes, 
											size_t len = 1);
	static InOutData8 create_from_vector(const std::vector<uint8_t>& vec,
                                     size_t start_idx = 0);
	//static InOutData8 from_vector(const std::vector<uint8_t>& vec, 
	//							  size_t start_idx = 0);
	//static InOutData8 from_vector_little_endian(const std::vector<uint8_t>& vec, 
	//											size_t start_idx = 0);
	//static InOutData8 from_vector_big_endian(const std::vector<uint8_t>& vec, 
	//										 size_t start_idx = 0);
};

#endif // IN_OUT_DATA_8_HPP
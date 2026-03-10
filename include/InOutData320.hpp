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

	
	// Явные методы с указанием порядка
	void set_from_bytes_little_endian(const uint8_t* data, 
									  size_t len, 
									  size_t offset = 0);
	
	void set_from_bytes_big_endian(const uint8_t* data, 
								   size_t len, 
								   size_t offset = 0);

	void from_vector_little_endian(const std::vector<uint8_t>& vec, 
												size_t start_idx = 0);
	
	void from_vector_big_endian(const std::vector<uint8_t>& vec, 
											size_t start_idx = 0);

	void convert_big_to_little_endian(uint8_t* dest, 
										const uint8_t* src, 
										size_t len);
	
	// -------------------------------------------------
	// Утилиты для порядка байт
	// -------------------------------------------------
	static uint32_t swap_endian(uint32_t value);
	
	// Доступ к байтам
	uint8_t get_byte(int index) const; // --------------
	void set_byte(int index, 
				  uint8_t value); // --------------
	
	// Доступ к словам (little-endian семантика)
	uint32_t get_word(int index) const; // --------------
	void set_word(int index, 
				  uint32_t value); // --------------
	
	// Доступ к словам с указанием порядка
	uint32_t get_word_little_endian(int index) const; // --------------
	uint32_t get_word_big_endian(int index) const; // --------------
	void set_word_little_endian(int index, 
								uint32_t value); // --------------
	void set_word_big_endian(int index, 
							 uint32_t value);
};

#endif // IN_OUT_DATA_320_HPP
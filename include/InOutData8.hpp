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
};

#endif // IN_OUT_DATA_8_HPP
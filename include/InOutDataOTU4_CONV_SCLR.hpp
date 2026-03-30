#pragma once
#include <iostream>
#include <stdint.h>
#include <iomanip>
#include <array>
#include <cstring>
#include <vector>
#include <bitset>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "Definitions.hpp"
#include "InOutFramerTXDataStrucHeader.hpp"

// ------------------------------------------------------------------
//  Составные ВХОДНЫЕ данные модуля OTU4_CONVERTER подсистемы FRAMER_PCS_TX
// ------------------------------------------------------------------

struct InSignalOtu4Converter {
	static constexpr int WIDTH = 2;
	static constexpr int NUM_WORDS = 1;
	static constexpr int TOTAL_BITS = 2;
	static constexpr int TOTAL_BYTES = 1;
	static constexpr uint8_t BIT_MASK = 0x3;  // Маска для 2 битов
	
	static_assert(WIDTH <= 8, "2-bit structure must fit into a single byte");
	
	// Внутреннее представление
    // FIXME: Using named union and functions !
	union {
		struct {
			uint8_t DATA_VALID_IN : 1;
			uint8_t nHARD_RESET   : 1;
			uint8_t RESERVED      : 6;   // Выравнивание
		};
		uint8_t packed;
		uint8_t bytes[TOTAL_BYTES];
		// Deprecated
		// uint32_t words[NUM_WORDS];
	};
	
	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	constexpr InSignalOtu4Converter() : packed(0) {}
	
	constexpr InSignalOtu4Converter(bool data_valid_in, 
									bool n_hard_reset) 
		: packed(0) {
		set_all(data_valid_in, n_hard_reset);
	}
	
	explicit constexpr InSignalOtu4Converter(uint8_t value) 
		: packed{value & BIT_MASK} {}  // Маска на 2 бита
	
	// Конструктор из вектора байт
	explicit InSignalOtu4Converter(const std::vector<uint8_t>& byte_vector, 
								   size_t start_index = 0) 
		: packed(0) {
		from_bytes(byte_vector, start_index);
	}
	
	// -------------------------------------------------
	// Инициализация из std::vector<uint8_t>
	// -------------------------------------------------
	
	// Основной метод загрузки из вектора байт
	void from_bytes(const std::vector<uint8_t>& byte_vector, 
					size_t start_index = 0) {
		if (start_index < byte_vector.size()) {
			packed = byte_vector[start_index] & BIT_MASK;
		} else {
			packed = 0;  // Если индекс выходит за границы, обнуляем
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": start_index " << start_index 
					  << " out of range (vector size: " << byte_vector.size() << ")"
					  << NORMAL << std::endl;
		}
	}
};

// ------------------------------------------------------------------
//  Составные ВЫХОДНЫЕ данные модуля OTU4_CONVERTER подсистемы FRAMER_PCS_TX
// ------------------------------------------------------------------
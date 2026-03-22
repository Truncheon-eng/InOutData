#pragma once

#ifndef IN_OUT_DATA_112_HPP
#define IN_OUT_DATA_112_HPP

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

struct InOutData112 {
	// Константы размеров
	static constexpr int TOTAL_BITS      = 112;
	static constexpr int NUM_BYTES       = 14;           // 112 / 8
	static constexpr int NUM_WORDS       = 4;            // 4 x 32 бита = 128 бит (с padding)
	static constexpr int FULL_WORDS      = 3;            // 112 / 32 = 3.5
	static constexpr int REMAINING_BITS  = 16;           // 112 % 32 = 16
	static constexpr uint32_t MASK_LAST_WORD = 0x0000FFFF;  // Маска для 16 бит
	
	union {
		// Доступ как массив 32-битных слов
		uint32_t words[NUM_WORDS];
		
		// Доступ как массив байт
		uint8_t bytes[NUM_BYTES];
		
		// Структурный доступ к словам
		struct {
			uint32_t w0;
			uint32_t w1;
			uint32_t w2;
			uint32_t w3;  // Только младшие 16 бит используются
		};
	};
	
	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------

	enum class InitOrder {
		LITTLE_ENDIAN_IN,  // данные уже в little-endian (по умолчанию)
		BIG_ENDIAN_IN      // данные в big-endian, нужно конвертировать
	};

	InOutData112();
	
	// Конструктор из массива 32-битных слов
	explicit InOutData112(const uint32_t data[4]);
	
	// Конструктор из четырех 32-битных слов
	InOutData112(uint32_t word0, uint32_t word1, 
				 uint32_t word2, uint32_t word3);
	
	// Конструктор из массива байт с указанием порядка
	explicit InOutData112(const uint8_t* data, 
						  size_t len = NUM_BYTES,
						  InitOrder order = InitOrder::LITTLE_ENDIAN_IN);
	
	// Конструктор из вектора байт с указанием порядка
	explicit InOutData112(const std::vector<uint8_t>& data,
						  InitOrder order = InitOrder::LITTLE_ENDIAN_IN);

	// Конструктор копирования
	InOutData112(const InOutData112& other);
	InOutData112(InOutData112&& other) noexcept;
	
	// -------------------------------------------------
	// Операторы присваивания
	// -------------------------------------------------
	InOutData112& operator=(const InOutData112& other);
	InOutData112& operator=(InOutData112&& other) noexcept;

	// -------------------------------------------------
	// Базовые методы
	// -------------------------------------------------
	void clear();

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
	// Утилиты для усечения размерной сетки
	// -------------------------------------------------
	void apply_mask();
	bool is_masked_correctly() const;

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
	// Методы вывода
	// -------------------------------------------------
	enum class OutputFormat {
		HEX_BIG_ENDIAN,
		HEX_LITTLE_ENDIAN,
		BINARY_BIG_ENDIAN,
		BINARY_LITTLE_ENDIAN
	};

    static OutputFormat default_output_format;

	// -------------------------------------------------
	// Для совместимости с Verilator
	// -------------------------------------------------
#ifdef VERILATOR
	operator VlWide<4>() const;
#endif

private:
	// Вспомогательные методы
	static void convert_big_to_little_endian(uint8_t* dest, 
											 const uint8_t* src, 
											 size_t len);
	
	// Вспомогательный метод для применения маски
	void apply_mask_internal();
};
#endif // IN_OUT_DATA_112_HPP
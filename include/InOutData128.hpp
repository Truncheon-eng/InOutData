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

    // -------------------------------------------------
    // методы установки байт/бит
    // -------------------------------------------------
    void set_from_bytes(const uint8_t* data, 
                                    size_t len, 
                                    size_t offset = 0);

	// -------------------------------------------------
	// Методы работы с векторами
	// -------------------------------------------------
	InOutData128& from_vector(const std::vector<uint8_t>& vec, 
					 size_t start_idx = 0,
					 InitOrder order = InitOrder::LITTLE_ENDIAN_IN);

	// Методы для работы с векторами с явным порядком
	void from_vector_little_endian(const std::vector<uint8_t>& vec, 
								   size_t start_idx = 0);
	void from_vector_big_endian(const std::vector<uint8_t>& vec, 
								size_t start_idx = 0);
	

    // проверка границ передаваемых параметров
    bool check_vector_bounds(const std::vector<uint8_t>& vec,
							 size_t start_idx,
							 const char* method_name) const;
};

#endif // IN_OUT_DATA_128_HPP
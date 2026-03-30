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

#include "Theme.hpp"
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
	
	// -------------------------------------------------
	// Экспорт в std::vector<uint8_t>
	// -------------------------------------------------
	
	// Преобразование в вектор байт
	std::vector<uint8_t> to_bytes() const {
		return {packed};
	}
	
	// Добавление байта в существующий вектор
	void append_to_bytes(std::vector<uint8_t>& byte_vector) const {
		byte_vector.push_back(packed);
	}
	
	// Запись в существующий вектор по индексу
	void write_to_bytes(std::vector<uint8_t>& byte_vector, 
						size_t index) const {
		if (index < byte_vector.size()) {
			byte_vector[index] = packed;
		} else {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": index " << index 
					  << " out of range (vector size: " << byte_vector.size() << ")"
					  << NORMAL << std::endl;
		}
	}
	
	// -------------------------------------------------
	// Батч-обработка нескольких структур
	// -------------------------------------------------
	
	// Создание вектора структур из вектора байт
	static std::vector<InSignalOtu4Converter> batch_from_bytes(
		const std::vector<uint8_t>& byte_vector) {
		
		std::vector<InSignalOtu4Converter> result;
		result.reserve(byte_vector.size());
		
		for (uint8_t byte : byte_vector) {
			result.emplace_back(static_cast<uint32_t>(byte & BIT_MASK));
		}
		
		return result;
	}
	
	// Создание вектора структур из вектора байт с шагом
	static std::vector<InSignalOtu4Converter> batch_from_bytes_stride(
		const std::vector<uint8_t>& byte_vector,
		size_t stride = 1) {
		
		if (stride == 0) {
			std::cerr << YELLOW << WARNING_MARK << " " << __FUNCTION__ 
					  << ": stride cannot be 0, using 1"
					  << NORMAL << std::endl;
			stride = 1;
		}
		
		std::vector<InSignalOtu4Converter> result;
		result.reserve(byte_vector.size() / stride);
		
		for (size_t i = 0; i < byte_vector.size(); i += stride) {
			result.emplace_back(static_cast<uint32_t>(byte_vector[i] & BIT_MASK));
		}
		
		return result;
	}
	
	// Преобразование вектора структур в вектор байт
	static std::vector<uint8_t> batch_to_bytes(
		const std::vector<InSignalOtu4Converter>& signals) {
		
		std::vector<uint8_t> result;
		result.reserve(signals.size());
		
		for (const auto& signal : signals) {
			result.push_back(signal.packed);
		}
		
		return result;
	}
	
	// -------------------------------------------------
	// Геттеры
	// -------------------------------------------------
	bool get_DATA_VALID_IN() const { 
		return static_cast<bool>(DATA_VALID_IN); 
	}
	
	bool get_nHARD_RESET() const { 
		return static_cast<bool>(nHARD_RESET); 
	}
	
	constexpr uint8_t get_packed() const { 
		return packed; 
	}
	
	// Получение в виде байта с маской
	constexpr uint8_t get_masked_byte() const {
		return packed & BIT_MASK;
	}
	
	// -------------------------------------------------
	// Сеттеры
	// -------------------------------------------------
	void set_DATA_VALID_IN(bool v) { 
		DATA_VALID_IN = v ? 1 : 0; 
	}
	
	void set_nHARD_RESET(bool v) { 
		nHARD_RESET = v ? 1 : 0; 
	}
	
	void set_packed(uint8_t value) { 
		packed = value & BIT_MASK;
	}
	
	// Установка из байта с явной маской
	void set_from_masked_byte(uint8_t masked_byte) {
		packed = masked_byte & BIT_MASK;
	}
	
};
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
	
	// -------------------------------------------------
	// Установка сразу всех битов
	// -------------------------------------------------
	void set_all(bool data_valid_in, 
                 bool n_hard_reset) {
		DATA_VALID_IN = data_valid_in ? 1 : 0;
		nHARD_RESET = n_hard_reset ? 1 : 0;
	}
	
    void clear() {
		packed = 0x00;
	}

	// -------------------------------------------------
	// Операторы доступа (унификация с InOutData)
	// -------------------------------------------------
	// Константный оператор доступа к байту
	constexpr uint8_t operator[](size_t index) const {
		if (index < TOTAL_BYTES) {
			return bytes[index] & BIT_MASK;  // Маска гарантирует корректность
		}
		throw std::out_of_range("Index out of range in InSignalOtu4Converter");
	}
	
	// НЕконстантный оператор доступа
	uint8_t& operator[](size_t index) {
		if (index < TOTAL_BYTES) {
			return bytes[index];  // Прямой доступ к байту в union
		}
		// Безопасная реализация без статической переменной
		throw std::out_of_range("Index out of range in InSignalOtu4Converter");
	}
	
	// -------------------------------------------------
	// Операторы сравнения
	// -------------------------------------------------
	constexpr bool operator==(const InSignalOtu4Converter& other) const {
		return packed == other.packed;
	}
	
	constexpr bool operator!=(const InSignalOtu4Converter& other) const {
		return packed != other.packed;
	}
	
	// -------------------------------------------------
	// Преобразование
	// -------------------------------------------------
	constexpr operator uint32_t() const { 
		return static_cast<uint32_t>(packed); 
	}
	
	// Явное преобразование в uint8_t
	constexpr explicit operator uint8_t() const {
		return packed;
	}
	
	// -------------------------------------------------
	// Получение битовой маски
	// -------------------------------------------------
	std::bitset<TOTAL_BITS> to_bitset() const {
		return std::bitset<TOTAL_BITS>(packed);
	}
	
	// -------------------------------------------------
	// Строковое представление
	// -------------------------------------------------
	std::string to_string() const {
		std::stringstream ss;
		ss << "DV_IN=" << get_DATA_VALID_IN()
		   << ", nRST=" << get_nHARD_RESET()
		   << " [0x" << std::hex << std::setw(2) << std::setfill('0')
		   << static_cast<int>(packed) << std::dec << "]";
		return ss.str();
	}
	
	std::string get_status() const {
		std::stringstream ss;
		ss << (get_DATA_VALID_IN() ? "DATA_VALID " : "NO_DATA ")
		   << (get_nHARD_RESET() ? "| RESET_ACTIVE" : "| RESET_INACTIVE");
		return ss.str();
	}
	
	std::string to_binary_string() const {
		return std::bitset<2>(packed).to_string();
	}
	
	// Форматированное строковое представление с указанием битов
	std::string to_detailed_string() const {
		std::stringstream ss;
		ss << "Bits: [DV_IN=" << (get_DATA_VALID_IN() ? "1" : "0")
		   << ", nRST=" << (get_nHARD_RESET() ? "1" : "0") << "]"
		   << " Hex: 0x" << std::hex << std::setw(2) << std::setfill('0')
		   << static_cast<int>(packed) << std::dec
		   << " Dec: " << static_cast<int>(packed);
		return ss.str();
	}
	
	// -------------------------------------------------
	// Вывод для отладки (унифицированный формат)
	// -------------------------------------------------
	void print(const char* prefix = "") const {
		std::ios_base::fmtflags old_flags = std::cout.flags();
		
		std::cout << prefix << "InSignalOtu4Converter [" << to_binary_string() << "]:\n";
		std::cout << "  DATA_VALID_IN: " << (get_DATA_VALID_IN() ? "1 (true)" : "0 (false)") << "\n";
		std::cout << "  nHARD_RESET:   " << (get_nHARD_RESET() ? "1 (active low)" : "0 (inactive)") << "\n";
		std::cout << "  Packed: 0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(packed) << std::dec << "\n";
		std::cout << "  Status: " << get_status() << std::endl;
		
		std::cout.flags(old_flags);
	}
	
	// Краткий вывод
	void print_short(const char* prefix = "") const {
		std::cout << prefix << to_string() << std::endl;
	}
};

// ------------------------------------------------------------------
//  Составные ВЫХОДНЫЕ данные модуля OTU4_CONVERTER подсистемы FRAMER_PCS_TX
// ------------------------------------------------------------------

struct OutSignalOtu4Converter {
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
			uint8_t DATA_VALID_OUT : 1;
			uint8_t FRAME_BEGIN    : 1;
			uint8_t RESERVED       : 6;   // Выравнивание
		};
		uint8_t packed;
		uint8_t bytes[TOTAL_BYTES];
	};
	
	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	constexpr OutSignalOtu4Converter() : packed(0) {}
	
	constexpr OutSignalOtu4Converter(bool data_valid_out, 
									 bool frame_begin) 
		: packed(0) {
		set_all(data_valid_out, frame_begin);
	}
	
	explicit constexpr OutSignalOtu4Converter(uint8_t value) 
		: packed{value & BIT_MASK} {}  // Маска на 2 бита
	
	// Конструктор из вектора байт
	explicit OutSignalOtu4Converter(const std::vector<uint8_t>& byte_vector, 
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
			packed = 0;
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": start_index " << start_index 
					  << " out of range (vector size: " << byte_vector.size() << ")"
					  << NORMAL << std::endl;
		}
	}
	
	// Метод для загрузки из поддиапазона вектора
	void from_bytes_range(const std::vector<uint8_t>& byte_vector,
						  size_t start_index, 
						  size_t count) {
		if (start_index < byte_vector.size() && count > 0) {
			packed = byte_vector[start_index] & BIT_MASK;
		} else {
			packed = 0;
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": invalid range start_index=" << start_index 
					  << " count=" << count 
					  << " (vector size: " << byte_vector.size() << ")"
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
	
	// Исходные данные: вектор из 5 байтов
	//std::vector<uint8_t> bytes = {0x01, 0x02, 0x03, 0x00, 0xFF};

	// Преобразуем в 5 структур InSignalOtu4Converter
	//auto signals = InSignalOtu4Converter::batch_from_bytes(bytes);

	// Создание вектора структур из вектора байт
	static std::vector<OutSignalOtu4Converter> batch_from_bytes(
		const std::vector<uint8_t>& byte_vector) {
		
		std::vector<OutSignalOtu4Converter> result;
		result.reserve(byte_vector.size());
		
		for (uint8_t byte : byte_vector) {
			result.emplace_back(static_cast<uint32_t>(byte & BIT_MASK));
		}
		
		return result;
	}
	
	// Создание вектора структур из вектора байт с шагом
	static std::vector<OutSignalOtu4Converter> batch_from_bytes_stride(
		const std::vector<uint8_t>& byte_vector,
		size_t stride = 1) {
		
		if (stride == 0) {
			std::cerr << YELLOW << WARNING_MARK << " " << __FUNCTION__ 
					  << ": stride cannot be 0, using 1"
					  << NORMAL << std::endl;
			stride = 1;
		}
		
		std::vector<OutSignalOtu4Converter> result;
		result.reserve(byte_vector.size() / stride);
		
		for (size_t i = 0; i < byte_vector.size(); i += stride) {
			result.emplace_back(static_cast<uint32_t>(byte_vector[i] & BIT_MASK));
		}
		
		return result;
	}
	
	// Преобразование вектора структур в вектор байт
	static std::vector<uint8_t> batch_to_bytes(
		const std::vector<OutSignalOtu4Converter>& signals) {
		
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
	bool get_DATA_VALID_OUT() const { 
		return static_cast<bool>(DATA_VALID_OUT); 
	}
	
	bool get_FRAME_BEGIN() const { 
		return static_cast<bool>(FRAME_BEGIN); 
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
	void set_DATA_VALID_OUT(bool v) { 
		DATA_VALID_OUT = v ? 1 : 0; 
	}
	
	void set_FRAME_BEGIN(bool v) { 
		FRAME_BEGIN = v ? 1 : 0; 
	}
	
	void set_packed(uint8_t value) { 
		packed = value & BIT_MASK;
	}
	
	// Установка из байта с явной маской
	void set_from_masked_byte(uint8_t masked_byte) {
		packed = masked_byte & BIT_MASK;
	}
	
	// -------------------------------------------------
	// Установка сразу всех битов
	// -------------------------------------------------
	void set_all(bool data_valid_out, bool frame_begin) {
		DATA_VALID_OUT = data_valid_out ? 1 : 0;
		FRAME_BEGIN = frame_begin ? 1 : 0;
	}

    void clear() {
		packed = 0x00;
	}
	
	// -------------------------------------------------
	// Операторы доступа (унификация с InOutData)
	// -------------------------------------------------
	constexpr uint8_t operator[](size_t index) const {
		if (index < TOTAL_BYTES) {
			return bytes[index] & BIT_MASK;
		}
		throw std::out_of_range("Index out of range in OutSignalOtu4Converter");
	}
	
	uint8_t& operator[](size_t index) {
		if (index < TOTAL_BYTES) {
			return bytes[index];  // Прямой доступ к байту в union
		}
		throw std::out_of_range("Index out of range in InSignalOtu4Converter");
	}
	
	// -------------------------------------------------
	// Операторы сравнения
	// -------------------------------------------------
	constexpr bool operator==(const OutSignalOtu4Converter& other) const {
		return packed == other.packed;
	}
	
	constexpr bool operator!=(const OutSignalOtu4Converter& other) const {
		return packed != other.packed;
	}
	
	// -------------------------------------------------
	// Преобразование
	// -------------------------------------------------
	constexpr operator uint32_t() const { 
		return static_cast<uint32_t>(packed); 
	}
	
	// Явное преобразование в uint8_t
	constexpr explicit operator uint8_t() const {
		return packed;
	}
	
	// -------------------------------------------------
	// Получение битовой маски
	// -------------------------------------------------
	std::bitset<TOTAL_BITS> to_bitset() const {
		return std::bitset<TOTAL_BITS>(packed);
	}
	
	// -------------------------------------------------
	// Строковое представление
	// -------------------------------------------------
	std::string to_string() const {
		std::stringstream ss;
		ss << "DV_OUT=" << get_DATA_VALID_OUT()
		   << ", FRM_BEG=" << get_FRAME_BEGIN()
		   << " [0x" << std::hex << std::setw(2) << std::setfill('0')
		   << static_cast<int>(packed) << std::dec << "]";
		return ss.str();
	}
	
	std::string get_status() const {
		std::stringstream ss;
		ss << (get_DATA_VALID_OUT() ? "DATA_VALID " : "NO_DATA ")
		   << (get_FRAME_BEGIN() ? "| FRAME_START" : "| FRAME_CONT");
		return ss.str();
	}
	
	std::string to_binary_string() const {
		return std::bitset<2>(packed).to_string();
	}
	
	// Форматированное строковое представление с указанием битов
	std::string to_detailed_string() const {
		std::stringstream ss;
		ss << "Bits: [DV_OUT=" << (get_DATA_VALID_OUT() ? "1" : "0")
		   << ", FRM_BEG=" << (get_FRAME_BEGIN() ? "1" : "0") << "]"
		   << " Hex: 0x" << std::hex << std::setw(2) << std::setfill('0')
		   << static_cast<int>(packed) << std::dec
		   << " Dec: " << static_cast<int>(packed);
		return ss.str();
	}
	
	// -------------------------------------------------
	// Вывод для отладки (унифицированный формат)
	// -------------------------------------------------
	void print(const char* prefix = "") const {
		std::ios_base::fmtflags old_flags = std::cout.flags();
		
		std::cout << prefix << "OutSignalOtu4Converter [" << to_binary_string() << "]:\n";
		std::cout << "  DATA_VALID_OUT: " << (get_DATA_VALID_OUT() ? "1 (true)" : "0 (false)") << "\n";
		std::cout << "  FRAME_BEGIN:    " << (get_FRAME_BEGIN() ? "1 (true)" : "0 (false)") << "\n";
		std::cout << "  Packed: 0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(packed) << std::dec << "\n";
		std::cout << "  Status: " << get_status() << std::endl;
		
		std::cout.flags(old_flags);
	}
	
	// Краткий вывод
	void print_short(const char* prefix = "") const {
		std::cout << prefix << to_string() << std::endl;
	}
};


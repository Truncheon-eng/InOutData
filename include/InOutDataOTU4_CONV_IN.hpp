#pragma once
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <iomanip>
#include <array>
#include <cstring>
#include <vector>
#include <bitset>
#include <sstream>
#include <stdexcept>

#include "Definitions.hpp"
#include "InOutFramerTXDataStrucHeader.hpp"
#include "InOutDataOTU4_CONV_SCLR.hpp"

// ------------------------------------------------------------------
//  Входные данные OTU4_CONVERTER
// ------------------------------------------------------------------
struct OTU4_CONVERTER_InputData {
	static constexpr int OTU4_CONV_TOTAL_BYTES = 40;   // 320 бит = 40 байт
	static constexpr int SCALAR_TOTAL_BYTES = 1;       // 2 бита в 1 байте
	static constexpr int TOTAL_BYTES = OTU4_CONV_TOTAL_BYTES + SCALAR_TOTAL_BYTES;  // 41 байт
	
	// 1. 320-битные данные (первые 40 байт)
	InOutData320 otl4_10_data_i{};
	
	// 2. 2-битные скалярные сигналы (41-й байт, биты 1-0)
	InSignalOtu4Converter scalar_signals_i{};
	
	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	OTU4_CONVERTER_InputData() = default;
	
	// Основной конструктор
	OTU4_CONVERTER_InputData(const InOutData320& data320,
							 const InSignalOtu4Converter& scalar)
		: otl4_10_data_i(data320), 
		  scalar_signals_i(scalar)
	{}
	
	// Конструктор из примитивных типов
	OTU4_CONVERTER_InputData(const InOutData320& data320,
							 bool otu4_conv_data_valid,
							 bool n_hard_reset)
		: otl4_10_data_i(data320),
		  scalar_signals_i(otu4_conv_data_valid, n_hard_reset)
	{}
	
	// Конструктор из байтового массива (41 байт)
	explicit OTU4_CONVERTER_InputData(const uint8_t* data, 
									  size_t len = TOTAL_BYTES) {
		if (len < TOTAL_BYTES) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": data too small (" << len 
					  << " < " << TOTAL_BYTES << " bytes)" << NORMAL << std::endl;
			throw std::runtime_error("Input data too small for OTU4_CONVERTER_InputData");
		}
		
		// 1. 320-битные данные (первые 40 байт)
		otl4_10_data_i.set_from_bytes(data, OTU4_CONV_TOTAL_BYTES);
		
		// 2. 2-битные скалярные сигналы (41-й байт, биты 1-0)
		// Байт 40 (индекс 40), биты 1-0 занимают младшие 2 бита
		scalar_signals_i.packed = data[OTU4_CONV_TOTAL_BYTES] & 0x03;  // Маска на 2 бита
	}
	
	// Конструктор из вектора байт
	explicit OTU4_CONVERTER_InputData(const std::vector<uint8_t>& data) {
		if (data.size() < TOTAL_BYTES) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": vector too small (" << data.size() 
					  << " < " << TOTAL_BYTES << " bytes)" << NORMAL << std::endl;
			throw std::runtime_error("Vector too small for OTU4_CONVERTER_InputData");
		}
		
		// 1. 320-битные данные
		otl4_10_data_i.set_from_bytes(data.data(), OTU4_CONV_TOTAL_BYTES);
		
		// 2. Скалярные сигналы
		scalar_signals_i.packed = data[OTU4_CONV_TOTAL_BYTES] & 0x03;
	}
	
	// -------------------------------------------------
	// Методы доступа
	// -------------------------------------------------
	const InOutData320& get_otl4_10_data() const { 
		return otl4_10_data_i;
	}
	
	InOutData320& get_otl4_10_data() { 
		return otl4_10_data_i;
	}
	
	const InSignalOtu4Converter& get_scalar_signals() const { 
		return scalar_signals_i; 
	}
	
	InSignalOtu4Converter& get_scalar_signals() { 
		return scalar_signals_i; 
	}
	
	// Геттеры для отдельных сигналов
	bool get_data_valid_in() const {
		return scalar_signals_i.get_DATA_VALID_IN();
	}
	
	bool get_nhard_reset() const {
		return scalar_signals_i.get_nHARD_RESET();
	}
	
	// -------------------------------------------------
	// Сеттеры
	// -------------------------------------------------
	void set_scalar_signals(const InSignalOtu4Converter& signals) { 
		scalar_signals_i = signals; 
	}
	
	void set_scalar_signals(bool otu4_conv_data_valid,
							bool n_hard_reset) {
		scalar_signals_i.set_all(otu4_conv_data_valid, n_hard_reset);
	}
	
	void set_otl4_10_data(const InOutData320& data) { 
		otl4_10_data_i = data; 
	}
	
	void set_data_valid_in(bool valid) {
		scalar_signals_i.set_DATA_VALID_IN(valid);
	}
	
	void set_nhard_reset(bool reset) {
		scalar_signals_i.set_nHARD_RESET(reset);
	}
	
	// -------------------------------------------------
	// Методы проверки состояния
	// -------------------------------------------------
	bool is_valid() const {
		// Данные валидны, если не в режиме сброса
		return !is_reset_active();
	}
	
	bool is_reset_active() const {
		// nHARD_RESET активен в 0 (active low)
		return scalar_signals_i.get_nHARD_RESET() == 0;
	}
	
	bool is_data_valid() const {
		return scalar_signals_i.get_DATA_VALID_IN() == 1;
	}
	
	bool is_operational() const {
		// Операционный режим: нет сброса и есть валидные данные
		return !is_reset_active() && is_data_valid();
	}
	
	// -------------------------------------------------
	// Вспомогательные методы
	// -------------------------------------------------
	void clear() {
		otl4_10_data_i.clear();
		scalar_signals_i = InSignalOtu4Converter();  // Вызовет конструктор по умолчанию
	}
	
	void set_reset_mode() {
		// Активировать сброс (active low)
		scalar_signals_i.set_nHARD_RESET(0);
		scalar_signals_i.set_DATA_VALID_IN(0);
	}
	
	void set_operational_mode() {
		// Выйти из сброса, активировать данные
		scalar_signals_i.set_nHARD_RESET(1);      // Сброс неактивен
		scalar_signals_i.set_DATA_VALID_IN(1);    // Данные валидны
	}
	
	void set_test_pattern() {
		// Установить тестовый паттерн в данные
		otl4_10_data_i.clear();
		// Например, установить чередующийся паттерн
		for (int i = 0; i < 10; i++) {
			otl4_10_data_i.set_word(i, (i % 2 == 0) ? 0xAAAAAAAA : 0x55555555);
		}
		set_operational_mode();
	}
	
	// -------------------------------------------------
	// Методы отладки и вывода
	// -------------------------------------------------
	void print(const char* name = "OTU4_CONVERTER Input Data") const {
		std::ios_base::fmtflags old_flags = std::cout.flags();
		
		std::cout << "\n=== " << name << " ===" << std::endl;
		std::cout << "Total size: " << TOTAL_BYTES << " bytes (" 
				  << (TOTAL_BYTES * 8) << " bits)" << std::endl;
		
		std::cout << "\n1. OTL4_10 Data (320 bits, 40 bytes):" << std::endl;
		//otl4_10_data_i.print_summary();  // Используем summary для краткости
		
		std::cout << "\n2. Scalar Signals (2 bits):" << std::endl;
		scalar_signals_i.print("  ");
		
		std::cout << "\n3. Status Summary:" << std::endl;
		std::cout << "   Operational: " << (is_operational() ? "YES" : "NO") << std::endl;
		std::cout << "   Reset active: " << (is_reset_active() ? "YES" : "NO") << std::endl;
		std::cout << "   Data valid: " << (is_data_valid() ? "YES" : "NO") << std::endl;
		std::cout << "   Overall valid: " << (is_valid() ? "YES" : "NO") << std::endl;
		
		std::cout.flags(old_flags);
	}
	
	void print_summary() const {
		std::ios_base::fmtflags old_flags = std::cout.flags();
		
		std::cout << "OTU4_CONVERTER_InputData: ";
		std::cout << "Scalar[";
		std::cout << (get_data_valid_in() ? "DV" : "dv");
		std::cout << (get_nhard_reset() ? "_RST" : "_rst");
		std::cout << "] ";
		std::cout << "320b=[" << std::hex << std::setfill('0');
		
		// Выводим первые 3 слова для краткости
		for (int i = 0; i < std::min(3, 10); i++) {
			if (i > 0) std::cout << " ";
			std::cout << std::setw(8) << otl4_10_data_i.get_word(i);
		}
		if (10 > 3) std::cout << " ...";
		std::cout << "]" << std::dec;
		
		std::cout.flags(old_flags);
		std::cout << std::endl;
	}
	
	void print_bytes() const {
		std::ios_base::fmtflags old_flags = std::cout.flags();
		
		std::cout << "\nOTU4_CONVERTER_InputData (" << TOTAL_BYTES << " bytes):" << std::endl;
		
		// 320-битные данные (40 байт)
		std::cout << "Bytes 0-39 (OTL4_10 data):" << std::endl;
		std::cout << "  ";
		for (int i = 0; i < OTU4_CONV_TOTAL_BYTES; i++) {
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
					 << static_cast<int>(otl4_10_data_i.get_byte(i)) << " ";
			if ((i + 1) % 16 == 0 && i != OTU4_CONV_TOTAL_BYTES - 1) {
				std::cout << std::endl << "  ";
			}
		}
		
		// Скалярные сигналы (1 байт)
		std::cout << "\nByte 40 (scalar signals): 0x" << std::hex 
				  << std::setw(2) << std::setfill('0')
				  << static_cast<int>(scalar_signals_i.packed) 
				  << " (bits 1-0: DATA_VALID_IN=" 
				  << scalar_signals_i.get_DATA_VALID_IN()
				  << ", nHARD_RESET=" << scalar_signals_i.get_nHARD_RESET()
				  << ")" << std::dec << std::endl;
		
		std::cout.flags(old_flags);
	}
	
	// -------------------------------------------------
	// Сериализация/десериализация
	// -------------------------------------------------
	std::vector<uint8_t> serialize() const {
		std::vector<uint8_t> result;
		result.reserve(TOTAL_BYTES);
		
		// 1. 320-битные данные (40 байт)
		for (int i = 0; i < OTU4_CONV_TOTAL_BYTES; i++) {
			result.push_back(otl4_10_data_i.get_byte(i));
		}
		
		// 2. 2-битные скалярные сигналы (1 байт, биты 1-0)
		// Просто записываем как есть, остальные биты будут 0
		result.push_back(scalar_signals_i.packed & 0x03);
		
		return result;
	}
	
	void serialize_to(uint8_t* buffer, size_t buffer_size) const {
		if (buffer_size < TOTAL_BYTES) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": buffer too small (" << buffer_size 
					  << " < " << TOTAL_BYTES << " bytes)" << NORMAL << std::endl;
			throw std::runtime_error("Buffer too small for serialization");
		}
		
		// 1. 320-битные данные
		for (int i = 0; i < OTU4_CONV_TOTAL_BYTES; i++) {
			buffer[i] = otl4_10_data_i.get_byte(i);
		}
		
		// 2. Скалярные сигналы
		buffer[OTU4_CONV_TOTAL_BYTES] = scalar_signals_i.packed & 0x03;
	}
	
	static OTU4_CONVERTER_InputData deserialize(const std::vector<uint8_t>& data) {
		return OTU4_CONVERTER_InputData(data);
	}
	
	static OTU4_CONVERTER_InputData deserialize(const uint8_t* data, 
												size_t len) {
		if (len < TOTAL_BYTES) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": data too small (" << len 
					  << " < " << TOTAL_BYTES << " bytes)" << NORMAL << std::endl;
			throw std::runtime_error("Data too small for deserialization");
		}
		return OTU4_CONVERTER_InputData(data, len);
	}
	
	// -------------------------------------------------
	// Операторы сравнения
	// -------------------------------------------------
	bool operator==(const OTU4_CONVERTER_InputData& other) const {
		return otl4_10_data_i == other.otl4_10_data_i &&
			   scalar_signals_i == other.scalar_signals_i;
	}
	
	bool operator!=(const OTU4_CONVERTER_InputData& other) const {
		return !(*this == other);
	}
	
	// -------------------------------------------------
	// Операторы ввода/вывода
	// -------------------------------------------------
	friend std::ostream& operator<<(std::ostream& os, 
									const OTU4_CONVERTER_InputData& data) {
		os << "OTU4_CONVERTER_InputData[";
		os << "DV_IN=" << data.scalar_signals_i.get_DATA_VALID_IN();
		os << ", nRST=" << data.scalar_signals_i.get_nHARD_RESET();
		os << ", 320b=";
		
		std::ios_base::fmtflags old_flags = os.flags();
		os << std::hex << std::setfill('0');
		for (int i = 0; i < std::min(2, 10); i++) {
			if (i > 0) os << ":";
			os << std::setw(8) << data.otl4_10_data_i.get_word(i);
		}
		if (10 > 2) os << "...";
		os << std::dec;
		os.flags(old_flags);
		
		os << "]";
		return os;
	}
	
	// -------------------------------------------------
	// Утилиты для работы с файлами
	// -------------------------------------------------
	bool save_to_file(const std::string& filename) const {
		std::ofstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": cannot open file '" << filename << "'" << NORMAL << std::endl;
			return false;
		}
		
		auto serialized = serialize();
		file.write(reinterpret_cast<const char*>(serialized.data()), serialized.size());
		
		bool success = file.good();
		if (!success) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": write failed for file '" << filename << "'" << NORMAL << std::endl;
		}
		
		return success;
	}
	
	static OTU4_CONVERTER_InputData load_from_file(const std::string& filename) {
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			std::string msg = "Cannot open file: " + filename;
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": " << msg << NORMAL << std::endl;
			throw std::runtime_error(msg);
		}
		
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		
		if (size != TOTAL_BYTES) {
			std::string msg = "Invalid file size: expected " + 
							 std::to_string(TOTAL_BYTES) + 
							 " bytes, got " + std::to_string(size);
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": " << msg << NORMAL << std::endl;
			throw std::runtime_error(msg);
		}
		
		std::vector<uint8_t> buffer(size);
		if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
			std::string msg = "Failed to read file: " + filename;
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": " << msg << NORMAL << std::endl;
			throw std::runtime_error(msg);
		}
		
		return deserialize(buffer);
	}
};

// Вспомогательная функция для тестирования
// inline void test_otu4_converter_input_data() {
// 	std::cout << "\n=== Testing OTU4_CONVERTER_InputData ===" << std::endl;
	
// 	// Тест 1: Создание и вывод
// 	auto test_data = OTU4_CONVERTER_InputData::create_test_pattern();
// 	test_data.print("Test Pattern");
	
// 	// Тест 2: Сериализация/десериализация
// 	auto serialized = test_data.serialize();
// 	std::cout << "\nSerialized size: " << serialized.size() << " bytes" << std::endl;
	
// 	auto deserialized = OTU4_CONVERTER_InputData::deserialize(serialized);
// 	if (test_data == deserialized) {
// 		std::cout << "✓ Serialization/deserialization test PASSED" << std::endl;
// 	} else {
// 		std::cout << "✗ Serialization/deserialization test FAILED" << std::endl;
// 	}
	
// 	// Тест 3: Режимы работы
// 	auto reset_data = OTU4_CONVERTER_InputData::create_reset_pattern();
// 	std::cout << "\nReset mode: " << (reset_data.is_reset_active() ? "ACTIVE" : "INACTIVE") << std::endl;
	
// 	auto op_data = OTU4_CONVERTER_InputData::create_operational_pattern();
// 	std::cout << "Operational mode: " << (op_data.is_operational() ? "YES" : "NO") << std::endl;
	
// 	std::cout << "\n=== End of Test ===" << std::endl;
// }

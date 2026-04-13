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

#ifdef VERILATOR
#include "VerilatorWrapper.hpp"
#endif

// ------------------------------------------------------------------
//  Выходные данные OTU4_CONVERTER
// ------------------------------------------------------------------
struct OTU4_CONVERTER_OutputData {
	static constexpr int OTU4_CONV_TOTAL_BYTES = 16;   // 128 бит = 16 байт
	static constexpr int SCALAR_TOTAL_BYTES = 1;       // 2 бита в 1 байте
	static constexpr int TOTAL_BYTES = OTU4_CONV_TOTAL_BYTES + SCALAR_TOTAL_BYTES;  // 17 байт
	
	// 1. 128-битный результат (первые 16 байт)
	InOutData128 data_out{};
	
	// 2. 2-битные скалярные сигналы (17-й байт, биты 1-0)
	OutSignalOtu4Converter scalar_signals_o{};
	
	// --------------------------------------------------------------
	// Конструкторы
	// --------------------------------------------------------------
	OTU4_CONVERTER_OutputData() = default;
	
	// Основной конструктор
	OTU4_CONVERTER_OutputData(const InOutData128& data128,
							  const OutSignalOtu4Converter& scalar)
		: data_out(data128),
		  scalar_signals_o(scalar)
	{}
	
	// Конструктор из примитивных типов
	OTU4_CONVERTER_OutputData(const InOutData128& data128,
							  bool data_valid_out,
							  bool frame_begin)
		: data_out(data128),
		  scalar_signals_o(data_valid_out, frame_begin)
	{}
	
	// Конструктор из байтового массива (17 байт)
	explicit OTU4_CONVERTER_OutputData(const uint8_t* data, 
									   size_t len = TOTAL_BYTES) {
		if (len < TOTAL_BYTES) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": data too small (" << len 
					  << " < " << TOTAL_BYTES << " bytes)" << NORMAL << std::endl;
			throw std::runtime_error("Input data too small for OTU4_CONVERTER_OutputData");
		}
		
		// 1. 128-битные данные (первые 16 байт)
		data_out.set_from_bytes(data, OTU4_CONV_TOTAL_BYTES);
		
		// 2. 2-битные скалярные сигналы (17-й байт, индекс 16)
		scalar_signals_o.packed = data[OTU4_CONV_TOTAL_BYTES] & 0x03;  // Маска на 2 бита
	}
	
	// Конструктор из вектора байт
	explicit OTU4_CONVERTER_OutputData(const std::vector<uint8_t>& data) {
		if (data.size() < TOTAL_BYTES) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": vector too small (" << data.size() 
					  << " < " << TOTAL_BYTES << " bytes)" << NORMAL << std::endl;
			throw std::runtime_error("Vector too small for OTU4_CONVERTER_OutputData");
		}
		
		// 1. 128-битные данные
		data_out.set_from_bytes(data.data(), OTU4_CONV_TOTAL_BYTES);
		
		// 2. Скалярные сигналы
		scalar_signals_o.packed = data[OTU4_CONV_TOTAL_BYTES] & 0x03;
	}
	
#ifdef VERILATOR
	// --------------------------------------------------------------
	// Чтение сигналов из DUT
	// --------------------------------------------------------------
	template<typename Top>
	void read_from_dut(const VerilatorWrapper<Top>& dut) {
		// const auto* dut_raw = dut.raw();
		
		// // 1. 2-битные скалярные сигналы (младшие 2 бита)
		// scalar_signals_o.packed = static_cast<uint8_t>(dut_raw->scalar_signals_o) & 0x03;
		
		// // 2. 128-битные данные (4 слова по 32 бита)
		// for (size_t i = 0; i < InOutData128::NUM_WORDS; ++i) {
		// 	if (i < 4) {  // Проверка на всякий случай
		// 		data_out.words[i] = dut_raw->data_out_112bit[i];
		// 	}
		// }
		
		// // Применяем маску для корректности (если нужно)
		// data_out.apply_mask();
	}
	
	// Альтернативная версия с явными именами сигналов
	template<typename Top>
	void read_from_dut_explicit(const VerilatorWrapper<Top>& dut) {
		// const auto* dut_raw = dut.raw();
		
		// // Если в Verilog сигналы называются по-другому:
		// // scalar_signals_o.packed = static_cast<uint8_t>(dut_raw->otu4_conv_data_valid_out) & 0x01;
		// // scalar_signals_o.packed |= (static_cast<uint8_t>(dut_raw->otu4_conv_frame_begin) & 0x01) << 1;
		
		// // Заполняем данные
		// for (size_t i = 0; i < InOutData128::NUM_WORDS; ++i) {
		// 	data_out.words[i] = dut_raw->data_out[i];
		// }
	}
#endif
	
	// --------------------------------------------------------------
	// Методы доступа
	// --------------------------------------------------------------
	const InOutData128& get_data_out() const { 
		return data_out; 
	}
	
	InOutData128& get_data_out() { 
		return data_out; 
	}
	
	const OutSignalOtu4Converter& get_scalar_signals() const { 
		return scalar_signals_o; 
	}
	
	OutSignalOtu4Converter& get_scalar_signals() { 
		return scalar_signals_o; 
	}
	
	// Геттеры для отдельных сигналов
	bool get_data_valid_out() const {
		return scalar_signals_o.get_DATA_VALID_OUT();
	}
	
	bool get_frame_begin() const {
		return scalar_signals_o.get_FRAME_BEGIN();
	}
	
	// --------------------------------------------------------------
	// Сеттеры
	// --------------------------------------------------------------
	void set_data_out(const InOutData128& data) { 
		data_out = data; 
	}
	
	void set_scalar_signals(const OutSignalOtu4Converter& signals) { 
		scalar_signals_o = signals; 
	}
	
	void set_scalar_signals(bool data_valid_out, bool frame_begin) {
		scalar_signals_o.set_all(data_valid_out, frame_begin);
	}
	
	void set_data_valid_out(bool valid) {
		scalar_signals_o.set_DATA_VALID_OUT(valid);
	}
	
	void set_frame_begin(bool begin) {
		scalar_signals_o.set_FRAME_BEGIN(begin);
	}
	
	// --------------------------------------------------------------
	// Методы проверки состояния
	// --------------------------------------------------------------
	bool is_data_valid_out() const {
		return scalar_signals_o.get_DATA_VALID_OUT() == 1;
	}
	
	bool has_frame_begin() const {
		return scalar_signals_o.get_FRAME_BEGIN() == 1;
	}
	
	bool is_output_valid() const {
		// Выходные данные валидны, когда есть data_valid_out
		return is_data_valid_out();
	}
	
	bool is_frame_start() const {
		// Начало кадра: valid + frame_begin
		return is_data_valid_out() && has_frame_begin();
	}
	
	bool is_frame_continuation() const {
		// Продолжение кадра: valid, но не начало
		return is_data_valid_out() && !has_frame_begin();
	}
	
	// --------------------------------------------------------------
	// Вспомогательные методы
	// --------------------------------------------------------------
	void clear() {
		data_out.clear();
		scalar_signals_o = OutSignalOtu4Converter();  // Обнуление через конструктор
	}
	
	void set_test_pattern() {
		// Установить тестовый паттерн
		data_out.clear();
		for (int i = 0; i < InOutData128::NUM_WORDS; i++) {
			data_out.set_word(i, 0xAA55AA55 + i);
		}
		set_scalar_signals(true, false);  // Данные валидны, не начало кадра
	}
	
	void set_frame_start_pattern(uint32_t frame_id = 0) {
		// Установить паттерн начала кадра
		data_out.clear();
		// Первое слово - идентификатор кадра
		data_out.set_word(0, frame_id);
		// Остальные - данные
		for (int i = 1; i < InOutData128::NUM_WORDS; i++) {
			data_out.set_word(i, 0xDEADBEEF + i);
		}
		set_scalar_signals(true, true);  // Начало кадра
	}
	
	// --------------------------------------------------------------
	// Методы отладки и вывода
	// --------------------------------------------------------------
	void print(const char* title = "OTU4_CONVERTER Output Data") const {
		std::ios_base::fmtflags old_flags = std::cout.flags();
		
		std::cout << "\n=== " << title << " ===" << std::endl;
		std::cout << "Total size: " << TOTAL_BYTES << " bytes (" 
				  << (TOTAL_BYTES * 8) << " bits)" << std::endl;
		
		std::cout << "\n1. Scalar Signals (2 bits):" << std::endl;
		scalar_signals_o.print("  ");
		
		std::cout << "\n2. 128-bit Data Output:" << std::endl;
		//TODO: Реализовать
		//data_out.print_summary("  ");
		
		std::cout << "\n3. Status Summary:" << std::endl;
		std::cout << "   Data Valid: " << (is_data_valid_out() ? "YES" : "NO") << std::endl;
		std::cout << "   Frame Begin: " << (has_frame_begin() ? "YES" : "NO") << std::endl;
		std::cout << "   Output Valid: " << (is_output_valid() ? "YES" : "NO") << std::endl;
		std::cout << "   Frame State: ";
		if (is_frame_start()) {
			std::cout << "START";
		} else if (is_frame_continuation()) {
			std::cout << "CONTINUATION";
		} else {
			std::cout << "INVALID";
		}
		std::cout << std::endl;
		
		std::cout.flags(old_flags);
	}
	
	void print_summary() const {
		std::ios_base::fmtflags old_flags = std::cout.flags();
		
		std::cout << "OTU4_OUT: ";
		std::cout << "Sigs[";
		std::cout << (get_data_valid_out() ? "DV" : "dv");
		std::cout << (get_frame_begin() ? "_FB" : "_fb");
		std::cout << "] ";
		std::cout << "128b=[" << std::hex << std::setfill('0');
		
		// Выводим все 4 слова (128 бит)
		for (int i = 0; i < InOutData128::NUM_WORDS; i++) {
			if (i > 0) std::cout << " ";
			std::cout << std::setw(8) << data_out.get_word(i);
		}
		std::cout << "]" << std::dec;
		
		std::cout.flags(old_flags);
		std::cout << std::endl;
	}
	
	void print_bytes() const {
		std::ios_base::fmtflags old_flags = std::cout.flags();
		
		std::cout << "\nOTU4_CONVERTER_OutputData (" << TOTAL_BYTES << " bytes):" << std::endl;
		
		// 128-битные данные (16 байт)
		std::cout << "Bytes 0-15 (128-bit data):" << std::endl;
		std::cout << "  ";
		for (int i = 0; i < OTU4_CONV_TOTAL_BYTES; i++) {
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
					 << static_cast<int>(data_out.get_byte(i)) << " ";
			if ((i + 1) % 8 == 0 && i != OTU4_CONV_TOTAL_BYTES - 1) {
				std::cout << std::endl << "  ";
			}
		}
		
		// Скалярные сигналы (1 байт)
		std::cout << "\nByte 16 (scalar signals): 0x" << std::hex 
				  << std::setw(2) << std::setfill('0')
				  << static_cast<int>(scalar_signals_o.packed) 
				  << " (bits 1-0: DATA_VALID_OUT=" 
				  << scalar_signals_o.get_DATA_VALID_OUT()
				  << ", FRAME_BEGIN=" << scalar_signals_o.get_FRAME_BEGIN()
				  << ")" << std::dec << std::endl;
		
		std::cout.flags(old_flags);
	}
	
	// --------------------------------------------------------------
	// Сериализация/десериализация
	// --------------------------------------------------------------
	std::vector<uint8_t> serialize() const {
		std::vector<uint8_t> result;
		result.reserve(TOTAL_BYTES);
		
		// 1. 128-битные данные (16 байт)
		for (int i = 0; i < OTU4_CONV_TOTAL_BYTES; i++) {
			result.push_back(data_out.get_byte(i));
		}
		
		// 2. 2-битные скалярные сигналы (1 байт)
		result.push_back(scalar_signals_o.packed & 0x03);
		
		return result;
	}
	
	void serialize_to(uint8_t* buffer, size_t buffer_size) const {
		if (buffer_size < TOTAL_BYTES) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": buffer too small (" << buffer_size 
					  << " < " << TOTAL_BYTES << " bytes)" << NORMAL << std::endl;
			throw std::runtime_error("Buffer too small for serialization");
		}
		
		// 1. 128-битные данные
		for (int i = 0; i < OTU4_CONV_TOTAL_BYTES; i++) {
			buffer[i] = data_out.get_byte(i);
		}
		
		// 2. Скалярные сигналы
		buffer[OTU4_CONV_TOTAL_BYTES] = scalar_signals_o.packed & 0x03;
	}
	
	static OTU4_CONVERTER_OutputData deserialize(const std::vector<uint8_t>& data) {
		return OTU4_CONVERTER_OutputData(data);
	}
	
	static OTU4_CONVERTER_OutputData deserialize(const uint8_t* data, 
												 size_t len) {
		if (len < TOTAL_BYTES) {
			std::cerr << RED << CROSS_MARK << " " << __FUNCTION__ 
					  << ": data too small (" << len 
					  << " < " << TOTAL_BYTES << " bytes)" << NORMAL << std::endl;
			throw std::runtime_error("Data too small for deserialization");
		}
		return OTU4_CONVERTER_OutputData(data, len);
	}
	
	// --------------------------------------------------------------
	// Операторы сравнения
	// --------------------------------------------------------------
	bool operator==(const OTU4_CONVERTER_OutputData& other) const {
		return data_out == other.data_out &&
			   scalar_signals_o == other.scalar_signals_o;
	}
	
	bool operator!=(const OTU4_CONVERTER_OutputData& other) const {
		return !(*this == other);
	}
	
	// --------------------------------------------------------------
	// Операторы ввода/вывода
	// --------------------------------------------------------------
	friend std::ostream& operator<<(std::ostream& os, 
									const OTU4_CONVERTER_OutputData& data) {
		std::ios_base::fmtflags old_flags = os.flags();
		
		os << "OTU4_OUT[";
		os << "DV=" << data.scalar_signals_o.get_DATA_VALID_OUT();
		os << ",FB=" << data.scalar_signals_o.get_FRAME_BEGIN();
		os << ",128b=";
		
		os << std::hex << std::setfill('0');
		for (int i = 0; i < std::min(2, InOutData128::NUM_WORDS); i++) {
			if (i > 0) os << ":";
			os << std::setw(8) << data.data_out.get_word(i);
		}
		if (InOutData128::NUM_WORDS > 2) os << "...";
		os << std::dec;
		
		os.flags(old_flags);
		os << "]";
		
		return os;
	}
	
	// --------------------------------------------------------------
	// Утилиты для работы с файлами
	// --------------------------------------------------------------
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
	
	static OTU4_CONVERTER_OutputData load_from_file(const std::string& filename) {
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
	
	// --------------------------------------------------------------
	// Статические методы для тестирования
	// --------------------------------------------------------------
	static OTU4_CONVERTER_OutputData create_test_pattern() {
		OTU4_CONVERTER_OutputData data;
		data.set_test_pattern();
		return data;
	}
	
	static OTU4_CONVERTER_OutputData create_frame_start(uint32_t frame_id = 0) {
		OTU4_CONVERTER_OutputData data;
		data.set_frame_start_pattern(frame_id);
		return data;
	}
	
	static OTU4_CONVERTER_OutputData create_frame_continuation() {
		OTU4_CONVERTER_OutputData data;
		data.set_scalar_signals(true, false);  // Valid, но не начало
		// Заполняем данными
		for (int i = 0; i < InOutData128::NUM_WORDS; i++) {
			data.data_out.set_word(i, 0xCCCCCCCC + i);
		}
		return data;
	}
	
	static OTU4_CONVERTER_OutputData create_invalid_output() {
		OTU4_CONVERTER_OutputData data;
		data.set_scalar_signals(false, false);  // Не valid
		return data;
	}
};

// Вспомогательная функция для тестирования
inline void test_otu4_converter_output_data() {
	std::cout << "\n=== Testing OTU4_CONVERTER_OutputData ===" << std::endl;
	
	// Тест 1: Создание и вывод
	auto test_data = OTU4_CONVERTER_OutputData::create_test_pattern();
	test_data.print("Test Pattern");
	
	// Тест 2: Сериализация/десериализация
	auto serialized = test_data.serialize();
	std::cout << "\nSerialized size: " << serialized.size() << " bytes" << std::endl;
	
	auto deserialized = OTU4_CONVERTER_OutputData::deserialize(serialized);
	if (test_data == deserialized) {
		std::cout << GREEN << CHECK_MARK << " Serialization/deserialization test PASSED" << NORMAL << std::endl;
	} else {
		std::cout << RED << CROSS_MARK << " Serialization/deserialization test FAILED" << NORMAL << std::endl;
	}
	
	// Тест 3: Различные состояния
	auto frame_start = OTU4_CONVERTER_OutputData::create_frame_start(0x1234);
	std::cout << "\nFrame start: " << (frame_start.is_frame_start() ? "YES" : "NO") << std::endl;
	
	auto frame_cont = OTU4_CONVERTER_OutputData::create_frame_continuation();
	std::cout << "Frame continuation: " << (frame_cont.is_frame_continuation() ? "YES" : "NO") << std::endl;
	
	auto invalid = OTU4_CONVERTER_OutputData::create_invalid_output();
	std::cout << "Invalid output: " << (invalid.is_output_valid() ? "VALID" : "INVALID") << std::endl;
	
	std::cout << "\n=== End of Test ===" << std::endl;
}
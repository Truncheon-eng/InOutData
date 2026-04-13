#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <iostream>

//#include <verilated.h>
//#include <verilated_vcd_c.h>

//#include "VerilatorWrapper.hpp"  // Добавляем вашу обертку
#include "InOutFramerTXDataStrucHeader.hpp"

// ------------------------------------------------------------------
//  Утилиты для вывода
// ------------------------------------------------------------------
namespace detail {
	/// Округление вывода в hex‑формате: 0xXXXXXXXX
	inline std::ostream& hex32(std::ostream& os, uint32_t v) {
		return os << "0x" << std::hex << std::setw(8) << std::setfill('0')
				  << v << std::dec;
	}
} // namespace detail

// ------------------------------------------------------------------
//  Входные данные FRAMER_PCS_TX
// ------------------------------------------------------------------
// Структура для входных данных FramerTX
struct FramerTXInputData {
	
	// 1. 320-битные данные (первые 40 байт)
	InOutData320 otl4_10_data_i{};
	
	// 2. 8-битные данные (41-й байт)
	InOutData8 thoxidat_i{};
	
	// 3. 4-битные скалярные сигналы (42-й байт, биты 7-4)
	InOutSignals4 scalar_signals_i{};
	
	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	FramerTXInputData() = default;
	
	// Основной конструктор
	FramerTXInputData(const InOutData320& data320,
					  const InOutData8& data8,
					  const InOutSignals4& scalar)
		: otl4_10_data_i(data320), 
		  thoxidat_i(data8), 
		  scalar_signals_i(scalar)
	{}
	
	// Конструктор из примитивных типов
	FramerTXInputData(const InOutData320& data320,
					  uint8_t data8,
					  bool thohivld,
					  bool otl4_10_data_valid,
					  bool rate_control,
					  bool nhard_reset)
		: otl4_10_data_i(data320),
		  thoxidat_i(data8),
		  scalar_signals_i(thohivld, 
						  otl4_10_data_valid, 
						  rate_control, 
						  nhard_reset)
	{}
	
	// Конструктор из байтового массива (42 байта)
	explicit FramerTXInputData(const uint8_t* data, size_t len = 42) {
		if (len < 42) {
			throw std::runtime_error("Input data too small for FramerTXInputData");
		}
		
		// 1. 320-битные данные (первые 40 байт)
		otl4_10_data_i.set_from_bytes(data, 40);
		
		// 2. 8-битные данные (41-й байт)
		thoxidat_i.set_byte(data[40]);
		
		// 3. 4-битные скалярные сигналы (42-й байт, биты 7-4)
		scalar_signals_i.packed = (data[41] >> 4) & 0x0F;
	}
	
	// -------------------------------------------------
	// Методы доступа
	// -------------------------------------------------
	// Геттеры
	const InOutSignals4& get_scalar_signals() const { 
		return scalar_signals_i; 
	}
	
	const InOutData8& get_thoxidat() const { 
		return thoxidat_i; 
	}
	
	const InOutData320& get_otl4_10_data() const { 
		return otl4_10_data_i;
	}
	
	// Сеттеры
	void set_scalar_signals(const InOutSignals4& signals) { 
		scalar_signals_i = signals; 
	}
	
	void set_scalar_signals(bool thohivld,
						   bool otl4_10_data_valid,
						   bool rate_control,
						   bool nhard_reset) {
		scalar_signals_i.set_all(thohivld, 
								otl4_10_data_valid, 
								rate_control, 
								nhard_reset);
	}
	
	void set_thoxidat(uint8_t value) { 
		thoxidat_i.set_byte(value); 
	}
	
	void set_otl4_10_data(const InOutData320& data) { 
		otl4_10_data_i = data; 
	}
	
	void set_otl4_10_data(const uint8_t* data, size_t len = 40) {
		otl4_10_data_i.set_from_bytes(data, len);
	}
	
	// -------------------------------------------------
	// Методы проверки состояния
	// -------------------------------------------------
	bool is_valid() const {
		// Проверка валидности данных
		return scalar_signals_i.get_nHARD_RESET() == 0 &&    // сброс неактивен
			   scalar_signals_i.get_OTL4_10_DATA_VALID() == 1; // данные валидны
	}
	
	bool is_reset_active() const {
		return scalar_signals_i.get_nHARD_RESET() == 1;  // сброс активен
	}
	
	bool is_data_valid() const {
		return scalar_signals_i.get_OTL4_10_DATA_VALID() == 1;
	}
	
	// -------------------------------------------------
	// Вспомогательные методы
	// -------------------------------------------------
	void clear() {
		otl4_10_data_i.clear();
		thoxidat_i.set_byte(0);
		scalar_signals_i.packed = 0;
	}
	
	void set_reset_sequence() {
		scalar_signals_i.set_reset_sequence();
		// Можно также сбросить другие поля при необходимости
	}
	
	void set_normal_operation() {
		scalar_signals_i.set_normal_operation();
		// Установить значения для других полей
	}
	
	// -------------------------------------------------
	// Методы отладки и вывода
	// -------------------------------------------------
	void print(const char* name = "FramerTX Input Data") const {
		std::cout << "=== " << name << " ===" << std::endl;
		
		std::cout << "\n1. Scalar Signals (4 bits):" << std::endl;
		scalar_signals_i.print("  ");
		
		std::cout << "\n2. THOXIDAT (8 bits):" << std::endl;
		thoxidat_i.print("  ");
		
		std::cout << "\n3. OTL4_10 Data (320 bits):" << std::endl;
		otl4_10_data_i.print("  ");
		
		std::cout << "\nStatus: " 
				  << (is_valid() ? "VALID" : "INVALID")
				  << (is_data_valid() ? " DATA_VALID" : " DATA_INVALID")
				  << (is_reset_active() ? " [RESET]" : "")
				  << std::endl;
	}
	
	void print_summary() const {
		std::cout << "FramerTXInputData: "
				  << "Scalar=0x" << std::hex << static_cast<int>(scalar_signals_i.packed)
				  << " THOXIDAT=0x" << static_cast<int>(thoxidat_i.get_byte())
				  << " 320b=[";
		
		// Выводим первые 4 слова для краткости
		std::cout << std::hex << std::setfill('0');
		for (int i = 0; i < std::min(4, 10); i++) {
			if (i > 0) std::cout << " ";
			std::cout << std::setw(8) << otl4_10_data_i.get_word(i);
		}
		if (10 > 4) std::cout << " ...";
		std::cout << "]" << std::dec << std::endl;
	}
	
	void print_bytes() const {
		std::cout << "FramerTXInputData bytes (42 bytes total):" << std::endl;
		
		// 320-битные данные (40 байт)
		std::cout << "  Bytes 0-39 (320-bit data):" << std::endl;
		std::cout << "  ";
		for (int i = 0; i < 40; i++) {
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
					 << (int)otl4_10_data_i.get_byte(i) << " ";
			if ((i + 1) % 16 == 0) std::cout << std::endl << "  ";
		}
		std::cout << std::dec << std::endl;
		
		// 8-битные данные (1 байт)
		std::cout << "  Byte 40 (THOXIDAT): 0x" << std::hex 
				  << std::setw(2) << std::setfill('0')
				  << (int)thoxidat_i.get_byte() << std::dec << std::endl;
		
		// 4-битные скалярные сигналы (1 байт, но только 4 бита)
		std::cout << "  Byte 41 (scalar signals): 0x" << std::hex
				  << std::setw(2) << std::setfill('0')
				  << (int)scalar_signals_i.packed << " (only bits 7-4 used)" << std::dec << std::endl;
	}
	
	// -------------------------------------------------
	// Сериализация/десериализация
	// -------------------------------------------------
	std::vector<uint8_t> serialize() const {
		std::vector<uint8_t> result;
		result.reserve(42);  // 40 + 1 + 1 = 42 байта
		
		// 1. 320-битные данные (40 байт)
		for (int i = 0; i < 40; i++) {
			result.push_back(otl4_10_data_i.get_byte(i));
		}
		
		// 2. 8-битные данные (1 байт)
		result.push_back(thoxidat_i.get_byte());
		
		// 3. 4-битные скалярные сигналы (1 байт, но используются только 4 бита)
		result.push_back(scalar_signals_i.packed << 4);  // Сдвигаем в старшие 4 бита
		
		return result;
	}
	
	static FramerTXInputData deserialize(const std::vector<uint8_t>& data) {
		if (data.size() < 42) {
			throw std::runtime_error("Invalid data size for deserialization: expected 42 bytes");
		}
		
		FramerTXInputData result;
		size_t idx = 0;
		
		// 1. 320-битные данные (40 байт)
		result.otl4_10_data_i.set_from_bytes(data.data(), 40);
		idx += 40;
		
		// 2. 8-битные данные (1 байт)
		result.thoxidat_i.set_byte(data[idx++]);
		
		// 3. 4-битные скалярные сигналы (1 байт, но используются только старшие 4 бита)
		result.scalar_signals_i.packed = (data[idx] >> 4) & 0x0F;
		
		return result;
	}
	
	// Десериализация из указателя
	static FramerTXInputData deserialize(const uint8_t* data, size_t len) {
		if (len < 42) {
			throw std::runtime_error("Invalid data size for deserialization");
		}
		
		return FramerTXInputData(data, 42);
	}
	
	// -------------------------------------------------
	// Операторы сравнения
	// -------------------------------------------------
	bool operator==(const FramerTXInputData& other) const {
		return scalar_signals_i.packed == other.scalar_signals_i.packed &&
			   thoxidat_i == other.thoxidat_i &&
			   otl4_10_data_i == other.otl4_10_data_i;
	}
	
	bool operator!=(const FramerTXInputData& other) const {
		return !(*this == other);
	}
	
	// -------------------------------------------------
	// Операторы ввода/вывода
	// -------------------------------------------------
	friend std::ostream& operator<<(std::ostream& os, const FramerTXInputData& data) {
		os << "FramerTXInputData("
		   << "scalar=0x" << std::hex << std::setw(1)
		   << static_cast<int>(data.scalar_signals_i.packed)
		   << ", thoxidat=0x" << std::setw(2) << std::setfill('0')
		   << static_cast<int>(data.thoxidat_i.get_byte())
		   << ", 320b=[" << std::setfill('0');
		
		for (int i = 0; i < 3; i++) {  // Выводим первые 3 слова для краткости
			if (i > 0) os << " ";
			os << std::setw(8) << data.otl4_10_data_i.get_word(i);
		}
		os << " ...])" << std::dec;
		
		return os;
	}
	
	// -------------------------------------------------
	// Утилиты для работы с файлами
	// -------------------------------------------------
	bool save_to_file(const std::string& filename) const {
		std::ofstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		
		auto serialized = serialize();
		file.write(reinterpret_cast<const char*>(serialized.data()), serialized.size());
		return file.good();
	}
	
	static FramerTXInputData load_from_file(const std::string& filename) {
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			throw std::runtime_error("Cannot open file: " + filename);
		}
		
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		
		if (size != 42) {
			throw std::runtime_error("Invalid file size: expected 42 bytes");
		}
		
		std::vector<uint8_t> buffer(size);
		if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
			throw std::runtime_error("Failed to read file");
		}
		
		return deserialize(buffer);
	}
	
	// -------------------------------------------------
	// Методы для тестирования
	// -------------------------------------------------
	static FramerTXInputData create_test_data() {
		InOutData320 data320;
		for (int i = 0; i < 10; i++) {
			data320.set_word(i, 0x12345678 + i * 0x11111111);
		}
		
		return FramerTXInputData(
			data320,
			0xAB,
			true,   // THOHIVLD
			true,   // OTL4_10_DATA_VALID
			false,  // RATE_CONTROL
			true    // nHARD_RESET (активный низкий уровень)
		);
	}
	
	static FramerTXInputData create_reset_sequence() {
		InOutData320 data320;  // Все нули
		FramerTXInputData result(data320, 0x00, false, false, false, true);
		result.scalar_signals_i.set_reset_sequence();
		return result;
	}
};

// ------------------------------------------------------------------
//  Выходные данные FRAMER_PCS_TX
// ------------------------------------------------------------------
struct FramerTXOutputData {
	
	// 1. 112‑битный результат (первые 14 байт)
	InOutData112 data_out{};
	
	// 2. 8‑битный «протокольный» результат (15-й байт)
	InOutData8 data_out_8bit{};
	
	// 3. Упакованные скалярные сигналы (5‑bit) (16-й байт, биты 7-3)
	InOutSignals5 scalar_signals_o{};
	
	// --------------------------------------------------------------
	// Конструкторы
	// --------------------------------------------------------------
	FramerTXOutputData() = default;
	
	FramerTXOutputData(const InOutData112& data112,
					   const InOutData8& data8,
					   const InOutSignals5& signals5)
		: data_out(data112),
		  data_out_8bit(data8),
		  scalar_signals_o(signals5)
	{}
	
	FramerTXOutputData(const InOutData112& data112,
					   uint8_t data8,
					   bool data_valid_out,
					   bool txohifp_out,
					   bool txohimfp_out,
					   bool fec_sof_out,
					   bool thohevalid)
		: data_out(data112),
		  data_out_8bit(data8),
		  scalar_signals_o(data_valid_out, txohifp_out, txohimfp_out, 
						  fec_sof_out, thohevalid)
	{}
	
	// Конструктор из байтового массива (16 байт)
	explicit FramerTXOutputData(const uint8_t* data, size_t len = 16) {
		if (len < 16) {
			throw std::runtime_error("Input data too small for FramerTXOutputData");
		}
		
		// 1. 112-битные данные (первые 14 байт)
		data_out.set_from_bytes(data, 14);
		
		// 2. 8-битные данные (15-й байт)
		data_out_8bit.set_byte(data[14]);
		
		// 3. 5-битные скалярные сигналы (16-й байт, биты 7-3)
		scalar_signals_o.packed = data[15] & 0xF8;  // Оставляем только биты 7-3
	}
	
#ifdef VERILATOR
	//TODO: оставить только для случая с InOutData112
	// т.к. в остальных случаях там типовые данные
	// --------------------------------------------------------------
	// Чтение сигналов из DUT (адаптировано для raw())
	// --------------------------------------------------------------
	template<typename Top>
	void read_from_dut(const VerilatorWrapper<Top>& dut) {
		const auto* dut_raw = dut.raw();
		
		// Копируем 8-битные данные
		data_out_8bit.set_byte(static_cast<uint8_t>(dut_raw->data_out_8bit));
		
		// Копируем 5-битные сигналы
		scalar_signals_o.packed = static_cast<uint8_t>(dut_raw->scalar_signals_o) & 0xF8;
		
		// Копируем 112-битные данные
		for (size_t i = 0; i < InOutData112::NUM_WORDS; ++i) {
			// Проверяем, что индекс в пределах массива
			if (i < 4) {
				data_out.words[i] = dut_raw->data_out_112bit[i];
			}
		}
		
		// Применяем маску для последнего слова (если нужно)
		data_out.apply_mask();
	}
#endif
	
	// --------------------------------------------------------------
	// Методы доступа
	// --------------------------------------------------------------
	const InOutData112& get_data_out() const { 
		return data_out; 
	}
	const InOutData8& get_data_out_8bit() const { 
		return data_out_8bit; 
	}
	const InOutSignals5& get_scalar_signals() const { 
		return scalar_signals_o; 
	}
	
	void set_data_out(const InOutData112& data) { 
		data_out = data; 
	}
	void set_data_out_8bit(uint8_t value) { 
		data_out_8bit.set_byte(value); 
	}
	void set_scalar_signals(const InOutSignals5& signals) { 
		scalar_signals_o = signals; 
	}
	
	// --------------------------------------------------------------
	// Методы проверки состояния
	// --------------------------------------------------------------
	bool is_data_valid() const {
		return scalar_signals_o.get_DATA_VALID_OUT() == 1;
	}
	
	bool has_frame_pulse() const {
		return scalar_signals_o.get_TXOHIFP_OUT() == 1;
	}
	
	bool has_multi_frame_pulse() const {
		return scalar_signals_o.get_TXOHIMFP_OUT() == 1;
	}
	
	bool has_fec_sof() const {
		return scalar_signals_o.get_FEC_SOF_OUT() == 1;
	}
	
	bool has_thohevalid() const {
		return scalar_signals_o.get_THOHEVALID() == 1;
	}
	
	// --------------------------------------------------------------
	// Методы отладки и вывода
	// --------------------------------------------------------------
	void print(const char* title = "FRAMER_TX OUTPUT") const {
		std::cout << "=== " << title << " ===" << std::endl;
		
		std::cout << "\n1. Scalar Signals (5 bits):" << std::endl;
		scalar_signals_o.print("  ");
		
		std::cout << "\n2. 8-bit Data Output:" << std::endl;
		data_out_8bit.print("  ");
		
		std::cout << "\n3. 112-bit Data Output:" << std::endl;
		//data_out.print_hex("  ");
		
		std::cout << "\nStatus: ";
		if (is_data_valid()) std::cout << "DATA_VALID ";
		if (has_frame_pulse()) std::cout << "FRAME_PULSE ";
		if (has_multi_frame_pulse()) std::cout << "MULTI_FRAME_PULSE ";
		if (has_fec_sof()) std::cout << "FEC_SOF ";
		if (has_thohevalid()) std::cout << "THOHEVALID ";
		std::cout << std::endl;
	}
	
	void print_bytes(const char* title = "FRAMER_TX OUTPUT Bytes") const {
		std::cout << "=== " << title << " ===" << std::endl;
		
		// 112-битные данные (14 байт)
		std::cout << "\nBytes 0-13 (112-bit data):" << std::endl;
		std::cout << "  ";
		for (int i = 0; i < 14; i++) {
			std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
					 << (int)data_out.get_byte(i) << " ";
			if ((i + 1) % 8 == 0) std::cout << std::endl << "  ";
		}
		std::cout << std::dec << std::endl;
		
		// 8-битные данные (1 байт)
		std::cout << "Byte 14 (8-bit data): 0x" << std::hex 
				  << std::setw(2) << std::setfill('0')
				  << (int)data_out_8bit.get_byte() << std::dec << std::endl;
		
		// 5-битные скалярные сигналы (1 байт, но только 5 бит)
		std::cout << "Byte 15 (scalar signals): 0x" << std::hex
				  << std::setw(2) << std::setfill('0')
				  << (int)scalar_signals_o.packed 
				  << " (only bits 7-3 used)" << std::dec << std::endl;
	}
	
	// --------------------------------------------------------------
	// Сериализация/десериализация
	// --------------------------------------------------------------
	std::vector<uint8_t> serialize() const {
		std::vector<uint8_t> result;
		result.reserve(16);  // 14 + 1 + 1 = 16 байт
		
		// 1. 112-битные данные (14 байт)
		for (int i = 0; i < 14; i++) {
			result.push_back(data_out.get_byte(i));
		}
		
		// 2. 8-битные данные (1 байт)
		result.push_back(data_out_8bit.get_byte());
		
		// 3. 5-битные скалярные сигналы (1 байт, но используются только биты 7-3)
		result.push_back(scalar_signals_o.packed);
		
		return result;
	}
	
	static FramerTXOutputData deserialize(const std::vector<uint8_t>& data) {
		if (data.size() < 16) {
			throw std::runtime_error("Invalid data size for deserialization: expected 16 bytes");
		}
		
		FramerTXOutputData result;
		size_t idx = 0;
		
		// 1. 112-битные данные (14 байт)
		result.data_out.set_from_bytes(data.data(), 14);
		idx += 14;
		
		// 2. 8-битные данные (1 байт)
		result.data_out_8bit.set_byte(data[idx++]);
		
		// 3. 5-битные скалярные сигналы (1 байт)
		result.scalar_signals_o.packed = data[idx] & 0xF8;  // Оставляем только биты 7-3
		
		return result;
	}
	
	// --------------------------------------------------------------
	// Операторы сравнения
	// --------------------------------------------------------------
	bool operator==(const FramerTXOutputData& other) const noexcept {
		// Сравниваем 112-битные данные
		for (size_t i = 0; i < InOutData112::NUM_WORDS; ++i) {
			if (data_out.get_word(i) != other.data_out.get_word(i)) {
				return false;
			}
		}
		
		// Сравниваем 8-битные данные
		if (data_out_8bit.get_byte() != other.data_out_8bit.get_byte()) {
			return false;
		}
		
		// Сравниваем 5-битные сигналы
		if (scalar_signals_o.packed != other.scalar_signals_o.packed) {
			return false;
		}
		
		return true;
	}
	
	bool operator!=(const FramerTXOutputData& other) const noexcept {
		return !(*this == other);
	}
	
	// --------------------------------------------------------------
	// Операторы ввода/вывода
	// --------------------------------------------------------------
	friend std::ostream& operator<<(std::ostream& os, const FramerTXOutputData& o) {
		os << "FramerTXOutputData("
		  // << "112b=0x" << o.data_out.to_hex_string()
		   << ", 8b=0x" << std::hex << std::setw(2) << std::setfill('0')
		   << static_cast<int>(o.data_out_8bit.get_byte())
		   << ", signals=0x" << std::setw(2)
		   << static_cast<int>(o.scalar_signals_o.packed)
		   << " [" << o.scalar_signals_o.get_status() << "]"
		   << ")" << std::dec;
		
		return os;
	}
	
	// --------------------------------------------------------------
	// Утилиты для работы с файлами
	// --------------------------------------------------------------
	bool save_to_file(const std::string& filename) const {
		std::ofstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			return false;
		}
		
		auto serialized = serialize();
		file.write(reinterpret_cast<const char*>(serialized.data()), serialized.size());
		return file.good();
	}
	
	static FramerTXOutputData load_from_file(const std::string& filename) {
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			throw std::runtime_error("Cannot open file: " + filename);
		}
		
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		
		if (size != 16) {
			throw std::runtime_error("Invalid file size: expected 16 bytes");
		}
		
		std::vector<uint8_t> buffer(size);
		if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
			throw std::runtime_error("Failed to read file");
		}
		
		return deserialize(buffer);
	}
	
	// --------------------------------------------------------------
	// Методы для тестирования
	// --------------------------------------------------------------
	static FramerTXOutputData create_test_data() {
		InOutData112 data112;
		for (int i = 0; i < 4; i++) {
			data112.set_word(i, 0x89ABCDEF + i * 0x11111111);
		}
		
		return FramerTXOutputData(
			data112,
			0x55,
			true,   // DATA_VALID_OUT
			false,  // TXOHIFP_OUT
			true,   // TXOHIMFP_OUT
			false,  // FEC_SOF_OUT
			true    // THOHEVALID
		);
	}
	
	static FramerTXOutputData create_empty() {
		InOutData112 data112;  // Все нули
		return FramerTXOutputData(data112, 0x00, false, false, false, false, false);
	}
};

// ------------------------------------------------------------------
//  Транзакция FRAMER_PCS_TX (адаптированная для VerilatorWrapper)
// ------------------------------------------------------------------
template<typename Top>
struct FramerTXTransaction {
	size_t              test_vector_id{0};  // ID тестового вектора
	std::string         test_name;          // Имя теста (опционально)
	FramerTXInputData   input;              // Входные данные
	FramerTXOutputData  expected;           // Ожидаемые выходные данные
	FramerTXOutputData  received;           // Полученные выходные данные
	bool                passed{false};      // Флаг прохождения теста
	std::string         failure_reason;     // Причина неудачи (если есть)
	uint64_t            timestamp{0};       // Временная метка выполнения

	// --------------------------------------------------------------
	//  Конструкторы
	// --------------------------------------------------------------
	FramerTXTransaction() = default;
	
	explicit FramerTXTransaction(size_t id, const std::string& name = "")
		: test_vector_id(id), test_name(name) {}
	
	FramerTXTransaction(size_t id, 
					   const FramerTXInputData& in,
					   const FramerTXOutputData& exp,
					   const std::string& name = "")
		: test_vector_id(id), test_name(name),
		  input(in), expected(exp) {}
	
	// --------------------------------------------------------------
	//  Методы для работы с DUT (Verilator)
	// --------------------------------------------------------------
#ifdef VERILATOR
	// TODO: оставить только для данных 320 и 112, 
	// т.к. это не типовые данные.
	// Применение входных данных к DUT
	void apply_input_to_dut(VerilatorWrapper<Top>& dut) {
		// 1. Устанавливаем 320-битные данные
		for (int i = 0; i < InOutData320::NUM_WORDS; i++) {
			if (i < 10) {  // Проверка границ
				dut.raw()->otl4_10_data_i[i] = input.get_otl4_10_data().get_word(i);
			}
		}
		
		// 2. Устанавливаем 8-битные данные
		dut.raw()->thoxidat_i = input.get_thoxidat().get_byte();
		
		// 3. Устанавливаем скалярные сигналы
		dut.raw()->scalar_signals_i = input.get_scalar_signals().packed;
	}
	
	// Вспомогательные высокоуровневые методы. 
	// Будет убраны после развертывания управляющих менеджеров

	// Чтение выходных данных из DUT
	void read_output_from_dut(VerilatorWrapper<Top>& dut) {
		received.read_from_dut(dut);
	}
	
	// Выполнение одного такта
	void execute_single_cycle(VerilatorWrapper<Top>& dut) {
		dut.tick();
		timestamp = dut.get_time();  // Обновляем временную метку
	}
	
	// Полное выполнение транзакции
	void run(VerilatorWrapper<Top>& dut) {
		try {
			// 1) Применяем входные данные
			apply_input_to_dut(dut);
			
			// 2) Выполняем один такт
			execute_single_cycle(dut);
			
			// 3) Читаем выходные данные
			read_output_from_dut(dut);
			
			// 4) Проверяем результат
			passed = check();
			
			if (!passed) {
				generate_failure_report();
			}
			
		} catch (const std::exception& e) {
			passed = false;
			failure_reason = "Exception during execution: " + std::string(e.what());
		}
	}
	
	// Многотактное выполнение (например, для пайплайнов)
	void run_multicycle(VerilatorWrapper<Top>& dut, int cycles = 1) {
		try {
			// 1) Применяем входные данные
			apply_input_to_dut(dut);
			
			// 2) Выполняем указанное количество тактов
			for (int i = 0; i < cycles; i++) {
				execute_single_cycle(dut);
			}
			
			// 3) Читаем выходные данные
			read_output_from_dut(dut);
			
			// 4) Проверяем результат
			passed = check();
			
			if (!passed) {
				generate_failure_report();
			}
			
		} catch (const std::exception& e) {
			passed = false;
			failure_reason = "Exception during multicycle execution: " + std::string(e.what());
		}
	}
#endif
	
	// --------------------------------------------------------------
	//  Проверка результата
	// --------------------------------------------------------------
	bool check() const noexcept {
		return received == expected;
	}
	
	// Детальная проверка с выводом различий
	bool check_detailed(std::ostream& os = std::cout) const {
		if (received == expected) {
			return true;
		}
		
		os << "\n=== DIFFERENCES FOUND ===" << std::endl;
		
		// Проверка 112-битных данных
		for (size_t i = 0; i < InOutData112::NUM_WORDS; ++i) {
			uint32_t exp_word = expected.get_data_out().get_word(i);
			uint32_t rec_word = received.get_data_out().get_word(i);
			
			if (exp_word != rec_word) {
				os << "  data_out word[" << i << "]: "
				   << "expected=0x" << std::hex << std::setw(8) << std::setfill('0') << exp_word
				   << " received=0x" << std::setw(8) << rec_word << std::dec << std::endl;
			}
		}
		
		// Проверка 8-битных данных
		if (expected.get_data_out_8bit().get_byte() != received.get_data_out_8bit().get_byte()) {
			os << "  data_out_8bit: "
			   << "expected=0x" << std::hex << std::setw(2) << std::setfill('0')
			   << (int)expected.get_data_out_8bit().get_byte()
			   << " received=0x" << std::setw(2)
			   << (int)received.get_data_out_8bit().get_byte() << std::dec << std::endl;
		}
		
		// Проверка скалярных сигналов
		if (expected.get_scalar_signals().packed != received.get_scalar_signals().packed) {
			os << "  scalar_signals: "
			   << "expected=0x" << std::hex << std::setw(2) << std::setfill('0')
			   << (int)expected.get_scalar_signals().packed
			   << " received=0x" << std::setw(2)
			   << (int)received.get_scalar_signals().packed << std::dec << std::endl;
			
			// Детализация по битам
			auto& exp_sig = expected.get_scalar_signals();
			auto& rec_sig = received.get_scalar_signals();
			
			if (exp_sig.get_DATA_VALID_OUT() != rec_sig.get_DATA_VALID_OUT())
				os << "    DATA_VALID_OUT: expected=" << exp_sig.get_DATA_VALID_OUT()
				   << " received=" << rec_sig.get_DATA_VALID_OUT() << std::endl;
			if (exp_sig.get_TXOHIFP_OUT() != rec_sig.get_TXOHIFP_OUT())
				os << "    TXOHIFP_OUT: expected=" << exp_sig.get_TXOHIFP_OUT()
				   << " received=" << rec_sig.get_TXOHIFP_OUT() << std::endl;
			if (exp_sig.get_TXOHIMFP_OUT() != rec_sig.get_TXOHIMFP_OUT())
				os << "    TXOHIMFP_OUT: expected=" << exp_sig.get_TXOHIMFP_OUT()
				   << " received=" << rec_sig.get_TXOHIMFP_OUT() << std::endl;
			if (exp_sig.get_FEC_SOF_OUT() != rec_sig.get_FEC_SOF_OUT())
				os << "    FEC_SOF_OUT: expected=" << exp_sig.get_FEC_SOF_OUT()
				   << " received=" << rec_sig.get_FEC_SOF_OUT() << std::endl;
			if (exp_sig.get_THOHEVALID() != rec_sig.get_THOHEVALID())
				os << "    THOHEVALID: expected=" << exp_sig.get_THOHEVALID()
				   << " received=" << rec_sig.get_THOHEVALID() << std::endl;
		}
		
		return false;
	}
	
	// --------------------------------------------------------------
	//  Генерация отчета об ошибке
	// --------------------------------------------------------------
	void generate_failure_report() {
		std::stringstream ss;
		check_detailed(ss);
		failure_reason = ss.str();
	}
	
	// --------------------------------------------------------------
	//  Методы вывода информации
	// --------------------------------------------------------------
	void print_input_summary(std::ostream& os = std::cout) const {
		os << "Input #" << test_vector_id;
		if (!test_name.empty()) {
			os << " [" << test_name << "]";
		}
		os << ": ";
		input.print_summary();
	}
	
	void print_result_summary(std::ostream& os = std::cout) const {
		os << "Test #" << test_vector_id << ": ";
		if (passed) {
			os << "PASS";
		} else {
			os << "FAIL";
		}
		
		if (!test_name.empty()) {
			os << " [" << test_name << "]";
		}
		
		if (timestamp > 0) {
			os << " at time " << timestamp;
		}
		os << std::endl;
	}
	
	void print_detailed_report(std::ostream& os = std::cout) const {
		os << "\n" << std::string(60, '=') << std::endl;
		os << "TEST VECTOR #" << test_vector_id << std::endl;
		if (!test_name.empty()) {
			os << "Test Name: " << test_name << std::endl;
		}
		if (timestamp > 0) {
			os << "Timestamp: " << timestamp << std::endl;
		}
		os << std::string(60, '-') << std::endl;
		
		// Входные данные
		os << "\nINPUT DATA:" << std::endl;
		input.print_summary();
		
		// Ожидаемые выходные данные
		os << "\nEXPECTED OUTPUT:" << std::endl;
		//
		//expected.print_summary();
		
		// Полученные выходные данные
		os << "\nRECEIVED OUTPUT:" << std::endl;
		//received.print_summary();
		
		// Результат
		os << "\nRESULT: ";
		if (passed) {
			os << "PASSED ✓" << std::endl;
		} else {
			os << "FAILED ✗" << std::endl;
			os << "\nFAILURE DETAILS:" << std::endl;
			os << failure_reason;
		}
		
		os << std::string(60, '=') << std::endl;
	}
	
	void print_full_details(std::ostream& os = std::cout) const {
		os << "\n" << std::string(80, '=') << std::endl;
		os << "FULL TRANSACTION DETAILS #" << test_vector_id << std::endl;
		if (!test_name.empty()) {
			os << "Test Name: " << test_name << std::endl;
		}
		os << std::string(80, '-') << std::endl;
		
		// Детальный вывод входных данных
		os << "\nINPUT DATA (42 bytes):" << std::endl;
		input.print_bytes();
		
		// Детальный вывод ожидаемых выходных данных
		os << "\nEXPECTED OUTPUT (16 bytes):" << std::endl;
		expected.print_bytes();
		
		// Детальный вывод полученных выходных данных
		os << "\nRECEIVED OUTPUT (16 bytes):" << std::endl;
		received.print_bytes();
		
		// Сравнение
		os << "\nCOMPARISON:" << std::endl;
		if (passed) {
			os << "All outputs match ✓" << std::endl;
		} else {
			os << "MISMATCHES FOUND ✗" << std::endl;
			check_detailed(os);
		}
		
		os << std::string(80, '=') << std::endl;
	}
	
	// --------------------------------------------------------------
	//  Утилиты
	// --------------------------------------------------------------
	void reset() {
		received = FramerTXOutputData();
		passed = false;
		failure_reason.clear();
		timestamp = 0;
	}
	
	void set_test_info(size_t id, const std::string& name = "") {
		test_vector_id = id;
		test_name = name;
	}
	
	// Сериализация результата транзакции
	std::string serialize_result() const {
		std::stringstream ss;
		ss << test_vector_id << ","
		   << (test_name.empty() ? "unnamed" : test_name) << ","
		   << (passed ? "PASS" : "FAIL") << ","
		   << timestamp;
		return ss.str();
	}
	
	// --------------------------------------------------------------
	//  Статические методы для создания тестовых транзакций
	// --------------------------------------------------------------
	static FramerTXTransaction<Top> create_reset_test(size_t id = 0) {
		FramerTXTransaction<Top> transaction(id, "Reset Test");
		transaction.input = FramerTXInputData::create_reset_sequence();
		transaction.expected = FramerTXOutputData::create_empty();
		return transaction;
	}
	
	static FramerTXTransaction<Top> create_normal_test(size_t id = 0) {
		FramerTXTransaction<Top> transaction(id, "Normal Operation Test");
		transaction.input = FramerTXInputData::create_test_data();
		transaction.expected = FramerTXOutputData::create_test_data();
		return transaction;
	}
	
	// --------------------------------------------------------------
	//  Операторы сравнения (для сортировки и поиска)
	// --------------------------------------------------------------
	bool operator<(const FramerTXTransaction& other) const {
		return test_vector_id < other.test_vector_id;
	}
	
	bool operator==(const FramerTXTransaction& other) const {
		return test_vector_id == other.test_vector_id &&
			   input == other.input &&
			   expected == other.expected &&
			   received == other.received;
	}
	
	// --------------------------------------------------------------
	//  Оператор вывода
	// --------------------------------------------------------------
	friend std::ostream& operator<<(std::ostream& os, const FramerTXTransaction<Top>& t) {
		os << "Transaction #" << t.test_vector_id;
		if (!t.test_name.empty()) {
			os << " \"" << t.test_name << "\"";
		}
		os << ": " << (t.passed ? "PASS" : "FAIL");
		return os;
	}
};
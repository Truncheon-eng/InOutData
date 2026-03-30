//InOutFlagStruct.hpp
#include <iostream>
#include <iomanip>
#include <array>
#include <cstdint>
#include <string>

using std::cout;
using std::endl;
using std::setw;
using std::endl;

// -----------------------------------------------------------------------------
// 4‑BIT data input structure
// -----------------------------------------------------------------------------
struct InOutSignals4 {
	static constexpr int width      = 4;
	static constexpr int num_words  = 1;
	static_assert(width <= 8, "4‑bit structure must fit into a single byte");

	union {
		struct {
			uint8_t THOHIVLD        : 1;
			uint8_t OTL4_10_DATA_VALID : 1;
			uint8_t RATE_CONTROL    : 1;
			uint8_t nHARD_RESET     : 1;
			uint8_t RESERVED        : 4;   // оставшиеся биты
		};
		uint8_t packed;
		uint8_t bytes[1];
		uint32_t words[1];                // для совместимости с Verilator
	};

	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	InOutSignals4() : packed(0) {
		nHARD_RESET = 1;                  // по умолчанию сброс неактивен
	}

	InOutSignals4(bool thohivld,
				  bool otl4_10_data_valid,
				  bool rate_control,
				  bool nhard_reset) : packed(0)
	{
		set_all(thohivld, otl4_10_data_valid, rate_control, nhard_reset);
	}

	// -------------------------------------------------
	// Геттеры (читают отдельный бит)
	// -------------------------------------------------
	bool get_THOHIVLD()        const { return THOHIVLD; }
	bool get_OTL4_10_DATA_VALID() const { return OTL4_10_DATA_VALID; }
	bool get_RATE_CONTROL()    const { return RATE_CONTROL; }
	bool get_nHARD_RESET()     const { return nHARD_RESET; }

	// -------------------------------------------------
	// Сеттеры (устанавливают отдельный бит)
	// -------------------------------------------------
	void set_THOHIVLD(bool v)        { THOHIVLD        = v ? 1 : 0; }
	void set_OTL4_10_DATA_VALID(bool v) { OTL4_10_DATA_VALID = v ? 1 : 0; }
	void set_RATE_CONTROL(bool v)    { RATE_CONTROL    = v ? 1 : 0; }
	void set_nHARD_RESET(bool v)     { nHARD_RESET     = v ? 1 : 0; }

	// -------------------------------------------------
	// Установка сразу всех битов
	// -------------------------------------------------
	void set_all(bool thohivld,
				 bool otl4_10_data_valid,
				 bool rate_control,
				 bool nhard_reset)
	{
		THOHIVLD            = thohivld            ? 1 : 0;
		OTL4_10_DATA_VALID = otl4_10_data_valid ? 1 : 0;
		RATE_CONTROL       = rate_control       ? 1 : 0;
		nHARD_RESET        = nhard_reset        ? 1 : 0;
	}

	// -------------------------------------------------
	// Специальные режимы
	// -------------------------------------------------
	void set_reset_sequence() {
		packed = 0;
		OTL4_10_DATA_VALID = 0;   // активный‑низкий сброс
	}

	void set_normal_operation() {
		THOHIVLD        = 1;
		OTL4_10_DATA_VALID = 1;
		RATE_CONTROL    = 1;
		nHARD_RESET     = 0;
	}

	// -------------------------------------------------
	// Оператор [] – совместимость с Verilator
	// -------------------------------------------------
	uint32_t operator[](int index) const {
		return (index == 0) ? static_cast<uint32_t>(packed) : 0u;
	}

	uint32_t& operator[](int index) {
		static uint32_t dummy = 0;
		if (index == 0) {
			// Приводим packed к 32‑битному виду, но сохраняем ссылку
			static uint32_t converted = 0;
			converted = static_cast<uint32_t>(packed);
			return converted;
		}
		return dummy;
	}

	// -------------------------------------------------
	// Вывод для отладки
	// -------------------------------------------------
	void print(const char* name = "") const {
		std::cout << name << " Input Signals (4 bits):" << std::endl;
		std::cout << "  THOHIVLD:          " << static_cast<int>(THOHIVLD) << std::endl;
		std::cout << "  OTL4_10_DATA_VALID:" << static_cast<int>(OTL4_10_DATA_VALID) << std::endl;
		std::cout << "  RATE_CONTROL:      " << static_cast<int>(RATE_CONTROL) << std::endl;
		std::cout << "  nHARD_RESET:       " << static_cast<int>(nHARD_RESET) << std::endl;
		std::cout << "  packed: 0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(packed) << std::dec << std::endl;
		std::cout << "  binary: "
				  << (nHARD_RESET ? "1" : "0") << "_"
				  << (RATE_CONTROL ? "1" : "0") << "_"
				  << (OTL4_10_DATA_VALID ? "1" : "0") << "_"
				  << (THOHIVLD ? "1" : "0") << std::endl;
	}

	// -------------------------------------------------
	// Приведение к типу Verilator
	// -------------------------------------------------
#ifdef VERILATOR
//TODO: Переделать под тип WLVIDE_8
	operator VlWide<1>() const {
		VlWide<1> result;
		result[0] = static_cast<uint32_t>(packed);
		return result;
	}
#endif
};

// -----------------------------------------------------------------------------
// 5‑BIT data output structure
// -----------------------------------------------------------------------------
struct InOutSignals5 {
	static constexpr int width      = 5;
	static constexpr int num_words  = 1;
	static_assert(width <= 8, "5‑bit structure must fit into a single byte");

	union {
		struct {
			uint8_t DATA_VALID_OUT : 1;
			uint8_t TXOHIFP_OUT    : 1;
			uint8_t TXOHIMFP_OUT   : 1;
			uint8_t FEC_SOF_OUT    : 1;
			uint8_t THOHEVALID     : 1;
			uint8_t RESERVED       : 3;   // Выравнивание
		};
		uint8_t packed;
		uint8_t bytes[1];
		uint32_t words[1];                // Унификация с существующим кодом
	};

	// -------------------------------------------------
	// Конструкторы
	// -------------------------------------------------
	InOutSignals5() : packed(0) {}

	InOutSignals5(bool data_valid_out,
				  bool txohifp_out,
				  bool txohimfp_out,
				  bool fec_sof_out,
				  bool thohevalid) : packed(0){
		set_all(data_valid_out, 
				txohifp_out, 
				txohimfp_out, 
				fec_sof_out, thohevalid);
	}

	// -------------------------------------------------
	// Геттеры
	// -------------------------------------------------
	bool get_DATA_VALID_OUT() const { 
		return DATA_VALID_OUT; 
	}
	
	bool get_TXOHIFP_OUT()    const { 
		return TXOHIFP_OUT; 
	}
	
	bool get_TXOHIMFP_OUT()   const { 
		return TXOHIMFP_OUT; 
	}

	bool get_FEC_SOF_OUT()    const { 
		return FEC_SOF_OUT; 
	}
	
	bool get_THOHEVALID()     const { 
		return THOHEVALID; 
	}
};
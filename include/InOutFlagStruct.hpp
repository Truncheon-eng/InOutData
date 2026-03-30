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
};
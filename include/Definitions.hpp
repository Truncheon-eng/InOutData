//Definitions.hpp
// Константы для смещений в заголовке
#pragma once
#define HEADER_SIGNATURE_OFFSET				0
#define HEADER_MODULE_ID_OFFSET 			4
#define HEADER_TOTAL_DATA_LENGTH_OFFSET 	8
#define HEADER_INPUT_BASE_ADDR_OFFSET  		12
#define HEADER_INPUT_VECTOR_SIZE_OFFSET 	16
#define HEADER_OUTPUT_BASE_ADDR_OFFSET 		20
#define HEADER_OUTPUT_VECTOR_SIZE_OFFSET 	24
#define HEADER_RESERVED_OFFSET 				28
#define HEADER_SIGNALS_INFO_OFFSET 			32
#define HEADER_SIGNAL_WIDTHS_OFFSET 		34

// Константы для битовых операций
#define BYTE_0_MASK 0x000000FF  // Младший байт (биты 0-7)
#define BYTE_1_MASK 0x0000FF00  // Байт 1 (биты 8-15)  
#define BYTE_2_MASK 0x00FF0000  // Байт 2 (биты 16-23)
#define BYTE_3_MASK 0xFF000000  // Старший байт (биты 24-31)

#define SHIFT_BYTE_0 0          // Сдвиг для байта 0
#define SHIFT_BYTE_1 8          // Сдвиг для байта 1
#define SHIFT_BYTE_2 16         // Сдвиг для байта 2
#define SHIFT_BYTE_3 24         // Сдвиг для байта 3

// Адрес заголовка в Control Regs
#define HEADER_BASE_ADDR    0x0C000000
#define DEBUG_BUS_BASE_ADDR 0x0D00_0000

// Максимальные количества для безопасности
#define MAX_INPUT_SIGNALS  16
#define MAX_OUTPUT_SIGNALS 16

//TODO: Как выйдем из горячей фазы - поменять на осознанное
#define TEST_VECTOR_TEST	0x54455354		// "TEST"

// Идентификаторы подсистем
#define FRAMER_PCS_TX 		    0x46525458      // FRTX
#define FRAMER_PCS_RX 		    0x46525052      // FRPR 
#define FRAMER_PCS 		        0x46525000      // FRPC //TODO:Поправить
#define DSP_RX 				    0x44535258      // DSRX
#define DSP_TX 				    0x44535458      // DSTX 

// Идентификаторы модулей
#define FULL					0x46554C4C		// FULL - вся подсистема в сборе
#define OTU4_CONVERTER         	0x4F545534 		// OTU4
#define DESCRAMBLER            	0x44534352 		// DSCR
#define FEC_EXTRACTOR          	0x46585452 		// FXTR 
#define AIS_FRAME_GENERATOR_TX 	0x41464754 		// AFGT
#define FRAMER_STAT            	0x53544154 		// STAT
#define HEADER_PROCESSOR       	0x48455052 		// HEPR
#define HEADERS_MANAGER_TX     	0x484D5458 		// HMTX
#define OTUC1_PADDER           	0x4F545044 		// OTPD
#define SCRAMBLER              	0x5343524D 		// SCRM 
#define OUTPUT_BUFFER          	0x4F424652 		// OBFR 

#define C0FEEF00D               0xC0FEF00D

#define DATA_WIDTH_320
#define DATA_WIDTH_128
#define DATA_WIDTH_112
#define DATA_WIDTH_8

// Цветовые коды

#define NORMAL  "\033[0m"

#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define CHECK_MARK "[+] "   // Check
#define CROSS_MARK "[-] "   // Error
#define WARNING_MARK "[!] " // Warning
#define INFO_MARK "[i] "    // Info
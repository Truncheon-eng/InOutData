#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <string>

#include "InOutFramerTXDataStruct.hpp"
#include "Definitions.hpp"

using std::cout;
using std::endl;
using std::endl;
using std::string;

// Функция для вывода помощи
void print_usage(const char* program_name) {
	cout << "Usage: " << program_name << " <hex_file> [options]" << endl;
	cout << "\nOptions:" << endl;
	cout << "  -v, --verbose     Enable verbose output" << endl;
	cout << "  -c, --checksum    Enable checksum verification" << endl;
	cout << "  -e, --errors      Show parser errors in detail" << endl;
	cout << "  -w, --warnings    Show parser warnings in detail" << endl;
	cout << "  -a, --all         Show all parser messages" << endl;
	cout << "  -s, --stats       Show only statistics" << endl;
	cout << "  -r, --raw         Show raw parser messages (for debugging)" << endl;
	cout << "  -h, --help        Show this help message" << endl;
}

// Функция для обработки аргументов командной строки
struct ProgramOptions {
	string hex_file;
	bool verbose = false;
	bool show_errors = true;
	bool show_warnings = true;
	bool show_all_messages = false;
	bool show_only_stats = false;
	bool show_raw_messages = false;
	bool enable_checksum = true;
};

ProgramOptions parse_arguments(int argc, char** argv) {
	ProgramOptions options;
	
	if (argc < 2) {
		print_usage(argv[0]);
		exit(1);
	}
	
	options.hex_file = argv[1];
	
	for (int i = 2; i < argc; i++) {
		string arg = argv[i];
		
		if (arg == "-v" || arg == "--verbose") {
			options.verbose = true;
		} else if (arg == "-c" || arg == "--checksum") {
			options.enable_checksum = true;
		} else if (arg == "-e" || arg == "--errors") {
			options.show_errors = true;
			options.show_warnings = false;
		} else if (arg == "-w" || arg == "--warnings") {
			options.show_errors = false;
			options.show_warnings = true;
		} else if (arg == "-a" || arg == "--all") {
			options.show_all_messages = true;
		} else if (arg == "-s" || arg == "--stats") {
			options.show_only_stats = true;
			options.show_errors = false;
			options.show_warnings = false;
		} else if (arg == "-r" || arg == "--raw") {
			options.show_raw_messages = true;
		} else if (arg == "-h" || arg == "--help") {
			print_usage(argv[0]);
			exit(0);
		} else {
			cerr << "Unknown option: " << arg << endl;
			print_usage(argv[0]);
			exit(1);
		}
	}
	
	return options;
}

void test_hex_conversion() {
	std::cout << "\n=== Testing Hex Conversion ===\n";
	
	// Test 1: Big-endian hex string
	std::string hex_be = "0x00112233445566778899AABBCCDDEEFF";
	InOutData128 data_be = InOutData128::from_hex_string(hex_be);
	
	std::cout << "\nInput (big-endian hex): " << hex_be << std::endl;
	data_be.print("Result in memory (little-endian)");
	data_be.print_bytes("Bytes");
	
	std::cout << "\nExpected words (LE):" << std::endl;
	std::cout << "  word0: 0xFFEEDDCC (bytes: FF EE DD CC)" << std::endl;
	std::cout << "  word1: 0xBBAA9988 (bytes: BB AA 99 88)" << std::endl;
	std::cout << "  word2: 0x77665544 (bytes: 77 66 55 44)" << std::endl;
	std::cout << "  word3: 0x33221100 (bytes: 33 22 11 00)" << std::endl;
	
	// Test 2: Little-endian hex string (редко используется)
	std::string hex_le = "0xFFEEDDCCBBAA99887766554433221100";
	InOutData128 data_le = InOutData128::from_hex_string_le(hex_le);
	
	std::cout << "\n\nInput (little-endian hex): " << hex_le << std::endl;
	data_le.print("Result in memory (little-endian)");
	
	std::cout << "\nBoth should give same result in memory!" << std::endl;
	
	// Проверка
	if (data_be == data_le) {
		std::cout << "\n✓ Conversion correct!" << std::endl;
	} else {
		std::cout << "\n✗ Conversion failed!" << std::endl;
	}
}

void test_correct_hex_conversion() {
	std::cout << "\n=== Testing Correct Hex Conversion ===\n";
	
	// Hex строка: 00112233445566778899AABBCCDDEEFF
	// Big-endian представление: 00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF
	// Мы хотим в памяти (LE):   FF EE DD CC BB AA 99 88 77 66 55 44 33 22 11 00
	//                          word0    word1    word2    word3
	
	std::string hex_str = "0x00112233445566778899AABBCCDDEEFF";
	InOutData128 data = InOutData128::from_hex_string(hex_str);
	
	std::cout << "\nInput hex: " << hex_str << std::endl;
	
	// Проверяем байты
	std::cout << "\nExpected bytes in memory (LE):" << std::endl;
	std::cout << "  word0: FF EE DD CC" << std::endl;
	std::cout << "  word1: BB AA 99 88" << std::endl;
	std::cout << "  word2: 77 66 55 44" << std::endl;
	std::cout << "  word3: 33 22 11 00" << std::endl;
	
	std::cout << "\nActual bytes in memory:" << std::endl;
	data.print_bytes("Data");
	
	// Проверяем слова
	std::cout << "\nExpected words:" << std::endl;
	std::cout << "  word0: 0xFFEEDDCC" << std::endl;
	std::cout << "  word1: 0xBBAA9988" << std::endl;
	std::cout << "  word2: 0x77665544" << std::endl;
	std::cout << "  word3: 0x33221100" << std::endl;
	
	std::cout << "\nActual words:" << std::endl;
	data.print("Data");
	
	// Проверка
	bool correct = true;
	correct &= (data.words[0] == 0xFFEEDDCC);
	correct &= (data.words[1] == 0xBBAA9988);
	correct &= (data.words[2] == 0x77665544);
	correct &= (data.words[3] == 0x33221100);
	
	if (correct) {
		std::cout << "\n✓ Conversion SUCCESS!" << std::endl;
	} else {
		std::cout << "\n✗ Conversion FAILED!" << std::endl;
		std::cout << "  word0: got 0x" << std::hex << data.words[0] 
				  << ", expected 0xFFEEDDCC" << std::endl;
		std::cout << "  word1: got 0x" << data.words[1] 
				  << ", expected 0xBBAA9988" << std::endl;
		std::cout << "  word2: got 0x" << data.words[2] 
				  << ", expected 0x77665544" << std::endl;
		std::cout << "  word3: got 0x" << data.words[3] 
				  << ", expected 0x33221100" << std::dec << std::endl;
	}
}

// В main.cpp или отдельном тестовом файле
void test_endian_methods() {
	std::cout << "\n=== Testing Endian-Specific Methods ===\n";
	
	// Тест 1: Little-endian создание
	std::cout << "\n1. Creating with little-endian words:" << std::endl;
	InOutData128 data_le = InOutData128::from_words_little_endian(
		0xAABBCCDD, 0xEEFF0011, 0x22334455, 0x66778899);
	
	data_le.print("Data (created as LE)");
	data_le.print_bytes("Bytes in memory");
	
	// Проверяем get методы
	std::cout << "\nUsing get_word_little_endian():" << std::endl;
	for (int i = 0; i < 4; i++) {
		std::cout << "  word[" << i << "] = 0x" << std::hex 
				  << std::setw(8) << std::setfill('0')
				  << data_le.get_word_little_endian(i) << std::dec << std::endl;
	}
	
	// Тест 2: Big-endian создание (должно дать другой результат)
	std::cout << "\n\n2. Creating with big-endian words:" << std::endl;
	std::cout << "   (Same values but interpreted as BE)" << std::endl;
	
	InOutData128 data_be = InOutData128::from_words_big_endian(
		0xAABBCCDD, 0xEEFF0011, 0x22334455, 0x66778899);
	
	data_be.print("Data (created as BE)");
	data_be.print_bytes("Bytes in memory");
	
	// Тест 3: Из hex строки
	std::cout << "\n\n3. From hex string (big-endian):" << std::endl;
	std::string hex_str = "0x00112233445566778899AABBCCDDEEFF";
	InOutData128 data_hex = InOutData128::from_hex_string(hex_str);
	
	std::cout << "Hex input: " << hex_str << std::endl;
	data_hex.print("Result");
	data_hex.print_bytes("Bytes");
	
	// Проверяем что получилось
	std::cout << "\nExpected for hex string:" << std::endl;
	std::cout << "  word0 (LE in memory) = 0xFFEEDDCC" << std::endl;
	std::cout << "  word1 (LE in memory) = 0xBBAA9988" << std::endl;
	std::cout << "  word2 (LE in memory) = 0x77665544" << std::endl;
	std::cout << "  word3 (LE in memory) = 0x33221100" << std::endl;
	
	std::cout << "\nActual:" << std::endl;
	std::cout << "  get_word_little_endian(0) = 0x" << std::hex 
			  << data_hex.get_word_little_endian(0) << std::endl;
	std::cout << "  get_word_little_endian(1) = 0x" 
			  << data_hex.get_word_little_endian(1) << std::endl;
	std::cout << "  get_word_little_endian(2) = 0x" 
			  << data_hex.get_word_little_endian(2) << std::endl;
	std::cout << "  get_word_little_endian(3) = 0x" 
			  << data_hex.get_word_little_endian(3) << std::dec << std::endl;
}

void test_set_endian_methods() {
	std::cout << "\n=== Testing Set Endian Methods ===\n";
	
	InOutData128 data;
	
	// Устанавливаем как little-endian
	std::cout << "\nSetting as little-endian:" << std::endl;
	data.set_word_little_endian(0, 0x12345678);
	data.set_word_little_endian(1, 0x9ABCDEF0);
	data.set_word_little_endian(2, 0x11223344);
	data.set_word_little_endian(3, 0x55667788);
	
	data.print_bytes("After set_word_little_endian");
	
	std::cout << "\nGetting as little-endian:" << std::endl;
	for (int i = 0; i < 4; i++) {
		std::cout << "  word[" << i << "] = 0x" << std::hex 
				  << std::setw(8) << std::setfill('0')
				  << data.get_word_little_endian(i) << std::dec << std::endl;
	}
	
	// Очищаем и устанавливаем как big-endian
	data.clear();
	
	std::cout << "\n\nSetting as big-endian:" << std::endl;
	std::cout << "(Same values but interpreted as BE)" << std::endl;
	
	data.set_word_big_endian(0, 0x12345678);
	data.set_word_big_endian(1, 0x9ABCDEF0);
	data.set_word_big_endian(2, 0x11223344);
	data.set_word_big_endian(3, 0x55667788);
	
	data.print_bytes("After set_word_big_endian");
	
	std::cout << "\nGetting as big-endian:" << std::endl;
	for (int i = 0; i < 4; i++) {
		std::cout << "  word[" << i << "] = 0x" << std::hex 
				  << std::setw(8) << std::setfill('0')
				  << data.get_word_big_endian(i) << std::dec << std::endl;
	}
}

void test_vector_methods() {
	std::cout << "\n=== Testing Vector Methods ===\n";
	
	// Создаем тестовый вектор (big-endian представление в голове)
	// В памяти хотим: FF EE DD CC BB AA 99 88 77 66 55 44 33 22 11 00
	// Это little-endian для hex строки: 0x00112233445566778899AABBCCDDEEFF
	
	std::vector<uint8_t> vec_be = {
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
	};
	
	std::vector<uint8_t> vec_le = {
		0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
		0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
	};
	
	// Тест 1: Из big-endian вектора
	std::cout << "\n1. From big-endian vector:" << std::endl;
	InOutData128 data_be = InOutData128::from_vector_be(vec_be, 0x00);
	data_be.print_detailed("from_vector_big_endian");
	
	// Проверяем что получилось то же что из hex строки
	InOutData128 data_hex = InOutData128::from_hex_string("0x00112233445566778899AABBCCDDEEFF");
	if (data_be == data_hex) {
		std::cout << "✓ Matches from_hex_string()" << std::endl;
	} else {
		std::cout << "✗ Does NOT match from_hex_string()" << std::endl;
	}
	
	// Unified functions

	std::cout << "\n1.1 From big-endian vector:" << std::endl;
	InOutData128 data_be_un;
	data_be_un.from_vector(vec_be, 0x00, InOutData128::InitOrder::BIG_ENDIAN_IN).print_detailed("from_vector_big_endian_unified");

	// Тест 2: Из little-endian вектора
	std::cout << "\n\n2. From little-endian vector:" << std::endl;
	InOutData128 data_le = InOutData128::from_vector_le(vec_le);
	//data_le.print_detailed("from_vector_little_endian");
	
	if (data_le == data_be) {
		std::cout << "✓ Both methods give same result" << std::endl;
	} else {
		std::cout << "✗ Methods give different results" << std::endl;
	}
	
	// Тест 3: Конвертация обратно в векторы
	std::cout << "\n\n3. Converting back to vectors:" << std::endl;
	
	std::vector<uint8_t> back_le = data_be.to_vector_little_endian();
	std::cout << "to_vector_little_endian(): ";
	for (uint8_t b : back_le) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') 
				  << static_cast<int>(b) << " ";
	}
	std::cout << std::dec << std::endl;
	
	std::vector<uint8_t> back_be = data_be.to_vector_big_endian();
	std::cout << "to_vector_big_endian():    ";
	for (uint8_t b : back_be) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') 
				  << static_cast<int>(b) << " ";
	}
	std::cout << std::dec << std::endl;
	
	// Тест 4: С offset
	std::cout << "\n\n4. With offset (starting from position 4):" << std::endl;
	
	std::vector<uint8_t> vec_with_offset = {
		0xAA, 0xBB, 0xCC, 0xDD,  // пропускаем эти
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
	};
	
	InOutData128 data_offset = InOutData128::from_vector_be(vec_with_offset, 4);
	data_offset.print_detailed("with offset 4");
}

void demonstrate_practical_use() {
	std::cout << "\n=== Practical Use Case ===" << std::endl;
	
	// Симуляция вашего кода парсинга
	// Предположим, что у вас есть сырые данные
	std::vector<uint8_t> raw_data = {
		// 320-битные данные (40 байт)
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		
		// 128-битные данные (16 байт) - LITTLE-ENDIAN!
		0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
		0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
		
		// Другие данные...
		0xAA, 0xBB, 0xCC, 0xDD
	};
	
	std::cout << "\nParsing 128-bit data from raw vector (offset 40):" << std::endl;
	
	// Ваш случай: данные уже в little-endian в векторе
	InOutData128 parsed_data = InOutData128::from_vector_be(raw_data, 40);
	parsed_data.print_detailed("Parsed 128-bit data");
	
	// Проверяем что получилось
	std::cout << "\nExpected (little-endian in memory):" << std::endl;
	std::cout << "  Bytes: FF EE DD CC BB AA 99 88 77 66 55 44 33 22 11 00" << std::endl;
	std::cout << "  Word0: 0xCCDDEEFF" << std::endl;
	std::cout << "  Word1: 0x8899AABB" << std::endl;
	std::cout << "  Word2: 0x44556677" << std::endl;
	std::cout << "  Word3: 0x00112233" << std::endl;
}

void test_hex_output() {
	std::cout << "\n=== Testing Hex Output Formats ===" << std::endl;
	
	// Создаем данные: в памяти FF EE DD CC BB AA 99 88 77 66 55 44 33 22 11 00
	std::vector<uint8_t> vec_le = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
									0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
	InOutData128 data(vec_le);
	
	std::cout << "\nTest data in memory: ";
	for (int i = 0; i < 16; i++) {
		std::cout << "0x" << std::hex << std::setw(2) 
				  << static_cast<int>(data.bytes[i]) << " ";
	}
	std::cout << std::dec << std::endl;
	
	std::cout << "\nHex representations:" << std::endl;
	std::cout << "  Big-endian:    " << data.to_hex_string(InOutData128::OutputFormat::HEX_BIG_ENDIAN) << std::endl;
	std::cout << "  Expected BE:   0x00112233445566778899aabbccddeeff" << std::endl;
	
	std::cout << "  Little-endian: " << data.to_hex_string(InOutData128::OutputFormat::HEX_LITTLE_ENDIAN) << std::endl;
	std::cout << "  Expected LE:   0xffeeddccbbaa99887766554433221100" << std::endl;
	
	// Проверка
	std::string be_str = data.to_hex_string(InOutData128::OutputFormat::HEX_BIG_ENDIAN);
	std::string le_str = data.to_hex_string(InOutData128::OutputFormat::HEX_LITTLE_ENDIAN);
	
	if (be_str == "0x00112233445566778899aabbccddeeff" && 
		le_str == "0xffeeddccbbaa99887766554433221100") {
		std::cout << "\n✓ All hex formats correct!" << std::endl;
	} else {
		std::cout << "\n✗ Hex formats incorrect!" << std::endl;
	}

	data.print_bytes("Shit print!\n");

}

void test_inoutdata320() {
	std::cout << "\n=========================================" << std::endl;
	std::cout << "=== TESTING InOutData320 STRUCTURE ===" << std::endl;
	std::cout << "=========================================\n" << std::endl;
	
	// Тест 1: Конструкторы
	std::cout << "1. Testing Constructors:" << std::endl;
	std::cout << "------------------------" << std::endl;
	
	// Конструктор по умолчанию
	InOutData320 data1;
	std::cout << "Default constructor: ";
	data1.print_raw_memory("data1 (all zeros)");
	
	// Конструктор из отдельных слов
	InOutData320 data2(0x12345678, 0x9ABCDEF0, 0x11223344, 0x55667788,
					   0xAABBCCDD, 0xEEFF0011, 0x22334455, 0x66778899,
					   0x8899AABB, 0xCCDDEEFF);
	std::cout << "\nFrom words constructor:" << std::endl;
	data2.print("data2");
	data2.print_bytes("data2 bytes");
	
	// Конструктор из массива байт (little-endian)
	std::vector<uint8_t> vec_le = {
		// word0: 0x12345678 -> байты: 0x78 0x56 0x34 0x12
		0x78, 0x56, 0x34, 0x12,
		// word1: 0x9ABCDEF0 -> байты: 0xF0 0xDE 0xBC 0x9A
		0xF0, 0xDE, 0xBC, 0x9A,
		// word2: 0x11223344 -> байты: 0x44 0x33 0x22 0x11
		0x44, 0x33, 0x22, 0x11,
		// word3: 0x55667788 -> байты: 0x88 0x77 0x66 0x55
		0x88, 0x77, 0x66, 0x55,
		// word4: 0xAABBCCDD -> байты: 0xDD 0xCC 0xBB 0xAA
		0xDD, 0xCC, 0xBB, 0xAA,
		// word5: 0xEEFF0011 -> байты: 0x11 0x00 0xFF 0xEE
		0x11, 0x00, 0xFF, 0xEE,
		// word6: 0x22334455 -> байты: 0x55 0x44 0x33 0x22
		0x55, 0x44, 0x33, 0x22,
		// word7: 0x66778899 -> байты: 0x99 0x88 0x77 0x66
		0x99, 0x88, 0x77, 0x66,
		// word8: 0x8899AABB -> байты: 0xBB 0xAA 0x99 0x88
		0xBB, 0xAA, 0x99, 0x88,
		// word9: 0xCCDDEEFF -> байты: 0xFF 0xEE 0xDD 0xCC
		0xFF, 0xEE, 0xDD, 0xCC
	};
	
	InOutData320 data3(vec_le);
	std::cout << "\nFrom vector (little-endian):" << std::endl;
	data3.print("data3");
	
	// Проверка равенства
	if (data2 == data3) {
		std::cout << "✓ data2 == data3 (identical in memory)" << std::endl;
	} else {
		std::cout << "✗ data2 != data3 (different in memory)" << std::endl;
	}
	
	// Тест 2: Методы доступа
	std::cout << "\n\n2. Testing Access Methods:" << std::endl;
	std::cout << "---------------------------" << std::endl;
	
	std::cout << "Testing get_word():" << std::endl;
	for (int i = 0; i < 10; i++) {
		std::cout << "  word[" << i << "] = 0x" << std::hex << std::setw(8) 
				  << std::setfill('0') << data2.get_word(i) << std::dec;
		if (i % 2 == 1) std::cout << std::endl;
		else std::cout << "  ";
	}
	
	std::cout << "\nTesting operator[]:" << std::endl;
	for (int i = 0; i < 10; i++) {
		std::cout << "  data2[" << i << "] = 0x" << std::hex << std::setw(8) 
				  << std::setfill('0') << data2[i] << std::dec;
		if (i % 2 == 1) std::cout << std::endl;
		else std::cout << "  ";
	}
	
	// Тест 3: Endian-specific методы
	std::cout << "\n\n3. Testing Endian-Specific Methods:" << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	
	std::cout << "Little-endian access:" << std::endl;
	std::cout << "  word0 (LE) = 0x" << std::hex << std::setw(8) 
			  << data2.get_word_little_endian(0) << std::dec << std::endl;
	std::cout << "  word9 (LE) = 0x" << std::hex << std::setw(8) 
			  << data2.get_word_little_endian(9) << std::dec << std::endl;
	
	std::cout << "\nBig-endian access (converted):" << std::endl;
	std::cout << "  word0 (BE) = 0x" << std::hex << std::setw(8) 
			  << data2.get_word_big_endian(0) << std::dec << std::endl;
	std::cout << "  word9 (BE) = 0x" << std::hex << std::setw(8) 
			  << data2.get_word_big_endian(9) << std::dec << std::endl;
	
	// Тест 4: Работа с векторами
	std::cout << "\n\n4. Testing Vector Methods:" << std::endl;
	std::cout << "--------------------------" << std::endl;
	
	// Конвертация в вектор
	std::vector<uint8_t> vec_back = data2.to_vector();
	std::cout << "Converted back to vector (first 8 bytes): ";
	for (size_t i = 0; i < std::min(vec_back.size(), (size_t)8); i++) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(vec_back[i]) << " ";
	}
	std::cout << "..." << std::dec << std::endl;
	
	// Тест 5: Hex строки
	std::cout << "\n\n5. Testing Hex String Conversion:" << std::endl;
	std::cout << "---------------------------------" << std::endl;
	
	// Создаем простые данные для теста
	InOutData320 simple_data = InOutData320::from_words(
		0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F,
		0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F,
		0x20212223, 0x24252627
	);
	
	std::cout << "Simple data for hex test:" << std::endl;
	simple_data.print_raw_memory("simple_data");
	
	std::cout << "\nHex representations:" << std::endl;
	std::cout << "  Big-endian:    " << simple_data.to_hex_string(InOutData320::OutputFormat::HEX_BIG_ENDIAN) << std::endl;
	std::cout << "  Little-endian: " << simple_data.to_hex_string(InOutData320::OutputFormat::HEX_LITTLE_ENDIAN) << std::endl;
	
	// Тест 6: Статические фабричные методы
	// std::cout << "\n\n6. Testing Static Factory Methods:" << std::endl;
	// std::cout << "-----------------------------------" << std::endl;
	
	// // Из hex строки
	// std::string hex_str = "0x000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627";
	// InOutData320 from_hex = InOutData320::from_hex_string_le(hex_str);
	
	// std::cout << "From hex string (big-endian):" << std::endl;
	// std::cout << "  Input: " << hex_str.substr(0, 40) << "..." << std::endl;
	// from_hex.print_raw_memory("from_hex (first 16 bytes)");
	
	// // Проверка
	// if (from_hex == simple_data) {
	// 	std::cout << GREEN << CHECK_MARK << "Hex conversion correct!" << NORMAL << std::endl;
	// } else {
	// 	std::cout << RED << CROSS_MARK << " Hex conversion incorrect!" << NORMAL << std::endl;
	// }
	
	// Тест 6: Статические фабричные методы
	// std::cout << "\n\n6. Testing Static Factory Methods:" << std::endl;
	// std::cout << "-----------------------------------" << std::endl;

	// // Из hex строки
	// std::string hex_str = "0x000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627";

	// // Тестируем ОБА метода
	// InOutData320 from_hex_be = InOutData320::from_hex_string(hex_str);  // предполагает big-endian
	// InOutData320 from_hex_le = InOutData320::from_hex_string_le(hex_str);  // предполагает little-endian

	// std::cout << "Hex string: " << hex_str.substr(0, 40) << "..." << std::endl;
	// std::cout << "\nUsing from_hex_string (assumes BE hex):" << std::endl;
	// from_hex_be.print_raw_memory("BE result");

	// std::cout << "\nUsing from_hex_string_le (assumes LE hex):" << std::endl;
	// from_hex_le.print_raw_memory("LE result");

	// std::cout << "\nsimple_data (expected):" << std::endl;
	// simple_data.print_raw_memory("expected");

	// // Побайтовое сравнение
	// std::cout << "\nByte-by-byte comparison:" << std::endl;
	// std::cout << "Index | BE Result | LE Result | Expected | BE Match | LE Match" << std::endl;
	// std::cout << "------|-----------|-----------|----------|----------|----------" << std::endl;

	// for (int i = 0; i < 40; i++) {
	// 	uint8_t be_byte = from_hex_be.get_byte(i);
	// 	uint8_t le_byte = from_hex_le.get_byte(i);
	// 	uint8_t expected_byte = simple_data.get_byte(i);
		
	// 	std::cout << std::setw(4) << i << " | "
	// 			<< "0x" << std::hex << std::setw(2) << std::setfill('0') 
	// 			<< static_cast<int>(be_byte) << std::dec << "      | "
	// 			<< "0x" << std::hex << std::setw(2) << std::setfill('0') 
	// 			<< static_cast<int>(le_byte) << std::dec << "      | "
	// 			<< "0x" << std::hex << std::setw(2) << std::setfill('0') 
	// 			<< static_cast<int>(expected_byte) << std::dec << "   | "
	// 			<< (be_byte == expected_byte ? "   ✓   " : "   ✗   ") << "| "
	// 			<< (le_byte == expected_byte ? "   ✓   " : "   ✗   ") << std::endl;
	// }

	// // Проверка
	// if (from_hex_be == simple_data) {
	// 	std::cout << GREEN << CHECK_MARK << " from_hex_string (BE) correct!" << NORMAL << std::endl;
	// } else {
	// 	std::cout << RED << CROSS_MARK << " from_hex_string (BE) incorrect!" << NORMAL << std::endl;
	// }

	// if (from_hex_le == simple_data) {
	// 	std::cout << GREEN << CHECK_MARK << " from_hex_string_le (LE) correct!" << NORMAL << std::endl;
	// } else {
	// 	std::cout << RED << CROSS_MARK << " from_hex_string_le (LE) incorrect!" << NORMAL << std::endl;
	// }

	// // Также покажем слова для понимания
	// std::cout << "\nWord comparison (first 4 words):" << std::endl;
	// std::cout << "Index | BE Result    | LE Result    | Expected     " << std::endl;
	// std::cout << "------|--------------|--------------|--------------" << std::endl;

	// for (int i = 0; i < 4; i++) {
	// 	std::cout << std::setw(4) << i << " | "
	// 			<< "0x" << std::hex << std::setw(8) << std::setfill('0') 
	// 			<< from_hex_be.get_word(i) << std::dec << " | "
	// 			<< "0x" << std::hex << std::setw(8) << std::setfill('0') 
	// 			<< from_hex_le.get_word(i) << std::dec << " | "
	// 			<< "0x" << std::hex << std::setw(8) << std::setfill('0') 
	// 			<< simple_data.get_word(i) << std::dec << std::endl;
	// }

	// Тест 6: Статические фабричные методы
	std::cout << "\n\n6. Testing Static Factory Methods:" << std::endl;
	std::cout << "-----------------------------------" << std::endl;

	// Hex строка в little-endian порядке (такой же как в памяти)
	std::string hex_str = "0x000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324252627";

	InOutData320 from_hex_le = InOutData320::from_hex_string_le(hex_str);

	std::cout << "From hex string (little-endian):" << std::endl;
	std::cout << "  Input: " << hex_str.substr(0, 40) << "..." << std::endl;
	from_hex_le.print_raw_memory("from_hex_le");

	// Ожидаемые данные (просто байты 0x00..0x27)
	std::vector<uint8_t> expected_bytes;
	for (int i = 0; i < 40; i++) {
		expected_bytes.push_back(static_cast<uint8_t>(i));
	}
	InOutData320 expected_data(expected_bytes);

	std::cout << "\nExpected (bytes 0x00..0x27):" << std::endl;
	expected_data.print_raw_memory("expected");

	// Проверка
	if (from_hex_le == expected_data) {
		std::cout << GREEN << CHECK_MARK << " Hex conversion correct! (bytes preserved)" << NORMAL << std::endl;
		
		// Дополнительная проверка
		std::cout << "\nVerification:" << std::endl;
		for (int i = 0; i < 40; i++) {
			uint8_t got = from_hex_le.get_byte(i);
			uint8_t expected = static_cast<uint8_t>(i);
			if (got == expected) {
				std::cout << "  byte[" << std::setw(2) << i << "] = 0x" << std::hex << std::setw(2) 
						<< static_cast<int>(got) << " ✓" << std::dec << std::endl;
			} else {
				std::cout << RED << "  byte[" << std::setw(2) << i << "] = 0x" << std::hex << std::setw(2) 
						<< static_cast<int>(got) << " expected 0x" << std::setw(2) 
						<< static_cast<int>(expected) << " ✗" << std::dec << NORMAL << std::endl;
			}
		}
	} else {
		std::cout << RED << CROSS_MARK << " Hex conversion incorrect!" << NORMAL << std::endl;
	}

	// Также проверяем from_hex_string (BE)
	InOutData320 from_hex_be = InOutData320::from_hex_string(hex_str);
	std::cout << "\nFor comparison - from_hex_string (big-endian):" << std::endl;
	from_hex_be.print_raw_memory("from_hex_be");

	// Тест 7: Побитовые операции
	std::cout << "\n\n7. Testing Bit Operations:" << std::endl;
	std::cout << "--------------------------" << std::endl;
	
	InOutData320 test_bits;
	
	// Устанавливаем несколько битов
	test_bits.set_bit(0, true);      // бит 0
	test_bits.set_bit(31, true);     // бит 31 (последний бит word0)
	test_bits.set_bit(128, true);    // бит 128 (первый бит word4)
	test_bits.set_bit(319, true);    // бит 319 (последний бит word9)
	
	std::cout << "Set bits at positions: 0, 31, 128, 319" << std::endl;
	std::cout << "Bit checks:" << std::endl;
	std::cout << "  bit 0   = " << test_bits.get_bit(0) << " (expected: 1)" << std::endl;
	std::cout << "  bit 31  = " << test_bits.get_bit(31) << " (expected: 1)" << std::endl;
	std::cout << "  bit 128 = " << test_bits.get_bit(128) << " (expected: 1)" << std::endl;
	std::cout << "  bit 319 = " << test_bits.get_bit(319) << " (expected: 1)" << std::endl;
	std::cout << "  bit 1   = " << test_bits.get_bit(1) << " (expected: 0)" << std::endl;
	
	// Тест 8: Бинарные операторы
	std::cout << "\n\n8. Testing Binary Operators:" << std::endl;
	std::cout << "----------------------------" << std::endl;
	
	InOutData320 mask_data = InOutData320::from_words(
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
		0xFFFFFFFF, 0x00000000
	);
	
	InOutData320 masked = data2 & mask_data;
	std::cout << "Mask operation (alternating words):" << std::endl;
	masked.print("masked data");
	
	// Тест 9: Для VectorTransformer (практическое использование)
	std::cout << "\n\n9. Practical Use Case for VectorTransformer:" << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	
	// Симуляция данных тестового вектора
	std::vector<uint8_t> test_vector_data;
	
	// Добавляем 320-битные данные (40 байт)
	for (int i = 0; i < 40; i++) {
		test_vector_data.push_back(static_cast<uint8_t>(i)); // 0x00, 0x01, ..., 0x27
	}
	
	// Добавляем другие данные...
	test_vector_data.push_back(0xAA); // дополнительные байты
	
	std::cout << "Simulated test vector (first 40 bytes = 320-bit data):" << std::endl;
	std::cout << "  Bytes 0-15: ";
	for (int i = 0; i < 16; i++) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(test_vector_data[i]) << " ";
	}
	std::cout << std::endl;
	std::cout << "  Bytes 16-31: ";
	for (int i = 16; i < 32; i++) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(test_vector_data[i]) << " ";
	}
	std::cout << std::endl;
	std::cout << "  Bytes 32-39: ";
	for (int i = 32; i < 40; i++) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
				  << static_cast<int>(test_vector_data[i]) << " ";
	}
	std::cout << std::dec << std::endl;
	
	// Парсинг как в VectorTransformer
	InOutData320 parsed_320bit(test_vector_data.data(), 40);
	std::cout << "\nParsed 320-bit data (using constructor):" << std::endl;
	parsed_320bit.print_detailed("parsed_320bit");
	
	parsed_320bit.set_byte (39, 0xFF);

	std::cout << "\n=========================================" << std::endl;
	std::cout << "=== INOUTDATA320 TESTS COMPLETE ===" << std::endl;
	std::cout << "=========================================\n" << std::endl;
}

void test_inoutdata8() {
	std::cout << "\n=====================================" << std::endl;
	std::cout << "=== TESTING InOutData8 STRUCTURE ===" << std::endl;
	std::cout << "=====================================\n" << std::endl;
	
	// Тест 1: Базовые операции
	std::cout << "1. Basic Operations:" << std::endl;
	std::cout << "-------------------" << std::endl;
	
	InOutData8 data1(0x5A);  // 01011010
	std::cout << "Created with 0x5A:" << std::endl;
	data1.print_detailed("data1");
	
	// Проверка конструктора из uint8_t
	InOutData8 data1b(0xA5);
	data1b.print_detailed("data1b (0xA5)");
	
	// Тест 2: Побитовые операции
	std::cout << "\n2. Bit Operations:" << std::endl;
	std::cout << "-----------------" << std::endl;
	
	InOutData8 data2;
	data2.set_bit(0, true);   // установить бит 0
	data2.set_bit(7, true);   // установить бит 7
	data2.set_bit(3, true);   // установить бит 3
	
	std::cout << "Set bits 0, 3, 7:" << std::endl;
	data2.print_detailed("data2");
	
	// Проверка get_bit
	std::cout << "Checking individual bits of data2:" << std::endl;
	for (int i = 0; i < 8; i++) {
		bool bit = data2.get_bit(i);
		std::cout << "  Bit " << i << ": " << (bit ? "1" : "0") << std::endl;
	}
	
	// Тест 3: Работа с векторами
	std::cout << "\n3. Vector Operations:" << std::endl;
	std::cout << "-------------------" << std::endl;
	
	std::vector<uint8_t> raw_data = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
	
	// Тест конструктора из вектора
	InOutData8 from_vec_ctor(raw_data, 2);
	std::cout << "From constructor (index 2):" << std::endl;
	from_vec_ctor.print_detailed("from_vec_ctor");
	std::cout << "Expected: 0x56" << std::endl;
	
	// Тест нестатического метода create_from_vector
	InOutData8 from_vec_method;
	from_vec_method.from_vector(raw_data, 3);
	std::cout << "\nFrom create_from_vector (index 3):" << std::endl;
	from_vec_method.print_detailed("from_vec_method");
	std::cout << "Expected: 0x78" << std::endl;
	
	// Тест статического метода from_vector (фабричный метод)
	InOutData8 from_static = InOutData8::create_from_vector(raw_data, 4);
	std::cout << "\nFrom static from_vector (index 4):" << std::endl;
	from_static.print_detailed("from_static");
	std::cout << "Expected: 0x9A" << std::endl;
	
	// Тест to_vector
	std::vector<uint8_t> back_to_vec = from_static.to_vector();
	std::cout << "\nBack to vector: ";
	for (auto byte : back_to_vec) {
		std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') 
				  << static_cast<int>(byte) << std::dec << " ";
	}
	std::cout << std::endl;
	
	// Тест 4: Операторы
	std::cout << "\n4. Operators:" << std::endl;
	std::cout << "------------" << std::endl;
	
	InOutData8 data3(0x0F);
	InOutData8 data4(0xF0);
	
	InOutData8 and_result = data3 & data4;  // 0x00
	InOutData8 or_result = data3 | data4;   // 0xFF
	InOutData8 xor_result = data3 ^ data4;  // 0xFF
	InOutData8 not_result = ~data3;         // 0xF0
	
	std::cout << "0x0F & 0xF0 = "; and_result.print_raw_memory("");
	std::cout << "0x0F | 0xF0 = "; or_result.print_raw_memory("");
	std::cout << "0x0F ^ 0xF0 = "; xor_result.print_raw_memory("");
	std::cout << "~0x0F = "; not_result.print_raw_memory("");
	
	// Составные операторы
	InOutData8 data5(0xAA);
	data5 &= data3;  // 0xAA & 0x0F = 0x0A
	std::cout << "0xAA &= 0x0F = "; data5.print_raw_memory("");
	
	InOutData8 data6(0x55);
	data6 |= data4;  // 0x55 | 0xF0 = 0xF5
	std::cout << "0x55 |= 0xF0 = "; data6.print_raw_memory("");
	
	// Операторы сравнения
	std::cout << "\nComparison operators:" << std::endl;
	std::cout << "data3 == data4: " << (data3 == data4 ? "true" : "false") << std::endl;
	std::cout << "data3 != data4: " << (data3 != data4 ? "true" : "false") << std::endl;
	
	InOutData8 data3_copy = data3;
	std::cout << "data3 == data3_copy: " << (data3 == data3_copy ? "true" : "false") << std::endl;
	
	// Тест 5: Операторы приведения
	std::cout << "\n5. Conversion Operators:" << std::endl;
	std::cout << "----------------------" << std::endl;
	
	InOutData8 data7(0x42);
	uint8_t as_uint8 = data7;  // неявное преобразование
	std::cout << "Implicit conversion to uint8_t: 0x" 
			  << std::hex << std::setw(2) << std::setfill('0') 
			  << static_cast<int>(as_uint8) << std::dec << std::endl;
	
	data7 = 0x99;  // оператор присваивания из uint8_t
	std::cout << "Assignment from uint8_t(0x99): ";
	data7.print_raw_memory("");
	
	// Тест 6: Для VectorTransformer
	std::cout << "\n6. Practical Use in VectorTransformer:" << std::endl;
	std::cout << "--------------------------------------" << std::endl;
	
	// Пример как может использоваться в вашем коде
	std::vector<uint8_t> test_vector = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,  // 320-битные данные
		0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,              // 128-битные данные
		0x55,                                            // 8-битные данные (thoxidat_i)
		0x0F                                             // 4-битные скалярные сигналы
	};
	
	// Симуляция парсинга в VectorTransformer
	size_t thoxidat_offset = 40 + 6;  // после 320-битных (40 байт) и 128-битных (6 байт) данных
	std::cout << "Total vector size: " << test_vector.size() << " bytes" << std::endl;
	std::cout << "Thoxidat offset: " << thoxidat_offset << std::endl;
	
	// Разные способы получить данные
	InOutData8 thoxidat_data1(test_vector, thoxidat_offset);  // конструктор
	InOutData8 thoxidat_data2 = InOutData8::create_from_vector(test_vector, thoxidat_offset);  // статический метод
	
	std::cout << "\nParsed thoxidat_i from test vector (constructor):" << std::endl;
	thoxidat_data1.print_detailed("thoxidat_data1");
	std::cout << "Expected: 0x55" << std::endl;
	
	std::cout << "\nParsed thoxidat_i from test vector (static method):" << std::endl;
	thoxidat_data2.print_detailed("thoxidat_data2");
	std::cout << "Expected: 0x55" << std::endl;
	
	// Тест граничных случаев
	std::cout << "\n7. Edge Cases:" << std::endl;
	std::cout << "--------------" << std::endl;
	
	// Пустой вектор
	std::vector<uint8_t> empty_vec;
	InOutData8 empty_data(empty_vec, 0);
	std::cout << "From empty vector: ";
	empty_data.print_raw_memory("empty_data");
	
	// Выход за границы вектора
	std::vector<uint8_t> small_vec = {0x11, 0x22};
	InOutData8 out_of_bounds(small_vec, 10);  // start_idx больше размера
	std::cout << "Out of bounds (index 10 in 2-element vector): ";
	out_of_bounds.print_raw_memory("out_of_bounds");
	
	// Последний элемент
	InOutData8 last_element(small_vec, 1);
	std::cout << "Last element: ";
	last_element.print_raw_memory("last_element");
	std::cout << "Expected: 0x22" << std::endl;
	
	// Тест 8: Форматы вывода
	std::cout << "\n8. Output Formats:" << std::endl;
	std::cout << "-----------------" << std::endl;
	
	InOutData8 test_data(0xAB);
	
	std::cout << "Default hex format: " << test_data.to_hex_string() << std::endl;
	std::cout << "Binary string: " << test_data.to_binary_string() << std::endl;
	std::cout << "Raw string (hex): " << test_data.to_raw_string(true) << std::endl;
	std::cout << "Raw string (binary): " << test_data.to_raw_string(false) << std::endl;
	std::cout << "To array: {" 
			  << "0x" << std::hex << std::setw(2) << std::setfill('0')
			  << static_cast<int>(test_data.to_array()[0]) << std::dec << "}" << std::endl;
	
	std::cout << "\n=====================================" << std::endl;
	std::cout << "=== INOUTDATA8 TESTS COMPLETE ===" << std::endl;
	std::cout << "=====================================\n" << std::endl;
}

int main(int argc, char** argv) {

    // Парсинг аргументов командной строки
	// ProgramOptions options = parse_arguments(argc, argv);

    // Транзация верхнего уровня
    // FramerTXTransaction Transaction;
    
    // Входной и выходной вектор в сборе
    // FramerTXInputData InpitData;
    // FramerTXOutputData OutputData;

    // Составные типы данные

    //InOutData320 Data320;
    // InOutData128 Data128;
    // InOutData112 Data112;
    // InOutData8   Data8;
    // InOutSignals4 InputScalarSignal;
    // InOutSignals5 OutputScalarSignal;

	test_inoutdata8();
	test_inoutdata320();

	return EXIT_SUCCESS;
}

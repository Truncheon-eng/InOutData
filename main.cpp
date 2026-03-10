#include "include/InOutData320.hpp"

void gen_random_vectors(std::vector<uint32_t>& test) {
    for(int i = 0; i < test.size(); i++) {
        test[i] = static_cast<uint32_t>(rand());
    }
    return;
}

int main(int argc, char * argv[]) {
    // std::vector<uint32_t> test(InOutData320::NUM_WORDS, 0);
    
    // gen_random_vectors(test);
    

    // проверка создания объекта на пример конструктор по определенным 32 битным словам
    InOutData320 obj_1{
        0x12345678, 0x12345678,
        0x12345678, 0x12345678,
        0x12345678, 0x12345678,
        0x12345678, 0x12345678,
        0x12345678, 0x12345678
    };

    std::cout << "0x"
        << std::hex << std::setw(8) << std::setfill('0') 
        << obj_1.get_word_little_endian(0)
        << std::setfill(' ')
        << std::endl;

    std::cout << "0x"
        << std::hex << std::setw(8) << std::setfill('0') 
        << obj_1.get_word_big_endian(0)
        << std::setfill(' ')
        << std::endl;


    obj_1.set_word(0, 0xaabbccdd);

    std::cout << "0x"
        << std::hex << std::setw(8) << std::setfill('0') 
        << obj_1.get_word(0)
        << std::setfill(' ')
        << std::endl;

    return 0;
}
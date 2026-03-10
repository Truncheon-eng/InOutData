#include "include/InOutData320.hpp"

void gen_random_vectors(std::vector<uint32_t>& test) {
    for(int i = 0; i < test.size(); i++) {
        test[i] = static_cast<uint32_t>(rand());
    }
    return;
}

int main(int argc, char * argv[]) {
    // проверка создания объекта на пример конструктор по определенным 32 битным словам
    InOutData320 obj_1{
        0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff
    };

    obj_1.set_default_output_format(InOutData320::OutputFormat::HEX_LITTLE_ENDIAN);

    obj_1.print_hex();

    std::cout << "Bit " << 10 << ": " << (obj_1.get_bit(10) ? "1" : "0" ) << std::endl;
    obj_1.set_bit(10, false);
    std::cout << "Bit " << 10 << ": " << (obj_1.get_bit(10) ? "1" : "0" ) << std::endl;

    obj_1.print_hex();

    return 0;
}
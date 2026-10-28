#include "include/InOutData320.hpp"
#include "include/InOutData128.hpp"

#define SEED 42

void gen_random_vectors(std::vector<uint32_t>& test) {
    for(int i = 0; i < test.size(); i++) {
        test[i] = static_cast<uint32_t>(rand());
    }
    return;
}

std::vector<uint8_t> gen_test_vector(void) {
    std::vector<uint8_t> res(16, 0);
    for(size_t i = 0; i < res.size(); i++)
        res[i] = static_cast<uint8_t>(rand());
    return res;
}

int main() {    
    srand(SEED);
    InOutData128 dut{gen_test_vector()};

    std::cout << "Test \"init InOutData128 with random value\"" << std::endl;
    
    for (size_t i = 0; i < InOutData128::NUM_BYTES; i++) {
	    std::cout << "\tdut[" << i << "] == 0x" << std::hex << std::setw(2) << std::setfill('0')
		<< static_cast<uint32_t>(dut.bytes[i])
		<< std::setfill('0')
		<< std::dec
		<< ";" << std::endl;
    }

    std::cout << "END TEST" << std::endl;

    return 0;
}

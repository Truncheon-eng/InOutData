#include "include/InOutData320.hpp"

void gen_random_vectors(std::vector<uint32_t>& test) {
    for(int i = 0; i < test.size(); i++) {
        test[i] = static_cast<uint32_t>(rand());
    }
    return;
}

int main(int argc, char * argv[]) {
    std::vector<uint32_t> test(InOutData320::NUM_WORDS, 0);
    
    gen_random_vectors(test);
    

    // проверка создания объекта на пример конструктор по определенным 32 битным словам
    InOutData320 obj_1{
        test[0], test[1],
        test[2], test[3],
        test[4], test[5],
        test[6], test[7],
        test[8], test[9]
    };
}
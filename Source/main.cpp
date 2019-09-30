#include <iostream>
#include "concepts.h"

int main() {
    std::cout << identifier::is_start()(0x1F600)  << std::endl;
    return 0;
}
#include "bigint.h"

int main() {
    bigint a, b;
    std::cin >> a >> b;
    bigint c = a * b;
    std::cout << c << std::endl;  
    return 0;
}
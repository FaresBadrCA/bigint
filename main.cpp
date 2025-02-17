#include "bigint.h"

int main() {
    bigint a, b;
    std::cin >> a >> b;
    bigint c = a * b;
    std::cout << (c >= 100 && c < 1000) << '\n';

    bigint d("12300000000000000000000000000000");
    c += d;
    std::cout << c << '\n';

    c *= 1000;
    c /= b;
    c %= 500000;
    int64_t e = static_cast<int64_t> (c);
    std::cout << e << '\n';
}
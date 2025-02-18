# 🔢 bigint 🔢

A header-only C++ implementation of arbitrary-length integers. It is meant to provide a balance between ease of use and performance. 

If you need maximum performance, use GMP instead (https://gmplib.org/).

## 🔧 Usage 🔧

```cpp
#include "bigint.h"

int main() {
    bigint a, b;
    std::cin >> a >> b;
    bigint c = a * b;

    std::cout << (c >= 100 && c < 1000) << '\n';
    c += a;
    c *= 1000;
    c /= b;
    c %= 500000;
    int64_t d = static_cast<int64_t> c;
    std::cout << c << '\n';
}
```

## 📋 ToDo List 📋
    ✅ Addition / Subtraction / Multiplication / Division / Modulo
    ✅ Relational Operators (==, !=, <, >, <=, >=)
    ✅ Mixed-type operations (bigint + int, etc...)
    ❌ Tests and benchmarks
    ❌ Base 2^32 or 2^64 representation
    ❌ Karatsuba multiplication
    ❌ Newton-Raphson division

## 📚 Other large integer libraries 📚
- https://github.com/rgroshanrg/bigint
- https://github.com/faheel/BigInt
- https://github.com/sercantutar/infint
- https://github.com/983/Num

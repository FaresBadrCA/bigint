# 🔢 bigint 🔢

A header-only C++ implementation of arbitrary-length integers.

## 🔧 Usage 🔧

```cpp
#include "bigint.h"

int main() {
    bigint a, b;
    std::cin >> a >> b;
    bigint c = a * b;
    c += 12;
    c *= 1000;
    c %= 500000;
    int64_t d = static_cast<int64_t> c;
    std::cout << c << std::endl;  
    return 0;
}
```

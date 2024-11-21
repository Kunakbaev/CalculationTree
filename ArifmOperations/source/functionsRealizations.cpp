#include <cstddef>
#include <cassert>

double add2nums(double a, double b) {
    return a + b;
}

double sub2nums(double a, double b) {
    return a - b;
}

double mul2nums(double a, double b) {
    return a * b;
}

double div2nums(double a, double b) {
    assert(b != 0);
    if (b == 0)
        return 0;
    return a / b;
}

#include <cstddef>
#include <cassert>
#include <math.h>

#include "../include/functionsRealizations.hpp"

static double root(double arg, double b) {
    assert(b != 0);
    return pow(arg, 1 / b);
}

static double myLog(double base, double arg) {
    assert(arg > 0);
    assert(base > 0);
    assert(base != 1);

    return log(arg) / log(base);
}

// WARNING: cringe: function gets 2 args, but works only with first and ignores second
static double ctan(double x) {
    // TODO: add error check
    assert(tan(x) != 0);
    return 1 / tan(x);
}

// ASK: what to do with division? no error check :(
#define ARIFM_OPP_INFIX_FUNC(operand, command, _, __)   \
    double command##Func(double a, double b) {      \
        return a operand b;                         \
    }                                               \

// ASK: same issue, no error check
#define ARIFM_OPP_UNARY_FUNC(_, command, __, ___)           \
    double command##Func(double a, double b) {      \
        return command(a);                     \
    }                                               \

#define ARIFM_OPP_BINARY_FUNC(_, command, __, ___)           \
    double command##Func(double a, double b) {      \
        return command(a, b);                     \
    }                                               \

#include "../include/functionsCodeGen/infixFunctionsPlainText.in"
#include "../include/functionsCodeGen/unaryFunctionsRealizations.in"
#include "../include/functionsCodeGen/binaryFunctionsRealizations.in"

#undef ARIFM_OPP_INFIX_FUNC
#undef ARIFM_OPP_UNARY_FUNC
#undef ARIFM_OPP_BINARY_FUNC


#ifndef ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_REALIZATIONS_HPP
#define ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_REALIZATIONS_HPP

#include <cstddef>
#include <cassert>

typedef double (*functionRealizationPtr)(double a, double b);

enum Function_type {
    UNARY_FUNC  = 0,
    BINARY_FUNC = 1,
};

struct Function {
    const char*             name;
    Function_type           type;
    functionRealizationPtr  func;
};

#define ARIFM_OPP_INFIX_FUNC(operand, name)         \
    double name##2numsFunc(double a, double b);     \

// ASK: same issue, no error check?
#define ARIFM_OPP_BINARY_FUNC(_, command)           \
    double command##Func(double a, double b);       \

// ASK: same issue, no error check?
#define ARIFM_OPP_UNARY_FUNC(_, command) \
    ARIFM_OPP_BINARY_FUNC(_, command)

#include "../include/functionsCodeGen/infixFunctionsPlainText.in"
#include "../include/functionsCodeGen/unaryFunctionsRealizations.in"
#include "../include/functionsCodeGen/binaryFunctionsRealizations.in"

#undef ARIFM_OPP_INFIX_FUNC
#undef ARIFM_OPP_UNARY_FUNC
#undef ARIFM_OPP_BINARY_FUNC

#endif

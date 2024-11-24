#ifndef ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_REALIZATIONS_HPP
#define ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_REALIZATIONS_HPP

#include <cstddef>
#include <cassert>
#include "arifmOperationsErrors.hpp"

typedef double                (*functionRealizationPtr )(double a, double b);
typedef ArifmOperationsErrors (*functionToLatexStringPtr)(char*  a, char*  b, char** result);

enum Function_type {
    UNARY_FUNC  = 0,
    BINARY_FUNC = 1,
};

#define ARIFM_OPP_GENERAL_FUNC(operand, name, _, __) \
    double name##Func(double a, double b);

#include "../include/functionsCodeGen/allFuncs.hpp"

// ASK: should be careful with nums that are assigned to this enums? On some compilers it can be random and not just 0, 1, 2, 3, ...?
#define ARIFM_OPP_GENERAL_FUNC(_, __, enumName, ___) ELEM_FUNC_##enumName,

enum FunctionsNames {
    #include "../include/functionsCodeGen/allFuncs.hpp"
};

#undef ARIFM_OPP_GENERAL_FUNC

struct Function {
    Function_type            type;
    FunctionsNames           name;
    functionRealizationPtr   calculationFunc;
    functionToLatexStringPtr latexToStringFunc;
};

#endif

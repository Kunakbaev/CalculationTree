#ifndef ARIFM_OPERATIONS_INCLUDE_FUNCS_TO_LATEX_STRING_HPP
#define ARIFM_OPERATIONS_INCLUDE_FUNCS_TO_LATEX_STRING_HPP

#define ARIFM_OPP_GENERAL_FUNC(_, command, __, ___) \
    ArifmOperationsErrors command##FuncToLatex(char* left, char* right, char** result);

#include "functionsCodeGen/allFuncs.hpp"

#undef ARIFM_OPP_GENERAL_FUNC

#endif

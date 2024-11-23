#ifndef ARIFM_OPERATIONS_INCLUDE_FUNCS_TO_LATEX_STRING_HPP
#define ARIFM_OPERATIONS_INCLUDE_FUNCS_TO_LATEX_STRING_HPP

#define ARIFM_OPP_BINARY_FUNC(_, command) \
    ArifmOperationsErrors command##FuncToLatex(char* left, char* right, char** result);

#define ARIFM_OPP_UNARY_FUNC ARIFM_OPP_BINARY_FUNC
#define ARIFM_OPP_INFIX_FUNC ARIFM_OPP_BINARY_FUNC

#include "functionsCodeGen/binaryFunctionsRealizations.in"
#include "functionsCodeGen/infixFunctionsPlainText.in"
#include "functionsCodeGen/unaryFunctionsRealizations.in"

#undef ARIFM_OPP_BINARY_FUNC
#undef ARIFM_OPP_UNARY_FUNC
#undef ARIFM_OPP_INFIX_FUNC

#endif

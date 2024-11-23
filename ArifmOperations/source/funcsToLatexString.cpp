#include <stdlib.h>
#include <cassert>
#include <string.h>

#include "../include/arifmOperationsErrors.hpp"
#include "../include/funcsToLatexString.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, ARIFM_OPERATIONS_INVALID_ARGUMENT, getArifmOperationsErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getArifmOperationsErrorMessage, ARIFM_OPERATIONS_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getArifmOperationsErrorMessage)\

static ArifmOperationsErrors funcToLatexCommonImplementation(char* left, char* right, const char* funcFormat, char** result) {
    IF_ARG_NULL_RETURN(left);
    IF_ARG_NULL_RETURN(right);
    IF_ARG_NULL_RETURN(funcFormat);
    IF_ARG_NULL_RETURN(result);
    LOG_DEBUG_VARS(format);
    LOG_DEBUG_VARS(left);
    LOG_DEBUG_VARS(right);

    size_t leftLen   = strlen(left);
    size_t rightLen  = strlen(right);
    size_t formatLen = strlen(funcFormat);

    size_t resLen = leftLen + rightLen + formatLen;
    *result = (char*)calloc(resLen + 1, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, ARIFM_OPERATIONS_MEMORY_ALLOCATION_ERROR);

    LOG_DEBUG_VARS(left, right, format);
    LOG_DEBUG_VARS(leftLen, rightLen, formatLen, resLen);
    snprintf(*result, resLen, funcFormat, left, right);

    return ARIFM_OPERATIONS_STATUS_OK;
}

#define FUNC_TO_LATEX(name, funcFormat) \
    ArifmOperationsErrors name##FuncToLatex(char* left, char* right, char** result) {\
        funcToLatexCommonImplementation(left, right, funcFormat, result);\
        return ARIFM_OPERATIONS_STATUS_OK;\
    }

// TODO: add this to code gen
// for unary functions left string must be empty
FUNC_TO_LATEX(cos,   "cos(%s%s)")
FUNC_TO_LATEX(sin,   "sin(%s%s)")
FUNC_TO_LATEX(tan,   "tan(%s%s)")
FUNC_TO_LATEX(ctan,  "ctan(%s%s)")
FUNC_TO_LATEX(myLog, "log_{%s}({%s})")
FUNC_TO_LATEX(pow,   "{%s}^{%s}")
FUNC_TO_LATEX(root,  "\\sqrt[%s]{%s}")
FUNC_TO_LATEX(add,   "{%s}+{%s}")
FUNC_TO_LATEX(sub,   "{%s}-{%s}")
FUNC_TO_LATEX(mul,   "{%s}\\cdot{%s}")
FUNC_TO_LATEX(div,   "\\frac{%s}{%s}")










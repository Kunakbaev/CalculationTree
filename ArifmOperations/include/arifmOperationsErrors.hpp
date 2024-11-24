#ifndef ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_ERROR_HPP
#define ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_ERROR_HPP

#include "../../include/errorsHandlerDefines.hpp"

#define ERROR_UNPACK(errName, errCode, errMessage) \
    ARIFM_OPERATIONS_##errName = errCode,

enum ArifmOperationsErrors {
    ARIFM_OPERATIONS_STATUS_OK = 0,
    #include "errorsPlainText.in"
};

#undef ERROR_UNPACK

const char* getArifmOperationsErrorMessage(ArifmOperationsErrors error);

#endif

#ifndef INCLUDE_TYPICAL_BIN_TREE_ERRORS_HPP
#define INCLUDE_TYPICAL_BIN_TREE_ERRORS_HPP

#include "../../include/errorsHandlerDefines.hpp"

#define ERROR_UNPACK(errName, errCode, errMessage) \
    errName = errCode,

enum ArifmTreeErrors {
    ARIFM_TREE_STATUS_OK             = 0,
    #include "errorsPlainText.in"
};

#undef ERROR_UNPACK

const char* getArifmTreeErrorMessage(ArifmTreeErrors error);

#endif
#ifndef ARIFM_TREE_READER_INCLUDE_READER_ERRORS_HPP
#define ARIFM_TREE_READER_INCLUDE_READER_ERRORS_HPP

#include "../../include/errorsHandlerDefines.hpp"

#define ERROR_UNPACK(errName, errCode, errMessage) \
    READER_##errName = errCode,

enum ReaderErrors {
    READER_STATUS_OK             = 0,
    #include "errorsPlainText.in"
};

#undef ERROR_UNPACK

const char* getReaderErrorMessage(ReaderErrors error);

#endif

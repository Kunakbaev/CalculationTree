#include "../include/readerErrors.hpp"

const char* getReaderErrorMessage(ReaderErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)          \
        case READER_##errName:                                           \
            return "Reader error: " errMessage ".\n";    \

    switch (error) {
        case READER_STATUS_OK:
            return "Reader: no errors occured.\n";

        // ASK: code generation
        #include "../include/errorsPlainText.in"

        default:
            return "Reader error: unknown error.\n";
    }

    #undef UNPACK
}

#include "../include/arifmOperationsErrors.hpp"

const char* getArifmOperationsErrorMessage(ArifmOperationsErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)          \
        case errName:                                           \
            return "Arifm operations error: " errMessage ".\n";    \

    switch (error) {
        case ARIFM_OPERATIONS_STATUS_OK:
            return "Arifm operations: no errors occured.\n";

        // ASK: code generation
        #include "../include/errorsPlainText.in"

        default:
            return "Arifm operations error: unknown error.\n";
    }

    #undef UNPACK
}

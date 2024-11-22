#include "../include/arifmTreeErrors.hpp"

const char* getArifmTreeErrorMessage(ArifmTreeErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)          \
        case ARIFM_TREE_##errName:                                           \
            return "Arifm tree error: " errMessage ".\n";    \

    switch (error) {
        case ARIFM_TREE_STATUS_OK:
            return "Arifm tree: no errors occured.\n";

        // ASK: code generation
        #include "../include/errorsPlainText.in"

        default:
            return "Arifm tree error: unknown error.\n";
    }

    #undef UNPACK
}

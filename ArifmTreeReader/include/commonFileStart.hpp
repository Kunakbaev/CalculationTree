#ifndef ARIFM_TREE_READER_INCLUDE_COMMON_FILE_START_HPP
#define ARIFM_TREE_READER_INCLUDE_COMMON_FILE_START_HPP

#include "../../ArifmeticTree/include/arifmTree.hpp"
#include "../../ArifmOperations/include/arifmOperations.hpp"
#include "reader.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, READER_INVALID_ARGUMENT, getReaderErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getReaderErrorMessage, READER_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getReaderErrorMessage)\

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, READER_DUMPER_ERROR);

#define ARIFM_OPS_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getArifmOperationsErrorMessage, ARIFM_OPERATIONS_STATUS_OK, READER_ARIFM_OPS_ERROR);

#define ARIFM_TREE_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getArifmTreeErrorMessage, ARIFM_TREE_STATUS_OK, READER_ARIFM_TREE_ERROR);


#endif

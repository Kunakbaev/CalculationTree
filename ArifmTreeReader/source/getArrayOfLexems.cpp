#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "../include/reader.hpp"
#include "../include/commonFileStart.hpp"

const size_t MAX_NUM_OF_LEXEMS  = 1 << 9;
const size_t MAX_INPUT_LINE_LEN = 1 << 11;
const char* DELIMS = " \n\t";
Node array[MAX_NUM_OF_LEXEMS] = {};
size_t arrInd = 0; // ASK: cringe?
char* tmpString = NULL;

static bool isDelimSkip(const char** ptr) {
    assert(ptr != NULL);
    assert(*ptr != NULL);

    if (strchr(DELIMS, **ptr) != NULL) {
        ++(*ptr);
        return true;
    }

    return false;
}

static bool tryNumber(const char** ptr) {
    assert(ptr != NULL);
    assert(*ptr != NULL);

    if (!isdigit(**ptr)) {
        return false;
    }

    errno = 0;
    char* endPtr = (char*)*ptr; // WARNING:
    double num = strtod(*ptr, &endPtr);
    if (errno != 0) {
        return false;
    }

    LOG_DEBUG_VARS(num, *ptr, endPtr);
    assert(arrInd < MAX_NUM_OF_LEXEMS);
    array[arrInd++] = {
        .nodeType = ARIFM_TREE_NUMBER_NODE,
        {.doubleData = num},
        .left = 0, .right = 0,
        .memBuffIndex = 0,
        .parent = 0,
    };
    *ptr = endPtr;
    return true;
}

static bool tryBracket(const char** ptr) {
    assert(ptr != NULL);
    assert(*ptr != NULL);

    char ch = **ptr;
    if (ch != '(' && ch != ')')
        return false;

    assert(arrInd < MAX_NUM_OF_LEXEMS);
    LOG_DEBUG_VARS("brackets");
    array[arrInd++] = {
        .nodeType = ARIFM_TREE_FUNC_NODE,
        {.data = (size_t)ch}, // ASK: is this ok
        .left = 0, .right = 0,
        .memBuffIndex = 0,
        .parent = 0,
    };
    ++(*ptr);

    return true;
}

void clearTmpString() {
    size_t strLen = strlen(tmpString);
    while (strLen > 0) {
        tmpString[--strLen] = '\0';
    }
}

static bool tryReadFuncName(const char* ptr) {
    assert(tmpString != NULL);

    size_t strLen = strlen(tmpString);
    if (strLen == 1 && isalpha(*(ptr - 1)) && !isalpha(*ptr)) { // this is variable
        assert(arrInd < MAX_NUM_OF_LEXEMS);
        array[arrInd++] = {
            .nodeType = ARIFM_TREE_VAR_NODE,
            {.data = (size_t)(*(ptr - 1) - 'a')}, // ASK: is this ok
            .left = 0, .right = 0,
            .memBuffIndex = 0,
            .parent = 0,
        };

        clearTmpString();
        return true;
    }

    // string is too long, so we asume
    // FIXME: add err check
    Function func = {};
    ArifmOperationsErrors error = getFunctionByName(tmpString, &func);
    if (error == ARIFM_OPERATIONS_FUNC_NOT_FOUND) {
        return false;
    }

    LOG_ERROR("----------");
    LOG_DEBUG_VARS("func name: ", tmpString);
    assert(arrInd < MAX_NUM_OF_LEXEMS);
    array[arrInd++] = {
        .nodeType = ARIFM_TREE_FUNC_NODE,
        {.data = (size_t)func.name}, // ASK: is this ok
        .left = 0, .right = 0,
        .memBuffIndex = 0,
        .parent = 0,
    };

    clearTmpString();

    return true;
}

ReaderErrors getArrayOfLexems(const char* line, size_t* arrLen, Node** arr) {
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(arrLen);
    IF_ARG_NULL_RETURN(arr);

    arrInd = 0;
    tmpString = (char*)calloc(MAX_INPUT_LINE_LEN, sizeof(char));
    IF_NOT_COND_RETURN(tmpString != NULL,
                       READER_MEMORY_ALLOCATION_ERROR);

    const char* ptr = line;
    while (*ptr != '\0') {
        LOG_ERROR("----------");
        if (isDelimSkip(&ptr)) continue;
        if (tryNumber(&ptr)) continue;
        if (tryBracket(&ptr)) continue;

        size_t tmpStringLen = strlen(tmpString);
        assert(tmpStringLen < MAX_INPUT_LINE_LEN);
        tmpString[tmpStringLen] = *ptr;
        ++ptr;
        if (tryReadFuncName(ptr)) continue;
    }

    // for (size_t i = 0; i < arrInd; ++i) {
    //     LOG_DEBUG_VARS(i, array[i].nodeType, array[i].data, array[i].doubleData);
    // }

    *arrLen = arrInd;
    *arr = (Node*)calloc(arrInd + 1, sizeof(Node));
    IF_NOT_COND_RETURN(*arr != NULL, READER_MEMORY_ALLOCATION_ERROR);
    memcpy(*arr, array, arrInd * sizeof(Node));
    (*arr)[arrInd] =  { // terminal element, just like \0 is in string
        .nodeType = ARIFM_TREE_INVALID_NODE,
        {.data = 0},
        .left = 0, .right = 0,
        .memBuffIndex = 0,
        .parent = 0,
    };

    return READER_STATUS_OK;
}


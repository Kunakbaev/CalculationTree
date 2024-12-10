#ifndef ARIFM_TREE_READER_INCLUDE_READER_HPP
#define ARIFM_TREE_READER_INCLUDE_READER_HPP

#include "readerErrors.hpp"
#include "../../ArifmeticTree/include/arifmTree.hpp"

enum LEXEM_TYPES {
    INVALID_LEXEM       = 0,
    NUMBER_LEXEM        = 1,
    OPERATOR_LEXEM      = 2,
    IDENTIFICATOR_LEXEM = 3,
};

ReaderErrors readFromFile(ArifmTree* tree, const char* fileName);
ReaderErrors getArrayOfLexems(const char* line, size_t* arrLen, Node** arr);

#endif

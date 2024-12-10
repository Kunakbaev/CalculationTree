#include "../include/reader.hpp"
#include "../include/commonFileStart.hpp"
#include "../include/definesForAppropriateRead.hpp"

/*
ReaderErrors getGrammar(ArifmTree* tree, size_t* index) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(index);



    return READER_STATUS_OK;
}
*/


struct Parser {
    ArifmTree* tree;
    size_t index;
    size_t arrLen;
    Node* array;
};

/*

NOTE:
once function does it's job

*/

/*

TODO:
TODO:
TODO:
REWRITE ALL THIS GARBAGE

*/

static ReaderErrors getGrammar(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    IF_ERR_RETURN(getAddSubSymbols(parser));
    IF_NOT_COND_RETURN(parser->index == parser->arrLen,
                       READER_INVALID_ARGUMENT); // TODO: add error type

    return READER_STATUS_OK;
}

static bool isCurLexemAddSubFunc(Parser* parser) {
    assert(parser != NULL);
    Node node = parser->array[parser->index];
    return (node.nodeType == ARIFM_TREE_FUNC_NODE) &&
           (node.data == ELEM_FUNC_ADD ||
            node.data == ELEM_FUNC_SUB);
}

static bool isCurLexemMulDivFunc(Parser* parser) {
    assert(parser != NULL);
    Node node = parser->array[parser->index];
    return (node.nodeType == ARIFM_TREE_FUNC_NODE) &&
           (node.data == ELEM_FUNC_MUL ||
            node.data == ELEM_FUNC_DIV);
}

// FIXME: cringe
static bool isCurLexemOpeningBracket(Parser* parser) {
    assert(parser != NULL);
    Node node = parser->array[parser->index];
    return (node.nodeType == ARIFM_TREE_FUNC_NODE) &&
           (node.data == '(');
}

static bool isCurLexemClosingBracket(Parser* parser) {
    assert(parser != NULL);
    Node node = parser->array[parser->index];
    return (node.nodeType == ARIFM_TREE_FUNC_NODE) &&
           (node.data == ')');
}

static ReaderErrors getAddSubSymbols(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    IF_ERR_RETURN(getMultDivSymbols(parser));
    while (parser->index < parser->arrLen && isCurLexemAddSubFunc(parser)) {
        Node node = parser->array[parser->index];

        size_t leftOperand = parser->tree->root;
        IF_ERR_RETURN(getMultDivSymbols(parser));
        size_t rightOperand = parser->tree->root;
        parser->tree->root = NEW_FUNC_NODE(node.data, leftOperand, rightOperand);

        ++parser->index;
    }

    return READER_STATUS_OK;
}

static ReaderErrors getMultDivSymbols(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    IF_ERR_RETURN(getParenthesis(parser));
    while (parser->index < parser->arrLen && isCurLexemMulDivFunc(parser)) {
        Node node = parser->array[parser->index];

        size_t leftOperand = parser->tree->root;
        IF_ERR_RETURN(getParenthesis(parser));
        size_t rightOperand = parser->tree->root;
        parser->tree->root = NEW_FUNC_NODE(node.data, leftOperand, rightOperand);

        ++parser->index;
    }

    return READER_STATUS_OK;
}

static ReaderErrors getParenthesis(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    return READER_STATUS_OK;
}

static ReaderErrors getNumber(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    return READER_STATUS_OK;
}

ReaderErrors constructArifmTreeFromTextLine(const char* line, ArifmTree* tree) {
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(tree);

    size_t arrLen = 0;
    Node* array = NULL;
    IF_ERR_RETURN(getArrayOfLexems(line, &arrLen, &array));

    for (size_t i = 0; i < arrLen; ++i) {
        LOG_DEBUG_VARS(i, array[i].nodeType, array[i].data, array[i].doubleData);
    }

    Parser parser = {
        .tree = tree,
        .index = 0,
        .arrLen = arrLen,
        .array = array,
    };

    return READER_STATUS_OK;
}

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

static ReaderErrors getAddSubSymbols(Parser* parser);
static ReaderErrors getMultDivSymbols(Parser* parser);
static ReaderErrors getFunctions(Parser* parser);
static ReaderErrors getParenthesis(Parser* parser);
static ReaderErrors getNumber(Parser* parser);
static ReaderErrors getVariable(Parser* parser);




static Node getCurrentLexem(const Parser* parser) {
    assert(parser != NULL);
    assert(parser->index < parser->arrLen);

    return parser->array[parser->index];
}

static ReaderErrors getGrammar(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    IF_ERR_RETURN(getAddSubSymbols(parser));
    dumpArifmTree(parser->tree);
    //openImageOfCurrentStateArifmTree(parser->tree);
    LOG_DEBUG_VARS(parser->index, parser->arrLen);
    IF_NOT_COND_RETURN(parser->index == parser->arrLen,
                       READER_INVALID_ARGUMENT); // TODO: add error type

    return READER_STATUS_OK;
}

static bool isCurLexemAddSubFunc(Parser* parser) {
    assert(parser != NULL);
    Node node = getCurrentLexem(parser);
    return (node.nodeType == ARIFM_TREE_FUNC_NODE) &&
           (node.data == ELEM_FUNC_ADD ||
            node.data == ELEM_FUNC_SUB);
}

static bool isCurLexemMulDivFunc(Parser* parser) {
    assert(parser != NULL);
    Node node = getCurrentLexem(parser);
    return (node.nodeType == ARIFM_TREE_FUNC_NODE) &&
           (node.data == ELEM_FUNC_MUL ||
            node.data == ELEM_FUNC_DIV);
}

// FIXME: cringe
static bool isCurLexemOpeningBracket(Parser* parser) {
    assert(parser != NULL);
    Node node = getCurrentLexem(parser);
    return (node.nodeType == ARIFM_TREE_FUNC_NODE) &&
           (node.data == '(');
}

static bool isCurLexemClosingBracket(Parser* parser) {
    assert(parser != NULL);
    Node node = getCurrentLexem(parser);
    return (node.nodeType == ARIFM_TREE_FUNC_NODE) &&
           (node.data == ')');
}

static bool isCurLexemNumber(Parser* parser) {
    assert(parser != NULL);
    Node node = getCurrentLexem(parser);
    return node.nodeType == ARIFM_TREE_NUMBER_NODE;
}

static bool isCurLexemVariable(Parser* parser) {
    assert(parser != NULL);
    Node node = getCurrentLexem(parser);
    return node.nodeType == ARIFM_TREE_VAR_NODE;
}

static bool isCurLexemComplexFunction(Parser* parser) {
    assert(parser != NULL);
    Node node = getCurrentLexem(parser);
    // TODO: cringe
    return node.nodeType == ARIFM_TREE_FUNC_NODE &&
           !isCurLexemMulDivFunc(parser) &&
           !isCurLexemOpeningBracket(parser) &&
           !isCurLexemClosingBracket(parser) &&
           !isCurLexemAddSubFunc(parser);
}













static ReaderErrors getAddSubSymbols(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    IF_ERR_RETURN(getMultDivSymbols(parser));
    LOG_DEBUG_VARS(parser->index);
    //openImageOfCurrentStateArifmTree(parser->tree);


    while (parser->index < parser->arrLen && isCurLexemAddSubFunc(parser)) {
        Node node = getCurrentLexem(parser);
        ++parser->index;

        size_t leftOperand = parser->tree->root;
        IF_ERR_RETURN(getMultDivSymbols(parser));
        size_t rightOperand = parser->tree->root;
        parser->tree->root = NEW_FUNC_NODE(node.data, leftOperand, rightOperand);
    }

    return READER_STATUS_OK;
}

static ReaderErrors getMultDivSymbols(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    IF_ERR_RETURN(getFunctions(parser));
    LOG_DEBUG_VARS("--------------------", parser->index);
    while (parser->index < parser->arrLen && isCurLexemMulDivFunc(parser)) {
        Node node = getCurrentLexem(parser);
        ++parser->index;
        LOG_DEBUG_VARS("--------------------", parser->index);

        size_t leftOperand = parser->tree->root;
        IF_ERR_RETURN(getFunctions(parser));
        size_t rightOperand = parser->tree->root;
        LOG_ERROR("mult parser node func");
        parser->tree->root = NEW_FUNC_NODE(node.data, leftOperand, rightOperand);
    }

    return READER_STATUS_OK;
}

static ReaderErrors getFunctions(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    LOG_DEBUG_VARS("aaaaaaaaaaaaaaaaahhhhhhhhh");
    if (!isCurLexemComplexFunction(parser)) {
        IF_ERR_RETURN(getParenthesis(parser));
        return READER_STATUS_OK;
    }

    LOG_ERROR("--------");
    Node node = getCurrentLexem(parser);
    LOG_ERROR("---------------------");
    LOG_DEBUG_VARS(node.data, node.nodeType);
    ++parser->index;
    IF_ERR_RETURN(getParenthesis(parser));
    parser->tree->root = NEW_FUNC_NODE(node.data, 0, parser->tree->root);
    // TODO: binary funcs

    return READER_STATUS_OK;
}

static ReaderErrors getParenthesis(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    Node node = getCurrentLexem(parser);
    if (!isCurLexemOpeningBracket(parser)) {
        IF_ERR_RETURN(getNumber(parser));
        IF_ERR_RETURN(getVariable(parser));
        return READER_STATUS_OK;
    }

    ++parser->index;
    LOG_DEBUG_VARS(parser->index);
    IF_ERR_RETURN(getAddSubSymbols(parser));
    LOG_DEBUG_VARS(parser->index);
    //openImageOfCurrentStateArifmTree(parser->tree);
    IF_NOT_COND_RETURN(isCurLexemClosingBracket(parser), READER_INVALID_ARGUMENT); // TODO: add error
    ++parser->index;

    return READER_STATUS_OK;
}

static ReaderErrors getNumber(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    if (parser->index >= parser->arrLen)
        return READER_STATUS_OK;
    if (!isCurLexemNumber(parser))
        return READER_STATUS_OK;

    LOG_DEBUG_VARS(parser->index);
    Node node = getCurrentLexem(parser);
    parser->tree->root = NEW_NUM_NODE(node.doubleData);
    ++parser->index;
    LOG_DEBUG_VARS(parser->index);

    return READER_STATUS_OK;
}

static ReaderErrors getVariable(Parser* parser) {
    IF_ARG_NULL_RETURN(parser);

    if (parser->index >= parser->arrLen)
        return READER_STATUS_OK;
    if (!isCurLexemVariable(parser))
        return READER_STATUS_OK;

    LOG_DEBUG_VARS(parser->index);
    Node node = getCurrentLexem(parser);
    parser->tree->root = NEW_VAR_NODE(node.data);
    ++parser->index;
    LOG_DEBUG_VARS(parser->index);

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

    IF_ERR_RETURN(getGrammar(&parser));
    dumpArifmTree(tree);
    FREE(parser.array);

    return READER_STATUS_OK;
}

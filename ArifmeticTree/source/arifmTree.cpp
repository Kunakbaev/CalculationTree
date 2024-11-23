#include <stdlib.h>
#include <inttypes.h>

#include "../include/arifmTree.hpp"
#include "../../Dumper/include/dumper.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, ARIFM_TREE_INVALID_ARGUMENT, getArifmTreeErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getArifmTreeErrorMessage, ARIFM_TREE_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getArifmTreeErrorMessage)\

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, ARIFM_TREE_DUMPER_ERROR);

#define ARIFM_OPS_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getArifmOperationsErrorMessage, ARIFM_OPERATIONS_STATUS_OK, ARIFM_TREE_ARIFM_OPS_ERROR);

#define RETURN_IF_INVALID() \
    IF_ERR_RETURN(validateArifmTree(tree));


const size_t MIN_MEM_BUFF_SIZE  = 8; // TODO: fix error with array resize
const size_t OUTPUT_BUFFER_SIZE = 1 << 9;
const size_t MAX_OBJ_NAME_LEN   = 30;
const char   BREAK_CHAR         = '|';

static void initMemBuff(ArifmTree* tree) {
    assert(tree != NULL);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        tree->memBuff[nodeInd].memBuffIndex = nodeInd;
    }
}

ArifmTreeErrors constructArifmTree(ArifmTree* tree, Dumper* dumper) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(dumper);

    tree->root              = 0;
    tree->memBuff           = (Node*)calloc(MIN_MEM_BUFF_SIZE, sizeof(Node));
    IF_NOT_COND_RETURN(tree->memBuff != NULL,
                       ARIFM_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuffSize       = MIN_MEM_BUFF_SIZE;
    tree->freeNodeIndex     = 0; // 0 index is equal to NULL
    tree->dumper            = *dumper;
    initMemBuff(tree);

    RETURN_IF_INVALID();
    return ARIFM_TREE_STATUS_OK;
}

static ArifmTreeErrors resizeMemBuffer(ArifmTree* tree, size_t newSize) {
    IF_ARG_NULL_RETURN(tree);

    if (newSize < MIN_MEM_BUFF_SIZE)
        newSize = MIN_MEM_BUFF_SIZE;

    if (tree->memBuffSize == newSize) // nothing to do
        return ARIFM_TREE_STATUS_OK;

    size_t oldSize   = tree->memBuffSize;
    size_t deltaSize = tree->memBuffSize > newSize
                            ? tree->memBuffSize - newSize
                            : newSize - tree->memBuffSize;
    size_t deltaBytes = deltaSize * sizeof(Node);

    if (oldSize > newSize) {
        memset(tree->memBuff + newSize, 0, deltaBytes);
    }

    LOG_DEBUG_VARS(oldSize, deltaSize, deltaBytes);
    Node* tmp = (Node*)realloc(tree->memBuff, newSize * sizeof(Node));
    IF_NOT_COND_RETURN(tmp != NULL, ARIFM_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuff     = tmp;
    tree->memBuffSize = newSize;

    LOG_DEBUG_VARS(tmp);

    if (oldSize < newSize) {
        memset(tree->memBuff + oldSize, 0, deltaBytes - 1);
    }

    // if oldSize > newSize, no iterations will be executed
    for (size_t nodeInd = oldSize; nodeInd < newSize; ++nodeInd) {
        tree->memBuff[nodeInd].memBuffIndex = nodeInd;
    }

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors getNewNode(ArifmTree* tree, size_t* newNodeIndex) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(newNodeIndex);

    if (tree->freeNodeIndex + 1 >= tree->memBuffSize) {
        IF_ERR_RETURN(resizeMemBuffer(tree, tree->memBuffSize * 2));
    }
    assert(tree->freeNodeIndex < tree->memBuffSize);

    *newNodeIndex = ++tree->freeNodeIndex;
    LOG_DEBUG_VARS(*newNodeIndex, tree->memBuffSize);
    LOG_DEBUG_VARS(newNodeIndex, tree->memBuff[*newNodeIndex].memBuffIndex);

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors isNodeAleftSonOfParent(const ArifmTree* tree, size_t parentInd, size_t vertInd, bool* is) {
    IF_ARG_NULL_RETURN(tree);
    IF_NOT_COND_RETURN(parentInd < tree->memBuffSize,
                       ARIFM_TREE_INVALID_ARGUMENT);

    Node parent = tree->memBuff[parentInd];
    *is = vertInd == parent.left;

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors getArifmTreeNodeByVertIndex(const ArifmTree* tree, size_t vertInd, Node* result) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(result);
    IF_NOT_COND_RETURN(vertInd < tree->memBuffSize,
                       ARIFM_TREE_INVALID_ARGUMENT); // TODO: add error

    *result = tree->memBuff[vertInd];

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors isArifmTreeEmpty(const ArifmTree* tree, bool* is) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(is);

    *is = tree->root == 0;

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors isArifmTreeNodeLeaf(const ArifmTree* tree, size_t currentNodeInd, bool* is) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(is);

    if (!currentNodeInd) {
        *is = true;
        return ARIFM_TREE_STATUS_OK;
    }

    assert(currentNodeInd < tree->memBuffSize);
    Node node = tree->memBuff[currentNodeInd];
    *is = node.left == 0 && node.right == 0;
    return ARIFM_TREE_STATUS_OK;
}



#include "readArifmTreeFromFile.cpp"
#include "saveArifmTreeToFile.cpp"

static ArifmTreeErrors dumpArifmTreeInConsole(const ArifmTree* tree, size_t nodeIndex,
                                                              char** outputBuffer) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(outputBuffer);
    IF_ARG_NULL_RETURN(*outputBuffer);

    if (nodeIndex == 0) {
        strncat(*outputBuffer, "?", OUTPUT_BUFFER_SIZE);
        ++(*outputBuffer);
        return ARIFM_TREE_STATUS_OK;
    }

    strncat(*outputBuffer, "(", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    assert(nodeIndex < tree->memBuffSize);
    Node node = tree->memBuff[nodeIndex];
    IF_ERR_RETURN(dumpArifmTreeInConsole(tree, node.left, outputBuffer));

    char* nodeDataString = NULL;
    ARIFM_OPS_ERR_CHECK(arifmTreeNodeToString(&node, &nodeDataString));
    LOG_DEBUG_VARS(node.memBuffIndex, nodeDataString);
    (*outputBuffer) += snprintf(*outputBuffer, OUTPUT_BUFFER_SIZE, " %s ", nodeDataString);
    FREE(nodeDataString);

    IF_ERR_RETURN(dumpArifmTreeInConsole(tree, node.right, outputBuffer));

    strncat(*outputBuffer, ")", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors dumpArifmTree(ArifmTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    LOG_DEBUG("--------------------------------------\n");
    LOG_DEBUG("arifmetic tree:\n");
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, "--------------------------------------\n");
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, "arifmetic tree:\n");

    char* outputBuffer = (char*)calloc(OUTPUT_BUFFER_SIZE, sizeof(char)); // ASK: is this ok?
    IF_NOT_COND_RETURN(outputBuffer != NULL,
                       ARIFM_TREE_MEMORY_ALLOCATION_ERROR);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        Node node = tree->memBuff[nodeInd];
        char* data = NULL;
        ARIFM_OPS_ERR_CHECK(arifmTreeNodeToString(&node, &data));

        size_t parent = node.parent;
        size_t left   = node.left;
        size_t right  = node.right;
        LOG_DEBUG_VARS(nodeInd, data, parent, left, right);
        DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, nodeInd, data, parent, left, right);
        FREE(data);
    }

    char* targetPtr = outputBuffer;
    IF_ERR_RETURN(dumpArifmTreeInConsole(tree, tree->root, &targetPtr));
    LOG_DEBUG(outputBuffer);
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, outputBuffer);
    FREE(outputBuffer);

    // FIXME: add dumper err check
    size_t nodesArr[MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT] = {}; // ASK: is it ok?
    nodesArr[0] = {tree->freeNodeIndex};
    NodesWithColor rule1 = {
        "green",
        (size_t)1,
        nodesArr,
    };
    NodesWithColor coloringRule[] = {rule1};
    DUMPER_ERR_CHECK(dumperDumpArifmTree(&tree->dumper, tree, coloringRule, 1));

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors openImageOfCurrentStateArifmTree(ArifmTree* tree) {
    IF_ARG_NULL_RETURN(tree);
    //RETURN_IF_INVALID();

    // first we need to create image of current state of tree
    IF_ERR_RETURN(dumpArifmTree(tree));

    // FIXME: be carefull with nasty commands
    const char* fileName = getLastImageFileName(&tree->dumper);
    const size_t TMP_LEN = 100;
    char tmp[TMP_LEN] = {};
    snprintf(tmp, TMP_LEN, "xdg-open %s", fileName);
    system(tmp);

    RETURN_IF_INVALID();
    return ARIFM_TREE_STATUS_OK;
}

// validates arifmetic tree, works not so fast
ArifmTreeErrors validateArifmTree(const ArifmTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    for (size_t nodeInd = 1; nodeInd < tree->memBuffSize; ++nodeInd) {
        Node node = tree->memBuff[nodeInd];
        Function func = {};

        switch (node.nodeType) {
            case ARIFM_TREE_NUMBER_NODE:
                IF_NOT_COND_RETURN(node.left == 0 && node.right == 0,
                                   ARIFM_TREE_NUM_NODE_NOT_LEAF);
                break;
            case ARIFM_TREE_VAR_NODE:
                IF_NOT_COND_RETURN(node.left == 0 && node.right == 0,
                                   ARIFM_TREE_VAR_NODE_NOT_LEAF);
                break;
            case ARIFM_TREE_FUNC_NODE:
                ARIFM_OPS_ERR_CHECK(getFuncByIndex(node.data, &func));
                LOG_DEBUG_VARS(func.type, func.name, node.data, node.doubleData);
                if (func.type == BINARY_FUNC) {
                    IF_NOT_COND_RETURN(node.left != 0 && node.right != 0,
                                       ARIFM_TREE_FUNC_NODE_NOT_ENOUGH_ARGS);
                } else {
                    IF_NOT_COND_RETURN(node.left == 0 && node.right != 0,
                                       ARIFM_TREE_UNARY_FUNC_NODE_BAD_ARGS);
                }
                break;
            default:
                return ARIFM_TREE_INVALID_NODE_TYPE;
        }
    }

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors destructArifmTree(ArifmTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    FREE(tree->memBuff);

    tree->memBuffSize   = 0;
    tree->freeNodeIndex = 0;
    dumperDestructor(&tree->dumper);

    return ARIFM_TREE_STATUS_OK;
}

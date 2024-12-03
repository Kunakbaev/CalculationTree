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
const size_t MAX_OBJ_NAME_LEN   = 30;

static void initMemBuff(ArifmTree* tree) {
    assert(tree != NULL);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        tree->memBuff[nodeInd].memBuffIndex = nodeInd;
    }
}

ArifmTreeErrors constructArifmTree(ArifmTree* tree, const Dumper* dumper) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(dumper);

    ARIFM_OPS_ERR_CHECK(validateArifmOperationsArrays());

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

static ArifmTreeErrors constructNode(ArifmTree* tree, Node* node, TreeNodeType nodeType, const NodeDataUnion data) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(node);

    node->left     = node->right = node->parent = 0;
    node->nodeType = nodeType;
    double* ptr    = NULL;
    LOG_DEBUG_VARS(data.data, data.doubleData);
    switch (nodeType) {
        case ARIFM_TREE_VAR_NODE:
            node->data = data.data;
            break;
        case ARIFM_TREE_NUMBER_NODE:
            node->doubleData = data.doubleData;
            break;
        case ARIFM_TREE_FUNC_NODE:
            node->data = data.data;
            break;
        default:
            return ARIFM_TREE_INVALID_NODE_TYPE;
    }

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors constructNodeWithKids(ArifmTree* tree, size_t* newNodeInd, TreeNodeType nodeType, const NodeDataUnion data,
                                      size_t leftSon,  size_t rightSon) {
    IF_ARG_NULL_RETURN(newNodeInd);
    IF_ARG_NULL_RETURN(tree);

    IF_ERR_RETURN(getNewNode(tree, newNodeInd));
    Node* node = getArifmTreeNodePtr(tree, *newNodeInd);

    IF_ERR_RETURN(constructNode(tree, node, nodeType, data));
    node->left  = leftSon;
    node->right = rightSon;
    return ARIFM_TREE_STATUS_OK;
}

size_t constructNodeWithKidsNoErrors(ArifmTree* tree, TreeNodeType nodeType, const NodeDataUnion data,
                                     size_t leftSon,  size_t rightSon) {
    size_t newNodeInd = 0;
    ArifmTreeErrors error = ARIFM_TREE_STATUS_OK;
    error = getNewNode(tree, &newNodeInd);
    assert(error == ARIFM_TREE_STATUS_OK);
    Node* node = getArifmTreeNodePtr(tree, newNodeInd);

    error = constructNode(tree, node, nodeType, data);
    assert(error == ARIFM_TREE_STATUS_OK);

    node->left  = leftSon;
    node->right = rightSon;
    return newNodeInd;
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

ArifmTreeErrors linkNewNodeToParent(ArifmTree* tree, size_t parentInd, bool isLeftSon,
                                    size_t* newNodeInd, const char* substr) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(newNodeInd);
    IF_ARG_NULL_RETURN(substr);

    *newNodeInd = 0;
    IF_ERR_RETURN(getNewNode(tree, newNodeInd));
    if (tree->root == 0) // tree is empty
        tree->root = *newNodeInd;
    Node* node = getArifmTreeNodePtr(tree, *newNodeInd);

    if (parentInd != 0) {
        node->parent = parentInd;
        Node* parent = getArifmTreeNodePtr(tree, parentInd);
        if (isLeftSon)
            parent->left  = *newNodeInd;
        else
            parent->right = *newNodeInd;
    }

    ARIFM_OPS_ERR_CHECK(initArifmTreeNodeWithString(node, substr));
    LOG_DEBUG_VARS((node)->data, (node)->nodeType);

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

    return ARIFM_TREE_STATUS_OK;
}

Node* getArifmTreeNodePtr(const ArifmTree* tree, size_t nodeInd) {
    assert(tree    != NULL);
    assert(nodeInd <= tree->freeNodeIndex);
    assert(nodeInd != 0);

    return &tree->memBuff[nodeInd];
}

size_t getCopyOfSubtree(const ArifmTree* tree, ArifmTree* destTree,
                        size_t srcNodeInd, size_t parentInd, bool isLeftSon) {
    if (!srcNodeInd)
        return 0;

    assert(tree     != NULL);
    assert(destTree != NULL);

    size_t dest = 0;
    IF_ERR_RETURN(getNewNode(destTree, &dest));
    Node* node = getArifmTreeNodePtr(destTree, dest);
    Node   old = *getArifmTreeNodePtr(tree, srcNodeInd);
    LOG_DEBUG_VARS(old.data, old.nodeType, old.memBuffIndex);

    node->nodeType   = old.nodeType;
    node->data       = old.data;
    node->doubleData = old.doubleData;

    if (parentInd != 0) {
        Node* parent = getArifmTreeNodePtr(destTree, parentInd);
        if (isLeftSon)
            parent->left  = dest;
        else
            parent->right = dest;
    }

    // FIXME: кажется копипаст, переписать. Выполнять для себя, вызывать для детей
    getCopyOfSubtree(tree, destTree, old.left,  dest, true);
    getCopyOfSubtree(tree, destTree, old.right, dest, false);

    return dest;
}

//#include "readArifmTreeFromFile.cpp"
#include "../../ArifmTreeReader/include/reader.hpp"

#define READER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getReaderErrorMessage, READER_STATUS_OK, ARIFM_TREE_READER_ERROR);

#include "saveArifmTreeToFile.cpp"
#include "findDerivativeOfTree.cpp"
#include "treeSimplification.cpp"
#include "treeDumperFuncs.cpp"
#include "treeValidationFuncs.cpp"

ArifmTreeErrors readArifmTreeFromFile(ArifmTree* tree, const char* fileName) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(fileName);

    READER_ERR_CHECK(readFromFile(tree, fileName));

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors destructArifmTree(ArifmTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    LOG_DEBUG_VARS(&tree->dumper);
    FREE(tree->memBuff);

    tree->memBuffSize   = 0;
    tree->freeNodeIndex = 0;

    return ARIFM_TREE_STATUS_OK;
}

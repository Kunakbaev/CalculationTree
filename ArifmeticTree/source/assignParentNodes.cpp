#include "../include/commonFileStart.hpp"
#include "../include/assingParents.hpp"

ArifmTreeErrors assignParentNodes(ArifmTree* tree, size_t nodeInd, size_t parentInd) {
    IF_ARG_NULL_RETURN(tree);
    IF_NOT_COND_RETURN(nodeInd <= tree->freeNodeIndex,
                       ARIFM_TREE_INVALID_ARGUMENT);

    if (!nodeInd)
        return ARIFM_TREE_STATUS_OK;

    Node* node = &tree->memBuff[nodeInd];
    LOG_DEBUG_VARS(nodeInd, node->memBuffIndex, node->left, node->right);
    node->parent = parentInd;
    if (node->left != 0) {
        IF_ERR_RETURN(assignParentNodes(tree, node->left, nodeInd));
    }
    if (node->right != 0) {
        IF_ERR_RETURN(assignParentNodes(tree, node->right, nodeInd));
    }

    return ARIFM_TREE_STATUS_OK;
}

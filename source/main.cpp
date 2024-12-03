#include <iostream>

#include "../Dumper/include/dumper.hpp"
#include "../ArifmeticTree/include/arifmTree.hpp"
#include "logLib.hpp"

#define IF_MAIN_ARIFM_TREE_ERR_RETURN(error)                                   \
    do {                                                            \
        ArifmTreeErrors tmp = error;                             \
        if (tmp != ARIFM_TREE_STATUS_OK) {                       \
            LOG_ERROR(getArifmTreeErrorMessage(tmp));            \
            /* if error occurs in destructor, then we are screwed*/ \
            destructArifmTree(&tree);                            \
            assert(tmp != ARIFM_TREE_STATUS_OK);                 \
            return tmp;                                             \
        }                                                           \
    } while(0)

int main() {
    setLoggingLevel(DEBUG);

    LOG_DEBUG("Hello world!");

    Dumper dumper = {};
    dumperConstructor(&dumper, 10, "logs", "png"); // referece_count: ctor +1
                                                   //                 dtor -1, if (0) -> free

    ArifmTree tree = {};
    IF_MAIN_ARIFM_TREE_ERR_RETURN(constructArifmTree(&tree, &dumper));
    IF_MAIN_ARIFM_TREE_ERR_RETURN(readArifmTreeFromFile(&tree, "expression.txt"));
    //IF_MAIN_ARIFM_TREE_ERR_RETURN(dumpArifmTree(&tree));
    //IF_MAIN_ARIFM_TREE_ERR_RETURN(openImageOfCurrentStateArifmTree(&tree));
    //IF_MAIN_ARIFM_TREE_ERR_RETURN(saveArifmTreeToFile(&tree, "expresionLatex.tex"));

    ArifmTree diffTree = {};
    IF_MAIN_ARIFM_TREE_ERR_RETURN(getDerivativeOfTree(&tree, &diffTree));
    IF_MAIN_ARIFM_TREE_ERR_RETURN(saveArifmTreeToFile(&diffTree, "bruh.tex"));

    // IF_MAIN_ARIFM_TREE_ERR_RETURN(simplifyTree(&tree));
    // IF_MAIN_ARIFM_TREE_ERR_RETURN(openImageOfCurrentStateArifmTree(&tree));

    // LOG_ERROR("--------------------------------");
    IF_MAIN_ARIFM_TREE_ERR_RETURN(simplifyTree(&diffTree));
    IF_MAIN_ARIFM_TREE_ERR_RETURN(saveArifmTreeToFile(&diffTree, "bruh.tex"));
    IF_MAIN_ARIFM_TREE_ERR_RETURN(openImageOfCurrentStateArifmTree(&diffTree));

    IF_MAIN_ARIFM_TREE_ERR_RETURN(destructArifmTree(&tree));
    IF_MAIN_ARIFM_TREE_ERR_RETURN(destructArifmTree(&diffTree));
    dumperDestructor(&dumper);

    return 0;
}

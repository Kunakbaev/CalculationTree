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
    dumperConstructor(&dumper, 10, "logs", ".png");

    ArifmTree tree = {};
    IF_MAIN_ARIFM_TREE_ERR_RETURN(constructArifmTree(&tree, &dumper));
    IF_MAIN_ARIFM_TREE_ERR_RETURN(readArifmTreeFromFile(&tree, "expression.txt"));
    //IF_MAIN_ARIFM_TREE_ERR_RETURN(dumpArifmTree(&tree));
    IF_MAIN_ARIFM_TREE_ERR_RETURN(openImageOfCurrentStateArifmTree(&tree));

    IF_MAIN_ARIFM_TREE_ERR_RETURN(destructArifmTree(&tree));

    return 0;
}
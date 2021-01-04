/* ADLLEAF.cpp
 *   by Lut99
 *
 * Created:
 *   26/11/2020, 11:33:54
 * Last edited:
 *   04/01/2021, 17:52:18
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLLeaf class, which is used as a basis for all
 *   nodes in the tree that do not have any children themselves.
**/

#include "ADLLeaf.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLLEAF CLASS *****/

/* Constructor for the ADLLeaf, which takes the type of the derived file and a DebugInfo struct containing the information where this node came from. */
ADLLeaf::ADLLeaf(NodeType type, const DebugInfo& debug) :
    ADLNode(type, debug)
{}



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLLeaf::traverse_recurse(const char*, NodeType, ADLNode* (*)(const char*, ADLNode*)) {
    // Simply do not recurse, as this node doesn't have any children
    return;
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLLeaf::traverse_recurse(const char*, NodeType, ADLNode* (*)(const char*, ADLNode*, void*), void*) {
    // Simply do not recurse, as this node doesn't have any children
    return;
}

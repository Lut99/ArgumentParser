/* ADLLEAF.cpp
 *   by Lut99
 *
 * Created:
 *   26/11/2020, 11:33:54
 * Last edited:
 *   26/11/2020, 11:35:27
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

/* Constructor for the ADLLeaf, which takes the type of the derived file, a breadcrumb trail of filenames, the line number where this node originates from and a matching column number. */
ADLLeaf::ADLLeaf(NodeType type, const std::vector<std::string>& filenames, size_t line, size_t col) :
    ADLNode(type, filenames, line, col)
{}



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLLeaf::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    // Simply do not recurse, as this node doesn't have any children
    return;
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLLeaf::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    // Simply do not recurse, as this node doesn't have any children
    return;
}

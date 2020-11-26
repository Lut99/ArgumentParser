/* ADLTREE.cpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:44:56 PM
 * Last edited:
 *   26/11/2020, 15:33:07
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file defines the tree used to represent a parsed ADL file.
**/

#include <algorithm>

#include "ADLTree.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLTREE CLASS *****/

/* Constructor for the ADLTree, which takes at least one file. */
ADLTree::ADLTree(ADLNode* file) :
    ADLBranch(NodeType::root, {}, di_empty, NodeType::file)
{
    // Add the file if it isn't a nullptr
    if (file != nullptr) { this->add_node((ADLNode*) file); }
}



/* Merge this ADLTree with another one. */
ADLTree& ADLTree::operator+=(const ADLTree& other) {
    // Simply copy all files from that tree to this one
    for (size_t i = 0; i < other.children.size(); i++) {
        this->add_node(other.children[i]->copy());
    }

    // Return ourselves
    return *this;
}



/* Allows the ADLTree to be copied polymorphically. */
ADLTree* ADLTree::copy() const { return new ADLTree(*this); }

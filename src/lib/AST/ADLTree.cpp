/* ADLTREE.cpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:44:56 PM
 * Last edited:
 *   27/11/2020, 14:59:21
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



/* Prints the entire tree to the given output stream, reflecting the AST structure. */
std::ostream& ADLTree::print(std::ostream& os) const {
    // Simply print all files following each other
    os << endl;
    for (size_t i = 0; i < this->children.size(); i++) {
        ADLFile* file = (ADLFile*) this->children[i];

        // Print the file, possible with a few newlines first
        if (i > 0) { os << endl << endl << endl << endl << endl; }
        file->print(os);
    }
    return os << endl;
}



/* Allows the ADLTree to be copied polymorphically. */
ADLTree* ADLTree::copy() const { return new ADLTree(*this); }

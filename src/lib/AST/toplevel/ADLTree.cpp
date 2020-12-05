/* ADLTREE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:47:57
 * Last edited:
 *   12/5/2020, 5:00:41 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class rooting the entire Abstract Syntax Tree (AST) of the ADL.
**/

#include <algorithm>

#include "ADLTree.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLFILE CLASS *****/

/* Constructor for the ADLTree class, which only optionally takes a toplevel node. */
ADLTree::ADLTree(ADLNode* toplevel) :
    ADLBranch(NodeType::root, {}, di_empty, nt_toplevel)
{
    // Add the node if it isn't NULL
    if (toplevel != nullptr) { this->add_node(toplevel); }
}



/* Prints all nodes in this file, reflecting the AST structure. */
std::ostream& ADLTree::print(std::ostream& os) const {
    // Print a header of the filename, making sure it has an even size
    std::string header = "============================================";
    os << header << endl << endl;

    // Print each toplevel node, separated by newlines
    for (size_t i = 0; i < this->children.size(); i++) {
        if (i > 0) { os << endl; }
        this->children[i]->print(os);
    }

    // Print some form of footer with equal size of the header and we're done
    return os << endl << header << endl;
}



/* Allows the ADLTree to be copied polymorphically. */
ADLTree* ADLTree::copy() const { return new ADLTree(*this); }

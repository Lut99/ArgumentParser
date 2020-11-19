/* ADLTREE.cpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:44:56 PM
 * Last edited:
 *   19/11/2020, 14:29:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file defines the tree used to represent a parsed ADL file.
**/

#include <algorithm>

#include "ADLTree.hpp"
#include "Exceptions.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLTREE CLASS *****/

/* Constructor for the ADLTree, which doesn't take anything! */
ADLTree::ADLTree() :
    ADLNode(NodeType::root, {}, 0, 0, nullptr)
{}

/* Copy constructor for the ADLTree. */
ADLTree::ADLTree(const ADLTree& other) :
    ADLNode(other)
{
    // Reserve enough space in our vector
    this->files.reserve(other.size());

    // Copy all individual nodes
    for (size_t i = 0; i < other.files.size(); i++) {
        this->files.push_back(other.files[i]->copy());
    }
}

/* Move constructor for the ADLTree. */
ADLTree::ADLTree(ADLTree&& other) :
    ADLNode(other),
    files(std::move(other.files))
{
    // Clear out the other's list of files
    other.files.clear();
}

/* Virtual destructor for the ADLTree. */
ADLTree::~ADLTree() {
    for (size_t i = 0; i < this->files.size(); i++) {
        delete this->files[i];
    }
}



/* Removes a given node as direct child of this ADLFile. */
void ADLTree::remove_node(const ADLFile& node) {
    const char* context = "ADLTree::remove_node()";

    // Find the place of the node
    std::vector<ADLFile*>::iterator iter = std::find(this->files.begin(), this->files.end(), &node);
    if (iter == this->files.end()) { throw std::runtime_error(std::string(context) + ": Unknown node encountered.\n"); }

    // Delete the pointer
    delete (*iter);

    // Use the iterator to remove it from the list
    this->files.erase(iter);
}



/* Traverses the tree, and calls the given function when any of the given types (OR'ed together) is encountered. The return pointer of your function will replace the current node pointer before traversing deeper.*/
void ADLTree::traverse(NodeType node_types, ADLNode* (*trav_func)(ADLNode*)) {
    // Call ADLNode's helper with files (casted to ADLNode's)
    this->_traverse(std::vector<ADLNode*>(this->files.begin(), this->files.end()), node_types, trav_func);
}

/* Traverses the tree, and calls the given function when any of the given types (OR'ed together) is encountered. The argument to the function is the current node and the argument that we given at the start and may be used to retain a state. The return pointer of your function will replace the current node pointer before traversing deeper. */
void ADLTree::traverse(NodeType node_types, ADLNode* (*trav_func)(ADLNode*, std::any&), std::any& state) {
    // Call ADLNode's helper with files (casted to ADLNode's)
    this->_traverse(std::vector<ADLNode*>(this->files.begin(), this->files.end()), node_types, trav_func, state);
}



/* Merge this ADLTree with another one. */
ADLTree& ADLTree::operator+=(const ADLTree& other) {
    // Simply copy all files from that tree to this one
    for (size_t i = 0; i < other.files.size(); i++) {
        this->files.push_back(other.files[i]->copy());
    }

    // Return ourselves
    return *this;
}

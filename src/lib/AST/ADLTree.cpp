/* ADLTREE.cpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:44:56 PM
 * Last edited:
 *   24/11/2020, 23:08:59
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



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLTree::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    const char* context = "ADLTree::_traverse_recurse(stateless)";

    // Simply loop through all nested elements, possibly replacing them
    for (size_t i = 0; i < this->files.size(); i++) {
        ADLFile* new_node = (ADLFile*) this->files[i]->traverse(trav_id, node_types, trav_func);

        // Replace it if it changed
        if (new_node != this->files[i]) {
            // Sanity check that the node is still of a legal type
            if (new_node->type != NodeType::file) {
                throw std::runtime_error(std::string(context) + ": " + trav_id + ": ADLTree cannot accept nodes of type " + nodetype_name.at(new_node->type) + " (was File), cannot continue.\n");
            }
            
            // If so, continue with swapping it out
            delete this->files[i];
            this->files[i] = new_node;
        }
    }
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLTree::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    const char* context = "ADLTree::_traverse_recurse()";

    // Simply loop through all nested elements, possibly replacing them
    for (size_t i = 0; i < this->files.size(); i++) {
        ADLFile* new_node = (ADLFile*) this->files[i]->traverse(trav_id, node_types, trav_func, state);

        // Replace it if it changed
        if (new_node != this->files[i]) {
            // Sanity check that the node is still of a legal type
            if (new_node->type != NodeType::file) {
                throw std::runtime_error(std::string(context) + ": " + trav_id + ": ADLTree cannot accept nodes of type " + nodetype_name.at(new_node->type) + " (was File), cannot continue.\n");
            }
            
            // If so, continue with swapping it out
            delete this->files[i];
            this->files[i] = new_node;
        }
    }
}



/* Removes a given node as direct child of this ADLFile. */
void ADLTree::remove_node(ADLFile* node) {
    const char* context = "ADLTree::remove_node()";

    // Find the place of the node
    std::vector<ADLFile*>::iterator iter = std::find(this->files.begin(), this->files.end(), node);
    if (iter == this->files.end()) { throw std::runtime_error(std::string(context) + ": Unknown node encountered.\n"); }

    // Delete the pointer
    delete (*iter);

    // Use the iterator to remove it from the list
    this->files.erase(iter);
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



/* Allows the ADLTree to be copied polymorphically. */
ADLTree* ADLTree::copy() const { return new ADLTree(*this); }

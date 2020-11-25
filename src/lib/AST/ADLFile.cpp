/* ADLFILE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:47:57
 * Last edited:
 *   25/11/2020, 20:35:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include <algorithm>

#include "ADLFile.hpp"
#include "Exceptions.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLFILE CLASS *****/

/* Constructor for the ADLFile class, which only takes a trail of filenames where it originated. */
ADLFile::ADLFile(const std::vector<std::string>& filenames) :
    ADLNode(NodeType::file, filenames, 0, 0)
{}

/* Copy constructor for the ADLFile class. */
ADLFile::ADLFile(const ADLFile& other) :
    ADLNode(other)
{
    // Copy all the children nodes from the other
    this->toplevel.reserve(other.toplevel.size());
    for (size_t i = 0; i < other.toplevel.size(); i++) {
        this->toplevel.push_back(other.toplevel[i]->copy());
    }
}

/* Move constructor for the ADLFile class. */
ADLFile::ADLFile(ADLFile&& other) :
    ADLNode(other),
    toplevel(other.toplevel)
{
    // Clear out the vector
    other.toplevel.empty();
}

/* Destructor for the ADLFile class. */
ADLFile::~ADLFile() {
    // Delete every pointer we have
    for (size_t i = 0; i < this->toplevel.size(); i++) {
        delete this->toplevel[i];
    }
}



/* Dynamically adds another nested node to the list of toplevel-nodes. */
void ADLFile::add_node(ADLNode* node) {
    // Add it to the internal list
    this->toplevel.push_back(node);
    // Don't forget to set ourselves as a parent
    node->parent = this;
}



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLFile::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    // Simply loop through all nested elements, possibly replacing them
    for (size_t i = 0; i < this->toplevel.size(); i++) {
        ADLNode* new_node = this->toplevel[i]->traverse(trav_id, node_types, trav_func);

        // Replace it if it changed
        if (new_node != this->toplevel[i]) {
            delete this->toplevel[i];
            this->toplevel[i] = new_node;
        }
    }
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLFile::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    // Simply loop through all nested elements, possibly replacing them
    for (size_t i = 0; i < this->toplevel.size(); i++) {
        ADLNode* new_node = this->toplevel[i]->traverse(trav_id, node_types, trav_func, state);

        // Replace it if it changed
        if (new_node != this->toplevel[i]) {
            delete this->toplevel[i];
            this->toplevel[i] = new_node;
        }
    }
}



/* Allows the ADLFile to be copied polymorphically. */
ADLFile* ADLFile::copy() const { return new ADLFile(*this); }

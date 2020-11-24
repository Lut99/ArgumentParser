/* ADLFILE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:47:57
 * Last edited:
 *   24/11/2020, 22:44:59
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

/* Constructor for the ADLFile class, which takes a trail of filenames where it originated, a column number where it originated, a matching column number and the parent of this node. */
ADLFile::ADLFile(const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent) :
    ADLNode(NodeType::file, filenames, line, col, parent)
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



/* Removes a given node as direct child of this ADLFile. */
void ADLFile::remove_node(ADLNode* node) {
    const char* context = "ADLFile::remove_node()";

    // Find the place of the node
    std::vector<ADLNode*>::iterator iter = std::find(this->toplevel.begin(), this->toplevel.end(), node);
    if (iter == this->toplevel.end()) { throw std::runtime_error(std::string(context) + ": Unknown node encountered.\n"); }

    // Delete the pointer
    delete (*iter);

    // Use the iterator to remove it from the list
    this->toplevel.erase(iter);
}



/* Allows the ADLFile to be copied polymorphically. */
ADLFile* ADLFile::copy() const { return new ADLFile(*this); }

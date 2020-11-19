/* ADLFILE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:47:57
 * Last edited:
 *   19/11/2020, 14:24:06
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



/* Updates the node with the given pointer internally with a new one, returned by the traversal function. */
void ADLFile::_traverse_update(ADLNode* old_node, ADLNode* new_node) {
    // Find the correct node in the list of nodes
    for (size_t i = 0; i < this->toplevel.size(); i++) {
        if (this->toplevel[i] == old_node) {
            // We found the old node, so delete it, set it to old node and we're done
            delete this->toplevel[i];
            this->toplevel[i] = new_node;
            return;
        }
    }
}



/* Removes a given node as direct child of this ADLFile. */
void ADLFile::remove_node(const ADLNode& node) {
    const char* context = "ADLFile::remove_node()";

    // Find the place of the node
    std::vector<ADLNode*>::iterator iter = std::find(this->toplevel.begin(), this->toplevel.end(), &node);
    if (iter == this->toplevel.end()) { throw std::runtime_error(std::string(context) + ": Unknown node encountered.\n"); }

    // Delete the pointer
    delete (*iter);

    // Use the iterator to remove it from the list
    this->toplevel.erase(iter);
}



/* Traverses the ADLFile and it's children. Calls the trav_func on any node with a type occuring in nodes. */
void ADLFile::traverse(NodeType node_types, ADLNode* (*trav_func)(ADLNode*)) {
    // Call the helper function with the correct list of nodes
    this->_traverse(this->toplevel, node_types, trav_func);
}

/* Traverses the ADLFile and it's children. Calls the trav_func on any node with a type occuring in nodes. Additionally, provides the option to retain a state between calls of trav_func in the form of an any-wrapper. */
void ADLFile::traverse(NodeType node_types, ADLNode* (*trav_func)(ADLNode*, std::any&), std::any& state) {
    // Call the helper function with the correct list of nodes
    this->_traverse(this->toplevel, node_types, trav_func, state);
}



/* Allows the ADLFile to be copied polymorphically. */
ADLFile* ADLFile::copy() const { return new ADLFile(*this); }

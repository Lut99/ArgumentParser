/* ADLTREE.cpp
 *   by Anonymous
 *
 * Created:
 *   11/12/2020, 5:44:56 PM
 * Last edited:
 *   18/11/2020, 21:22:52
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file defines the tree used to represent a parsed ADL file.
**/

#include "ADLTree.hpp"
#include "Exceptions.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLTREE CLASS, a.k.a., ROOT NODE *****/

/* Copy constructor for the ADLTree. */
ADLTree::ADLTree(const ADLTree& other) {
    // Reserve enough space in our vector
    this->files.reserve(other.size());

    // Copy all individual nodes
    for (size_t i = 0; i < other.files.size(); i++) {
        this->files.push_back(other.files[i]->copy());
    }
}

/* Move constructor for the ADLTree. */
ADLTree::ADLTree(ADLTree&& other) :
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



/* Traverses the tree, and calls the given function when any of the given types (OR'ed together) is encountered. The return pointer of your function will replace the current node pointer before traversing deeper.*/
void ADLTree::traverse(NodeType nodes, ADLNode* (*trav_func)(ADLNode*)) {
    const char* context = "ADLTree::traverse()";

    /* For each of the files, check if we should call trav_func, and else traverse a layer deeper. */
    for (size_t i = 0; i < this->files.size(); i++) {
        // If 'file' is in the nodes, then call the trav_func since these are all files
        if ((nodetype_t) nodes & (nodetype_t) NodeType::file) {
            // Call the traversal function
            ADLNode* new_node = trav_func((ADLNode*) this->files[i]);

            // If it differs from the node we gave it, try to update it
            if (new_node != (ADLNode*) this->files[i]) {
                // Check if it's still a file
                if (new_node->get_type() != NodeType::file) { throw Exceptions::IllegalNewNodeException(context, NodeType::file, new_node->get_type()); }

                // It is, so deallocate the old one and replace it with the new one
                delete this->files[i];
                this->files[i] = (ADLFile*) new_node;
            }
        }

        // In any case, traverse deeper by calling the traverse on the node itself
        this->files[i]->traverse(nodes, trav_func);
    }
}

/* Traverses the tree, and calls the given function when any of the given types (OR'ed together) is encountered. The argument to the function is the current node and the argument that we given at the start and may be used to retain a state. The return pointer of your function will replace the current node pointer before traversing deeper. */
void ADLTree::traverse(NodeType nodes, ADLNode* (*trav_func)(ADLNode*, std::any&), std::any& traverse_value) {
    const char* context = "ADLTree::traverse(value)";

    /* For each of the files, check if we should call trav_func, and else traverse a layer deeper. */
    for (size_t i = 0; i < this->files.size(); i++) {
        // If 'file' is in the nodes, then call the trav_func since these are all files
        if ((nodetype_t) nodes & (nodetype_t) NodeType::file) {
            // Call the traversal function
            ADLNode* new_node = trav_func((ADLNode*) this->files[i], traverse_value);

            // If it differs from the node we gave it, try to update it
            if (new_node != (ADLNode*) this->files[i]) {
                // Check if it's still a file
                if (new_node->get_type() != NodeType::file) { throw Exceptions::IllegalNewNodeException(context, NodeType::file, new_node->get_type()); }

                // It is, so deallocate the old one and replace it with the new one
                delete this->files[i];
                this->files[i] = (ADLFile*) new_node;
            }
        }

        // In any case, traverse deeper by calling the traverse on the node itself
        this->files[i]->traverse(nodes, trav_func, traverse_value);
    }
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



/* Move assignment operator for the ADLTree class. */
ADLTree& ADLTree::operator=(ADLTree&& other) {
    if (this != &other) { swap(*this, other); }
    return *this;
}

/* Swap operator for the ADLTree class. */
void ArgumentParser::swap(ADLTree& t1, ADLTree& t2) {
    using std::swap;

    swap(t1.files, t2.files);
}

/* ADLNODE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:39:22
 * Last edited:
 *   19/11/2020, 14:14:09
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for all nodes in the ADL Abstract Syntax Tree (AST).
**/

#include "ADLNode.hpp"
#include "Exceptions.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLNODE CLASS *****/

/* Constructor for the ADLNode, which takes a type and a parent node. */
ADLNode::ADLNode(NodeType type, const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent) :
    type(type),
    filenames(filenames),
    line(line),
    col(col),
    parent(parent)
{}



/* Actual traversal-function for stateless traversal. Takes a list of nodes to traverse (in order), which nodes we should trigger the traversal function and the (stateless) traversal function itself. */
void ADLNode::_traverse(const std::vector<ADLNode*>& to_traverse, NodeType node_types, ADLNode* (*trav_func)(ADLNode*)) {
    /* For each of the files, check if we should call trav_func, and else traverse a layer deeper. */
    for (size_t i = 0; i < to_traverse.size(); i++) {
        // If 'file' is in the nodes, then call the trav_func since these are all files
        if ((nodetype_t) to_traverse[i]->type & (nodetype_t) node_types) {
            // Call the traversal function
            ADLNode* new_node = trav_func(to_traverse[i]);

            // If it differs from the node we gave it, try to update it
            if (new_node != to_traverse[i]) {
                // It is, so deallocate the old one and replace it with the new one
                this->_traverse_update(to_traverse[i], new_node);
            }
        }

        // In any case, traverse deeper by calling the traverse on the node itself
        to_traverse[i]->traverse(node_types, trav_func);
    }
}

/* Actual traversal-function for traversal. Takes a list of nodes to traverse (in order), which nodes we should trigger the traversal function, the traversal function itself and the initial state. */
void ADLNode::_traverse(const std::vector<ADLNode*>& to_traverse, NodeType node_types, ADLNode* (*trav_func)(ADLNode*, std::any&), std::any& state) {
    /* For each of the files, check if we should call trav_func, and else traverse a layer deeper. */
    for (size_t i = 0; i < to_traverse.size(); i++) {
        // If 'file' is in the nodes, then call the trav_func since these are all files
        if ((nodetype_t) to_traverse[i]->type & (nodetype_t) node_types) {
            // Call the traversal function
            ADLNode* new_node = trav_func(to_traverse[i], state);

            // If it differs from the node we gave it, try to update it
            if (new_node != to_traverse[i]) {
                // It is, so deallocate the old one and replace it with the new one
                this->_traverse_update(to_traverse[i], new_node);
            }
        }

        // In any case, traverse deeper by calling the traverse on the node itself
        to_traverse[i]->traverse(node_types, trav_func, state);
    }
}

/* ADLNODE.cpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:39:22
 * Last edited:
 *   24/11/2020, 23:12:32
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
ADLNode* ADLNode::traverse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    // Only call the traversal function on this node if we're of the target type
    if (this->type & node_types) {
        // Note that in this case, we leave it to the traversal function to actually recurse deeper (so it may choose the timing)
        return trav_func(trav_id, this);
    } else {
        // Default to the normal recursion
        this->_traverse_recurse(trav_id, node_types, trav_func);
    }
    
    // Otherwise, just return ourselves
    return this;
}

/* Actual traversal-function for traversal. Takes a list of nodes to traverse (in order), which nodes we should trigger the traversal function, the traversal function itself and the initial state. */
ADLNode* ADLNode::traverse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    // Only call the traversal function on this node if we're of the target type
    if (this->type & node_types) {
        // Note that in this case, we leave it to the traversal function to actually recurse deeper (so it may choose the timing)
        return trav_func(trav_id, this, state);
    } else {
        // Default to the normal recursion
        this->_traverse_recurse(trav_id, node_types, trav_func, state);
    }
    
    // Otherwise, just return ourselves
    return this;
}

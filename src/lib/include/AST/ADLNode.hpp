/* ADLNODE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:37:00
 * Last edited:
 *   25/11/2020, 17:14:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for all nodes in the ADL Abstract Syntax Tree (AST).
**/

#ifndef ADL_NODE_HPP
#define ADL_NODE_HPP

#include <any>
#include <vector>
#include <string>

#include "NodeType.hpp"

namespace ArgumentParser {
    /* Baseclass for all nodes in the tree. */
    class ADLNode {
    protected:
        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) = 0;
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) = 0;

    public:
        /* The type of the node. */
        const NodeType type;
        /* The breadcrumb of files where the node was parsed from. */
        const std::vector<std::string> filenames;
        /* The line where this node was parsed from. */
        const size_t line;
        /* The column where this node was parsed from. */
        const size_t col;

        /* The parent node of this node. */
        ADLNode* parent;


        /* Constructor for the ADLNode, which takes the type of the node, a breadcrumb of filenames where the node originates, the line number where the node originates, a matching column number and the parent node in the tree. */
        ADLNode(NodeType type, const std::vector<std::string>& filenames, size_t line, size_t col);
        /* Virtual destructor for the ADLNode class, which doesn't do a lot yet. */
        virtual ~ADLNode() = default;

        /* Traverses through the tree and calls the given traversal function for any node that matches any of the given node types. The trav_id is used for debugging purposes, to help identify which traversal went wrong. This particular overlead does not carry states between trav_func calls. */
        ADLNode* traverse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Traverses through the tree and calls the given traversal function for any node that matches any of the given node types. The trav_id is used for debugging purposes, to help identify which traversal went wrong. This particular overlead carries a state between trav_func calls, which is initialized with the given value (as std::any). */
        ADLNode* traverse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);

        /* Allows the ADLNode to be copied polymorphically. */
        virtual ADLNode* copy() const = 0;

    };
}

#endif

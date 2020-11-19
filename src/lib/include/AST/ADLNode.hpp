/* ADLNODE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:37:00
 * Last edited:
 *   19/11/2020, 14:13:41
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
        /* Updates the node with the given pointer internally with a new one, returned by the traversal function. */
        virtual void _traverse_update(ADLNode* old_node, ADLNode* new_node) = 0;
        /* Actual traversal-function for stateless traversal. Takes the context where it is called from, a list of nodes to traverse (in order), which nodes we should trigger the traversal function and the (stateless) traversal function itself. */
        void _traverse(const std::vector<ADLNode*>& to_traverse, NodeType node_types, ADLNode* (*trav_func)(ADLNode*));
        /* Actual traversal-function for traversal. Takes the context where it is called from, a list of nodes to traverse (in order), which nodes we should trigger the traversal function, the traversal function itself and the initial state. */
        void _traverse(const std::vector<ADLNode*>& to_traverse, NodeType node_types, ADLNode* (*trav_func)(ADLNode*, std::any&), std::any& state);
        
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

    public:
        /* Constructor for the ADLNode, which takes the type of the node, a breadcrumb of filenames where the node originates, the line number where the node originates, a matching column number and the parent node in the tree. */
        ADLNode(NodeType type, const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent);
        /* Virtual destructor for the ADLNode class, which doesn't do a lot yet. */
        virtual ~ADLNode() = default;

        /* Virtual function that allows the ADLNode to be traversed polymorphically. */
        virtual void traverse(NodeType node_types, ADLNode* (*trav_func)(ADLNode*)) = 0;
        /* Virtual function that allows the ADLNode to be traversed polymorphically. */
        virtual void traverse(NodeType node_types, ADLNode* (*trav_func)(ADLNode*, std::any&), std::any& state) = 0;

        /* Allows the ADLNode to be copied polymorphically. */
        virtual ADLNode* copy() const = 0;

    };
}

#endif

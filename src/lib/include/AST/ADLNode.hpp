/* ADLNODE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:37:00
 * Last edited:
 *   04/01/2021, 17:49:51
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
#include <ostream>

#include "DebugInfo.hpp"
#include "NodeType.hpp"
#include "WarningTypes.hpp"

namespace ArgumentParser {
    /* Baseclass for all nodes in the tree. */
    class ADLNode {
    public:
        /* The type of the node. */
        const NodeType type;
        /* The debug information noting where this node originates from. */
        DebugInfo debug;
        /* A list of warnings that are suppressed for this node. */
        Exceptions::WarningType suppressed;

        /* The parent node of this node. */
        ADLNode* parent;


        /* Constructor for the ADLNode, which takes the type of the node and the debug information of where this nodes originates. Note that it automatically assings all currently suppressed tokens in the static error handler as suppressed for this node. */
        ADLNode(NodeType type, const DebugInfo& debug);
        /* Virtual destructor for the ADLNode class, which doesn't do a lot yet. */
        virtual ~ADLNode() = default;

        /* Traverses through the tree and calls the given traversal function for any node that matches any of the given node types. The trav_id is used for debugging purposes, to help identify which traversal went wrong. This particular overlead does not carry states between trav_func calls. */
        ADLNode* traverse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Traverses through the tree and calls the given traversal function for any node that matches any of the given node types. The trav_id is used for debugging purposes, to help identify which traversal went wrong. This particular overlead carries a state between trav_func calls, which is initialized with the given value (as std::any). */
        ADLNode* traverse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, void*), void* state);

        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) = 0;
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, void*), void* state) = 0;

        /* Virtual function to let the Node (and its children, if any) print itself to the given oustream. Note that this will take multiple lines, and should reflect the structure as defined by the AST. */
        virtual std::ostream& print(std::ostream& os) const = 0;

        /* Allows the ADLNode to be copied polymorphically. */
        virtual ADLNode* copy() const = 0;

    };
}

#endif

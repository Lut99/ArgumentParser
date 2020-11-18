/* ADLNODE.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 20:37:00
 * Last edited:
 *   18/11/2020, 20:42:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Baseclass for all nodes in the ADL Abstract Syntax Tree (AST).
**/

#ifndef ADL_NODE_HPP
#define ADL_NODE_HPP

#include <any>

#include "NodeType.hpp"

namespace ArgumentParser {
    /* Baseclass for all nodes in the tree. */
    class ADLNode {
    private:
        /* The type of the node. */
        NodeType type;

    public:
        /* The parent node of this node. */
        ADLNode* parent;

        /* Constructor for the ADLNode, which takes a type and a parent node. */
        ADLNode(NodeType type, ADLNode* parent);
        /* Virtual destructor for the ADLNode class, which doesn't do a lot yet. */
        virtual ~ADLNode() = default;

        /* Virtual function that allows the ADLNode to be traversed polymorphically. */
        virtual void traverse(NodeType nodes, ADLNode* (*trav_func)(ADLNode*)) = 0;
        /* Virtual function that allows the ADLNode to be traversed polymorphically. */
        virtual void traverse(NodeType nodes, ADLNode* (*trav_func)(ADLNode*, std::any&), std::any& traverse_value) = 0;

        /* Returns the type of this node. */
        inline NodeType get_type() const { return this->type; }

        /* Allows the ADLNode to be copied polymorphically. */
        virtual ADLNode* copy() const = 0;

        /* Define the swap operator for the ADLNode so children may use it. */
        friend void swap(ADLNode& n1, ADLNode& n2);

    };
    /* Define the swap operator for the ADLNode so children may use it. */
    void swap(ADLNode& n1, ADLNode& n2);
}

#endif

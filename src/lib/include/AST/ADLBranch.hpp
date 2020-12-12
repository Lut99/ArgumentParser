/* ADLBRANCH.hpp
 *   by Lut99
 *
 * Created:
 *   26/11/2020, 11:36:56
 * Last edited:
 *   12/12/2020, 17:29:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLBranch class, which is used as a base for
 *   all nodes that have children themselves. This way, memory management
 *   and traversal is centralized, where the nodes can define access to the
 *   children array using inline functions or references.
**/

#ifndef ADL_BRANCH_HPP
#define ADL_BRANCH_HPP

#include <string>
#include <vector>

#include "ADLNode.hpp"
#include "NodeType.hpp"
#include "WarningTypes.hpp"

namespace ArgumentParser {
    /* The ADLBranch class forms the baseclass for all nodes with children in the AST, centralizing memory management and traversal. */
    class ADLBranch: public ADLNode {
    protected:
        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);
    
    public:
        /* Indicates whether or not the number of children is bounded. */
        bool has_max;
        /* Denotes the maximum number of children this node can have if applicable. */
        size_t max;
        /* Lists the allowed children types of this node. */
        NodeType whitelist;
        /* Contains all children of this node. */
        std::vector<ADLNode*> children;
        /* Contains all suppressed warnings for this node. */
        std::vector<Exceptions::WarningType> suppressed;


        /* Constructor for the ADLBranch class, which takes the type of the derived node, a DebugInfo struct linking this node to a location in the source file and optionally a NodeType denoting all accepted children types. */
        ADLBranch(NodeType type, const DebugInfo& debug, NodeType whitelist = nt_all);
        /* Constructor for the ADLBranch class, which takes the type of the derived node, a DebugInfo struct linking this node to a location in the source file, the maximum number of children to allow and optionally a NodeType denoting all accepted children types. */
        ADLBranch(NodeType type, const DebugInfo& debug, size_t max, NodeType whitelist = nt_all);
        /* Copy constructor for the ADLBranch class. */
        ADLBranch(const ADLBranch& other);
        /* Move constructor for the ADLBranch class. */
        ADLBranch(ADLBranch&& other);
        /* Virtual destructor for the ADLBranch class. */
        virtual ~ADLBranch();

        /* Adds a node as child of this node. */
        void add_node(ADLNode* node);

        /* Returns all nodes (as vector) with the given type(s) stored as child of this node. */
        std::vector<ADLNode*> get_nodes(NodeType type) const;
        /* Returns the number of children currently in this node. */
        inline size_t size() const { return this->children.size(); }

    };
}

#endif

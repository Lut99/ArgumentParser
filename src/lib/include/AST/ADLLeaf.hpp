/* ADLLEAF.hpp
 *   by Lut99
 *
 * Created:
 *   26/11/2020, 11:26:44
 * Last edited:
 *   08/12/2020, 20:44:52
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLLeaf class, which is used as a basis for all
 *   nodes in the tree that do not have any children themselves.
**/

#ifndef ADL_LEAF_HPP
#define ADL_LEAF_HPP

#include <string>
#include <vector>

#include "ADLNode.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLLeaf class is baseclass for all leaf nodes in the AST of the ADL. */
    class ADLLeaf: public ADLNode {
    protected:
        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);

    public:
        /* Constructor for the ADLLeaf, which takes the type of the derived file and a DebugInfo struct containing the information where this node came from. */
        ADLLeaf(NodeType type, const DebugInfo& debug);
        /* Virtual destructor for the ADLLeaf class, which is just the default one but virtual. */
        virtual ~ADLLeaf() = default;

    };
};

#endif

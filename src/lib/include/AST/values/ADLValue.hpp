/* ADLVALUE.hpp
 *   by Lut99
 *
 * Created:
 *   24/11/2020, 22:59:43
 * Last edited:
 *   24/11/2020, 23:22:56
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the baseclass ADLValue, which is used to derive all
 *   values types from (strings, regex-expressions, numbers and decimals).
**/

#ifndef ADL_VALUE_HPP
#define ADL_VALUE_HPP

#include <string>
#include <vector>

#include "ADLNode.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLValue class is the baseclass for all value types in the tree. */
    class ADLValue : public ADLNode {
    private:
        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);
    
    public:
        /* Constructor for the ADLValue class, which takes the value type, a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from and this node's parent node. */
        ADLValue(NodeType type, const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent);

        /* Allows the ADLValue to be copied polymorphically. */
        virtual ADLValue* copy() const;

    };
}

#endif

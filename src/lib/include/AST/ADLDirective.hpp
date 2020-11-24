/* ADLDIRECTIVE.hpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:41:08
 * Last edited:
 *   24/11/2020, 22:46:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ADLDirective class, which is a special type of node that
 *   represents compiler directives. Will therefore likely be removed in
 *   one of the first AST traversals.
**/

#ifndef ADL_DIRECTIVE_HPP
#define ADL_DIRECTIVE_HPP

#include <cstdlib>
#include <string>
#include <vector>

#include "NodeType.hpp"
#include "ADLNode.hpp"
#include "ADLValue.hpp"

namespace ArgumentParser {
    /* The ADLDirective class is used to temporarily store compiler directives before they are processed. */
    class ADLDirective : public ADLNode {
    private:
        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);

    public:
        /* The directive that this nodes represents (i.e., the 'X' in '.X <values>'). */
        std::string directive;
        /* The values given to this directive. */
        std::vector<ADLValue*> values;

        /* Constructor for the ADLDirective class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from, this node's parent node, the directive itself and a list of arguments as Value nodes. */
        ADLDirective(const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent, const std::string& directive, const std::vector<ADLValue*>& arguments);
        /* Copy constructor for the ADLDirective class. */
        ADLDirective(const ADLDirective& other);
        /* Move constructor for the ADLDirective class. */
        ADLDirective(ADLDirective&& other);
        /* Destructor for the ADLDirective class. */
        ~ADLDirective();

    };
}

#endif

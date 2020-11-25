/* ADLVALUES.hpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 18:11:33
 * Last edited:
 *   25/11/2020, 20:37:13
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a class that is used to group multiple, consecutive
 *   ADLValue symbols into one list of them.
**/

#ifndef ADL_VALUES_HPP
#define ADL_VALUES_HPP

#include <string>
#include <vector>

#include "ADLNode.hpp"
#include "ADLValue.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLValues class is used to group ADLValue symbols. */
    class ADLValues: public ADLNode {
    protected:
        /* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*));
        /* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
        virtual void _traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state);

    public:
        /* List of values stored in this ADLValue. */
        std::vector<ADLValue*> values;

        /* Constructor for the ADLValues class, which takes a trail of breadcrumbs for files we're parsing, the line number where the symbol originated and a matching column number. */
        ADLValues(const std::vector<std::string>& filenames, size_t line, size_t col);
        /* Copy constructor for the ADLValues class. */
        ADLValues(const ADLValues& other);
        /* Move constructor for the ADLValues class. */
        ADLValues(ADLValues&& other);
        /* Destructor for the ADLValues class. */
        virtual ~ADLValues();

        /* Dynamically adds another value node to the list of values. */
        void add_value(ADLValue* value);

        /* Allows the ADLValues to be copied polymorphically. */
        virtual ADLValues* copy() const;

    };
};

#endif

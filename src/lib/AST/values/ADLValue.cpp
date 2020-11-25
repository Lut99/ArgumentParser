/* ADLVALUE.cpp
 *   by Lut99
 *
 * Created:
 *   24/11/2020, 23:00:48
 * Last edited:
 *   25/11/2020, 17:20:21
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the baseclass ADLValue, which is used to derive all
 *   values types from (strings, regex-expressions, numbers and decimals).
**/

#include "ADLValue.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLVALUE CLASS *****/

/* Constructor for the ADLValue class, which takes the value type, a breadcrumb trail of filenames where this node originated from, the line number where this node originated from and the column number where it originated from. */
ADLValue::ADLValue(NodeType type, const std::vector<std::string>& filenames, size_t line, size_t col) :
    ADLNode(type, filenames, line, col)
{}



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLValue::_traverse_recurse(const char*, NodeType, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    // Nothing, since all values are end nodes
    return;
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLValue::_traverse_recurse(const char*, NodeType, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    // Nothing, since all values are end nodes
    return;
}



/* Allows the ADLValue to be copied polymorphically. */
ADLValue* ADLValue::copy() const { return new ADLValue(*this); }

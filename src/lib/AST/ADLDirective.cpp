/* ADLDIRECTIVE.cpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:40:37
 * Last edited:
 *   25/11/2020, 20:29:50
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ADLDirective class, which is a special type of node that
 *   represents compiler directives. Will therefore likely be removed in
 *   one of the first AST traversals.
**/

#include "ADLDirective.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLDIRECTIVE CLASS *****/

/* Constructor for the ADLDirective class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from, the directive itself and a list of values following it. */
ADLDirective::ADLDirective(const std::vector<std::string>& filenames, size_t line, size_t col, const std::string& directive, ADLValues* values) :
    ADLNode(NodeType::directive, filenames, line, col),
    directive(directive),
    values(values)
{}

/* Copy constructor for the ADLDirective class. */
ADLDirective::ADLDirective(const ADLDirective& other) :
    ADLNode(other),
    directive(other.directive)
{
    // Copy the values list
    this->values = other.values->copy();
}

/* Move constructor for the ADLDirective class. */
ADLDirective::ADLDirective(ADLDirective&& other) :
    ADLNode(other),
    directive(std::move(other.directive)),
    values(std::move(other.values))
{
    // Empty the other's value list, so the pointer doesn't get deallocated
    other.values = nullptr;
}

/* Destructor for the ADLDirective class. */
ADLDirective::~ADLDirective() {
    // Delete the values
    if (this->values != nullptr) { delete this->values; }
}



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLDirective::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    const char* context = "ADLDirective::_traverse_recurse(stateless)";

    // Call the traverse on the internal list of values
    ADLNode* new_node = this->values->traverse(trav_id, node_types, trav_func);

    // If the node changed, replace it
    if (new_node != (ADLNode*) this->values) {
        // Sanity check that the node is still of a legal type
        if (new_node->type == NodeType::values) {
            throw std::runtime_error(std::string(context) + ": " + trav_id + ": ADLDirective cannot accept nodes of type " + nodetype_name.at(new_node->type) + " (was Values), cannot continue.\n");
        }

        // If so, continue with swapping it out
        delete this->values;
        this->values = (ADLValues*) new_node;
    }
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLDirective::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    const char* context = "ADLDirective::_traverse_recurse()";

    // Call the traverse on the internal list of values
    ADLNode* new_node = this->values->traverse(trav_id, node_types, trav_func, state);

    // If the node changed, replace it
    if (new_node != (ADLNode*) this->values) {
        // Sanity check that the node is still of a legal type
        if (new_node->type == NodeType::values) {
            throw std::runtime_error(std::string(context) + ": " + trav_id + ": ADLDirective cannot accept nodes of type " + nodetype_name.at(new_node->type) + " (was Values), cannot continue.\n");
        }

        // If so, continue with swapping it out
        delete this->values;
        this->values = (ADLValues*) new_node;
    }
}



/* Allows the ADLDirective to be copied polymorphically. */
ADLDirective* ADLDirective::copy() const { return new ADLDirective(*this); }

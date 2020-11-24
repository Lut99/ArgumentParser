/* ADLDIRECTIVE.cpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:40:37
 * Last edited:
 *   24/11/2020, 23:01:24
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

/* Constructor for the ADLDirective class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from, this node's parent node, the directive itself and a list of arguments as Value nodes. */
ADLDirective::ADLDirective(const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent, const std::string& directive, const std::vector<ADLValue*>& arguments) :
    ADLNode(NodeType::directive, filenames, line, col, parent),
    directive(directive),
    arguments(arguments)
{}

/* Copy constructor for the ADLDirective class. */
ADLDirective::ADLDirective(const ADLDirective& other) :
    directive(other.directive)
{
    // Copy the elements one-by-one
    this->values.resize(other.values.size());
    for (size_t i = 0; i < other.values.size(); i++) {
        this->values.push_back(other.values[i]->copy());
    }
}

/* Move constructor for the ADLDirective class. */
ADLDirective::ADLDirective(ADLDirective&& other) :
    directive(std::move(other.directive)),
    values(std::move(other.values))
{
    // Empty the other's value list, so the pointers don't get deallocated
    other.values.clear();
}

/* Destructor for the ADLDirective class. */
ADLDirective::~ADLDirective() {
    // Delete the values
    for (size_t i = 0; i < this->values.size(); i++) {
        delete this->values[i];
    }
}



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLDirective::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    const char* context = "ADLDirective::_traverse_recurse(stateless)";

    // Simply loop through all nested elements, possibly replacing them
    for (size_t i = 0; i < this->files.size(); i++) {
        ADLFile* new_node = (ADLFile*) this->files[i]->traverse(trav_id, node_types, trav_func);

        // Replace it if it changed
        if (new_node != this->files[i]) {
            // Sanity check that the node is still of a legal type
            if ((nodetype_t) new_node->type & ArgumentParser::values) {
                throw std::runtime_error(std::string(context) + ": " + trav_id + ": ADLDirective cannot accept nodes of type " + nodetype_name.at(new_node->type) + " (was " + nodetype_name.at(this->files[i]->type) + "), cannot continue.\n");
            }
            
            // If so, continue with swapping it out
            delete this->files[i];
            this->files[i] = new_node;
        }
    }
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLDirective::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    const char* context = "ADLDirective::_traverse_recurse()";

    // Simply loop through all nested elements, possibly replacing them
    for (size_t i = 0; i < this->files.size(); i++) {
        ADLFile* new_node = (ADLFile*) this->files[i]->traverse(trav_id, node_types, trav_func, state);

        // Replace it if it changed
        if (new_node != this->files[i]) {
            // Sanity check that the node is still of a legal type
            if ((nodetype_t) new_node->type & ArgumentParser::values) {
                throw std::runtime_error(std::string(context) + ": " + trav_id + ": ADLTree cannot accept nodes of type " + nodetype_name.at(new_node->type) + " (was " + nodetype_name.at(this->files[i]->type) + "), cannot continue.\n");
            }
            
            // If so, continue with swapping it out
            delete this->files[i];
            this->files[i] = new_node;
        }
    }
}

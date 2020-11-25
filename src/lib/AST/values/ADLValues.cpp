/* ADLVALUES.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 18:11:08
 * Last edited:
 *   25/11/2020, 20:39:50
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a class that is used to group multiple, consecutive
 *   ADLValue symbols into one list of them.
**/

#include "ADLValues.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLVALUES CLASS *****/

/* Constructor for the ADLValues class, which takes a trail of breadcrumbs for files we're parsing, the line number where the symbol originated and a matching column number.*/
ADLValues::ADLValues(const std::vector<std::string>& filenames, size_t line, size_t col) :
    ADLNode(NodeType::values, filenames, line, col)
{}

/* Copy constructor for the ADLValues class. */
ADLValues::ADLValues(const ADLValues& other) :
    ADLNode(other)
{
    // Copy the list of values
    this->values.reserve(other.values.size());
    for (size_t i = 0; i < other.values.size(); i++) {
        this->values.push_back(other.values[i]->copy());
    }
}

/* Move constructor for the ADLValues class. */
ADLValues::ADLValues(ADLValues&& other) :
    ADLNode(other),
    values(std::move(other.values))
{
    // Don't forget to clear the other array to prevent it from deallocating our nodes
    other.values.clear();
}

/* Destructor for the ADLValues class. */
ADLValues::~ADLValues() {
    // Loop & delete
    for (size_t i = 0; i < this->values.size(); i++) {
        delete this->values[i];
    }
}



/* Function that will recurse the (stateless) traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLValues::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*)) {
    const char* context = "ADLValues::_traverse_recurse(stateless)";

    // Simply loop through all nested elements, possibly replacing them
    for (size_t i = 0; i < this->values.size(); i++) {
        ADLNode* new_node = this->values[i]->traverse(trav_id, node_types, trav_func);

        // Replace it if it changed
        if (new_node != this->values[i]) {
            // Sanity check that the node is still of a legal type
            if (!(new_node->type & ArgumentParser::values)) {
                throw std::runtime_error(std::string(context) + ": " + trav_id + ": ADLValues cannot accept nodes of type " + nodetype_name.at(new_node->type) + " (was " + nodetype_name.at(this->values[i]->type) + "), cannot continue.\n");
            }
            
            delete this->values[i];
            this->values[i] = (ADLValue*) new_node;
        }
    }
}

/* Function that will recurse the traversal one layer deeper if the trav function needn't be called for this one. Note that this function may replace (and therefore deallocate) older nodes if it proves needed. */
void ADLValues::_traverse_recurse(const char* trav_id, NodeType node_types, ADLNode* (*trav_func)(const char*, ADLNode*, std::any&), std::any& state) {
    const char* context = "ADLValues::_traverse_recurse()";

    // Simply loop through all nested elements, possibly replacing them
    for (size_t i = 0; i < this->values.size(); i++) {
        ADLNode* new_node = this->values[i]->traverse(trav_id, node_types, trav_func, state);

        // Replace it if it changed
        if (new_node != this->values[i]) {
            // Sanity check that the node is still of a legal type
            if (!(new_node->type & ArgumentParser::values)) {
                throw std::runtime_error(std::string(context) + ": " + trav_id + ": ADLValues cannot accept nodes of type " + nodetype_name.at(new_node->type) + " (was " + nodetype_name.at(this->values[i]->type) + "), cannot continue.\n");
            }
            
            delete this->values[i];
            this->values[i] = (ADLValue*) new_node;
        }
    }
}



/* Dynamically adds another value node to the list of values. */
void ADLValues::add_value(ADLValue* value) {
    // Add it to the internal list
    this->values.push_back(value);
    // Don't forget to set ourselves as a parent
    value->parent = this;
}



/* Allows the ADLValues to be copied polymorphically. */
ADLValues* ADLValues::copy() const { return new ADLValues(*this); }

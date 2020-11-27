/* ADLVALUES.cpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 18:11:08
 * Last edited:
 *   27/11/2020, 14:55:57
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

/* Constructor for the ADLValues class, which takes a trail of breadcrumbs for files we're parsing, a DebugInfo struct linking this node to a location in the source file and at least one value. */
ADLValues::ADLValues(const std::vector<std::string>& filenames, const DebugInfo& debug, ADLNode* value) :
    ADLBranch(NodeType::values, filenames, debug, nt_values)
{
    // Immediately add the value
    this->add_node(value);
}



/* Prints each of the internal values to the given output stream, reflecting the AST structure. */
std::ostream& ADLValues::print(std::ostream& os) const {
    // Simply print all our children, separated by spaces
    for (size_t i = 0; i < this->children.size(); i++) {
        if (i > 0) { os << ' '; }
        this->children[i]->print(os);
    }
    // Return the os
    return os;
}



/* Allows the ADLValues to be copied polymorphically. */
ADLValues* ADLValues::copy() const { return new ADLValues(*this); }

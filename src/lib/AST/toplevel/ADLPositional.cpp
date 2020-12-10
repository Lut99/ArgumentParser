/* ADLPOSITIONAL.cpp
 *   by Lut99
 *
 * Created:
 *   12/1/2020, 5:55:59 PM
 * Last edited:
 *   10/12/2020, 17:23:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLPositional class, which is used to represent
 *   a Positional argument definition in the ADL's Abstract Syntax Tree.
**/

#include "ADLPositional.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLPOSITIONAL CLASS *****/

/* Constructor for the ADLPositional, which takes a DebugInfo struct linking this node to the source code, the identifier of this Positional, a list of types that make up this Positional as ADLTypes, whether or not this Positional is optional, whether or not this Positional is variadic and optionally a list of configuration parameters for this Positional as ADLConfigs. */
ADLPositional::ADLPositional(const DebugInfo& debug, ADLIdentifier* id, ADLTypes* types, bool is_optional, bool is_variadic, ADLConfigs* configs) :
    ADLBranch(NodeType::positional, debug, 3, NodeType::identifier | NodeType::types | NodeType::configs),
    optional(is_optional),
    variadic(is_variadic)
{
    // Add the identifier always
    this->add_node((ADLNode*) id);
    // Add the types always
    this->add_node((ADLNode*) types);
    // Only add the values if they are given
    if (configs != nullptr) { this->add_node((ADLNode*) configs); }
}

/* Prints thePositional definition and all its configuration parameters to the given output stream. */
std::ostream& ADLPositional::print(std::ostream& os) const {
    // Print the type definition header
    if (this->optional) { os << '['; this->children[0]->print(os) << "] "; }
    else { this->children[0]->print(os) << ' '; }
    this->children[1]->print(os);
    if (this->variadic) { os << "..."; }
    os << " {";

    // Print each of the configuration parameters, if any, preceded by a space
    if (this->children.size() == 3) {
        os << endl;
        this->children[2]->print(os);
    }
    
    // Close with a bracket and newline, then we're done
    return os << '}' << endl;
}

/* Allows the ADLPositional to be copied polymorphically. */
ADLPositional* ADLPositional::copy() const { return new ADLPositional(*this); }

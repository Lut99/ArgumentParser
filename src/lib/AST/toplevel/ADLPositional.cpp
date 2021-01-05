/* ADLPOSITIONAL.cpp
 *   by Lut99
 *
 * Created:
 *   12/1/2020, 5:55:59 PM
 * Last edited:
 *   05/01/2021, 13:59:51
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
    ADLDefinition(NodeType::positional, debug, 3, NodeType::identifier | NodeType::types | NodeType::configs, id, configs),
    optional(is_optional),
    variadic(is_variadic)
{
    // Add the types always
    this->add_node((ADLNode*) types);
}

/* Prints thePositional definition and all its configuration parameters to the given output stream. */
std::ostream& ADLPositional::print(std::ostream& os) const {
    // Print the type definition header
    if (this->optional) { os << '['; this->identifier->print(os) << "] "; }
    else { this->identifier->print(os) << ' '; }
    this->get_nodes(NodeType::types)[0]->print(os);
    if (this->variadic) { os << "..."; }
    os << " {";

    // Print each of the configuration parameters, if any, preceded by a space
    if (this->configs != nullptr) {
        os << endl;
        this->configs->print(os);
    }
    
    // Close with a bracket and newline, then we're done
    return os << '}' << endl;
}

/* Allows the ADLPositional to be copied polymorphically. */
ADLPositional* ADLPositional::copy() const { return new ADLPositional(*this); }

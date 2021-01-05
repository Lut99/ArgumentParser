/* ADLOPTION.cpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 16:15:55
 * Last edited:
 *   05/01/2021, 13:38:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "ADLOption.hpp"
#include "ParseExceptions.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLOPTION CLASS *****/

/* Constructor for the ADLOption class, which takes a DebugInfo struct linking this node to the source code, the shortlabel for this Option (empty to disable), the longlabel of this Option (empty to disable), an optional list of types that make up this Option as ADLTypes, optionally a list of configuration parameters for this Positional as ADLConfigs, whether or not this Option is optional, whether or not this Option's type may be omitted and whether or not this Positional is variadic. */
ADLOption::ADLOption(const DebugInfo& debug, ADLIdentifier* shortlabel, ADLIdentifier* longlabel, ADLTypes* types, ADLConfigs* configs, bool is_optional, bool type_is_optional, bool is_variadic) :
    ADLDefinition(NodeType::option, debug, 4, NodeType::identifier | NodeType::types | NodeType::configs, shortlabel, longlabel, configs),
    optional(is_optional),
    type_optional(type_is_optional),
    variadic(is_variadic)
{
    // Only add the types if they are given
    if (types != nullptr) { this->add_node((ADLNode*) types); }
}

/* Prints the Option definition and all its configuration parameters to the given output stream. */
std::ostream& ADLOption::print(std::ostream& os) const {
    // First, print the ids
    if (this->optional) { os << '['; }
    bool first = false;
    for (ADLNode* id : this->get_nodes(NodeType::identifier)) {
        if (first) { os << ' '; } else { first = true; }
        id->print(os);
    }
    if (this->optional) { os << ']'; }

    // Then, print the types (if any)
    std::vector<ADLNode*> nodes = this->get_nodes(NodeType::types);
    if (nodes.size() >= 1) {
        os << ' ';
        if (this->type_optional) { os << '['; }
        nodes[0]->print(os);
        if (this->variadic) { os << "..."; }
        if (this->type_optional) { os << ']'; }
    }

    // Print the starting bracket
    os << " {";

    // Print each of the configuration parameters, if any, preceded by a space
    nodes = this->get_nodes(NodeType::configs);
    if (nodes.size() >= 1) {
        os << endl;
        nodes[0]->print(os);
    }
    
    // Close with a bracket and newline, then we're done
    return os << '}' << endl;
}

/* Allows the ADLOption to be copied polymorphically. */
ADLOption* ADLOption::copy() const { return new ADLOption(*this); }

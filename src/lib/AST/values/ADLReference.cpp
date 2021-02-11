/* ADLREFERENCE.cpp
 *   by Lut99
 *
 * Created:
 *   30/11/2020, 17:49:49
 * Last edited:
 *   11/02/2021, 16:53:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLReference class is used to represent a reference to another
 *   parameter of the same or another type, positional or option.
**/

#include "ADLReference.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLREFERENCE CLASS *****/
/* Constructor for the ADLReference class, which takes debug information about the node's origin, the type, positional or option to reference, which of those type we reference, and the field to reference. */
ADLReference::ADLReference(const DebugInfo& debug, ADLIdentifier* definition, ADLIdentifier* property) :
    ADLBranch(NodeType::reference, debug, 2, NodeType::identifier),
    definition(definition),
    property(property)
{
    // Add the ID and identifier as our children
    this->add_node((ADLNode*) definition);
    this->add_node((ADLNode*) property);
}

/* Prints the reference to the given output stream. */
std::ostream& ADLReference::print(std::ostream& os) const {
    this->definition->print(os);
    os << '.';
    return this->property->print(os);
}

/* Allows the ADLReference to be copied polymorphically. */
ADLReference* ADLReference::copy() const { return new ADLReference(*this); }

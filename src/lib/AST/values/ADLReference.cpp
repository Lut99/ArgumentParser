/* ADLREFERENCE.cpp
 *   by Lut99
 *
 * Created:
 *   30/11/2020, 17:49:49
 * Last edited:
 *   12/12/2020, 17:00:13
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
ADLReference::ADLReference(const DebugInfo& debug, const std::string& id, IdentifierType reference_type, const std::string& property) :
    ADLLeaf(NodeType::reference, debug),
    id(id),
    type(reference_type),
    property(property)
{}

/* Prints the reference to the given output stream. */
std::ostream& ADLReference::print(std::ostream& os) const {
    return os << this->id << '.' << this->property;
}

/* Allows the ADLReference to be copied polymorphically. */
ADLReference* ADLReference::copy() const { return new ADLReference(*this); }

/* ADLREFERENCE.cpp
 *   by Lut99
 *
 * Created:
 *   30/11/2020, 17:49:49
 * Last edited:
 *   08/12/2020, 20:51:01
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
ADLReference::ADLReference(const DebugInfo& debug, const std::string& id, TokenType reference_type, const std::string& property) :
    ADLLeaf(NodeType::reference, debug),
    id(id),
    type(reference_type),
    property(property)
{}

/* Prints the reference to the given output stream. */
std::ostream& ADLReference::print(std::ostream& os) const {
    if (this->type == TokenType::shortlabel) { os << '-'; }
    else if (this->type == TokenType::longlabel) { os << "--"; }
    else if (this->type == TokenType::type) { os << '<'; }
    os << this->id;
    if (this->type == TokenType::type) { os << '>'; }
    return os << '.' << this->property;
}

/* Allows the ADLReference to be copied polymorphically. */
ADLReference* ADLReference::copy() const { return new ADLReference(*this); }

/* ADLREFERENCE.cpp
 *   by Lut99
 *
 * Created:
 *   30/11/2020, 17:49:49
 * Last edited:
 *   30/11/2020, 17:55:53
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

/* Constructor for the ADLReference class, which takes a breadcrumb trial of filenames, debug information about the node's origin, the type, positional or option to reference and the field to reference. */
ADLReference::ADLReference(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& id, const std::string& property) :
    ADLLeaf(NodeType::reference, filenames, debug),
    id(id),
    property(property)
{}

/* Prints the reference to the given output stream. */
std::ostream& ADLReference::print(std::ostream& os) const {
    return os << this->id << "." << this->property;
}

/* Allows the ADLReference to be copied polymorphically. */
ADLReference* ADLReference::copy() const { return new ADLReference(*this); }

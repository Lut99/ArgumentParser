/* ADLIDENTIFIER.cpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 16:34:15
 * Last edited:
 *   10/12/2020, 17:22:35
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file is used to implement the ADLIdentifier class, which is a
 *   wrapper around either a positional, option or type identifier. In
 *   particular, it not only stores the identifier and its type as an
 *   IdentifierType, but also debug information where it can be found
 *   (which is interesting for traversal debugging).
**/

#include "ADLIdentifier.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLIDENTIFIER CLASS *****/

/* Constructor for the ADLIdentifier class, which takes debugging information, the identifier value we wrap and the type of said identifier. */
ADLIdentifier::ADLIdentifier(const DebugInfo& debug, const std::string& identifier, IdentifierType identifier_type) :
    ADLLeaf(NodeType::identifier, debug),
    identifier(identifier),
    type(identifier_type)
{}

/* Prints the identifier's value to the given output stream, possible wrapped in <> or preceded by - or --. */
std::ostream& ADLIdentifier::print(std::ostream& os) const {
    switch (this->type) {
        case IdentifierType::meta:
        case IdentifierType::positional:
            // Just print the value
            return os << this->identifier;
        
        case IdentifierType::longlabel:
            // Longlabel, so secretly add in one extra slash
            os << '-';
            [[fallthrough]];
        case IdentifierType::shortlabel:
            // Preceed by a single slash
            return os << '-' << this->identifier;
        
        case IdentifierType::type:
            // Type definition, so wrap in <>
            return os << '<' << this->identifier << '>';
        
        default:
            // Do nothing
            return os;
    }
}

/* Allows the ADLIdentifier to be copied polymorphically. */
ADLIdentifier* ADLIdentifier::copy() const { return new ADLIdentifier(*this); }

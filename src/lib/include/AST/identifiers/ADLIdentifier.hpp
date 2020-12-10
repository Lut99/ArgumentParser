/* ADLIDENTIFIER.hpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 16:33:50
 * Last edited:
 *   10/12/2020, 16:41:03
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

#ifndef ADL_IDENTIFIER_HPP
#define ADL_IDENTIFIER_HPP

#include <string>
#include <ostream>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* Defines the possible identifiers that the ADLIdentifier can wrap. */
    enum class IdentifierType {
        meta,
        positional,
        shortlabel,
        longlabel,
        type 
    };

    /* The ADLIdentifier class is used to wrap the identifiers used in the ADL. */
    class ADLIdentifier: public ADLLeaf {
    public:
        /* The string value of the identifier. */
        std::string identifier;  
        /* The type of the identifier, as IdentifierType. */
        IdentifierType type;

        /* Constructor for the ADLIdentifier class, which takes debugging information, the identifier value we wrap and the type of said identifier. */
        ADLIdentifier(const DebugInfo& debug, const std::string& identifier, IdentifierType identifier_type);

        /* Prints the identifier's value to the given output stream, possible wrapped in <> or preceded by - or --. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLIdentifier to be copied polymorphically. */
        virtual ADLIdentifier* copy() const;

    };
}

#endif

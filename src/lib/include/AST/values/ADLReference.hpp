/* ADLREFERENCE.hpp
 *   by Lut99
 *
 * Created:
 *   30/11/2020, 17:49:26
 * Last edited:
 *   08/12/2020, 20:51:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLReference class is used to represent a reference to another
 *   parameter of the same or another type, positional or option.
**/

#ifndef ADL_REFERENCE_HPP
#define ADL_REFERENCE_HPP

#include <string>
#include <ostream>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"
#include "TokenTypes.hpp"

namespace ArgumentParser {
    /* The ADLReference class is used to represent a reference to another property. */
    class ADLReference: public ADLLeaf {
    public:
        /* The ID (either a type ID, positional ID or Option short/longlabel) we reference. */
        std::string id;
        /* The type (i.e., namespace) that we reference. */
        TokenType type;
        /* The field we reference in that type or argument. */
        std::string property;

        /* Constructor for the ADLReference class, which takes debug information about the node's origin, the type, positional or option to reference, which of those type we reference, and the field to reference. */
        ADLReference(const DebugInfo& debug, const std::string& id, TokenType reference_type, const std::string& property);

        /* Prints the reference to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLReference to be copied polymorphically. */
        virtual ADLReference* copy() const;

    };
}

#endif

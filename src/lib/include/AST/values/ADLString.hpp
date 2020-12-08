/* ADLSTRING.hpp
 *   by Lut99
 *
 * Created:
 *   24/11/2020, 23:19:31
 * Last edited:
 *   08/12/2020, 20:52:05
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLString class, which is used to represent raw
 *   string values in the Abstract Syntax Tree (AST) of the ADL. Derived from
 *   the ADLValue class.
**/

#ifndef ADL_STRING_HPP
#define ADL_STRING_HPP

#include <string>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLString class is used to represent raw string values. */
    class ADLString : public ADLLeaf {
    public:
        /* Raw value stored in this ADLString. */
        std::string value;

        /* Constructor for the ADLString class, which takes a DebugInfo struct linking this node to a location in the source file and the raw string value. */
        ADLString(const DebugInfo& debug, const std::string& value);

        /* Prints the string to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLString to be copied polymorphically. */
        virtual ADLString* copy() const;

    };
}

#endif

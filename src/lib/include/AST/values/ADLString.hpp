/* ADLSTRING.hpp
 *   by Lut99
 *
 * Created:
 *   24/11/2020, 23:19:31
 * Last edited:
 *   26/11/2020, 12:27:24
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
#include <vector>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLString class is used to represent raw string values. */
    class ADLString : public ADLLeaf {
    public:
        /* Raw value stored in this ADLString. */
        std::string value;

        /* Constructor for the ADLString class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from and the raw string value. */
        ADLString(const std::vector<std::string>& filenames, size_t line, size_t col, const std::string& value);

        /* Allows the ADLString to be copied polymorphically. */
        virtual ADLString* copy() const;

    };
}

#endif

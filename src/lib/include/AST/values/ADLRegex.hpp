/* ADLREGEX.hpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:09:45
 * Last edited:
 *   26/11/2020, 12:27:12
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLRegex class is used to represent a Regex-expression in the AST.
 *   In particular, it represents a value-regex, not a compile-result regex
 *   (although it does wrap them).
 * 
 *   Note: for now, it stores the Regex as a string, but this should change to
 *   an already parsed regex as soon as the Tokenizer supports this.
**/

#ifndef ADL_REGEX_HPP
#define ADL_REGEX_HPP

#include <string>
#include <vector>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLRegex class is used to represent regex-values in the AST. */
    class ADLRegex : public ADLLeaf {
    public:
        /* Raw value stored in this ADLRegex. */
        std::string value;

        /* Constructor for the ADLRegex class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from and the raw regex value. */
        ADLRegex(const std::vector<std::string>& filenames, size_t line, size_t col, const std::string& value);

        /* Allows the ADLRegex to be copied polymorphically. */
        virtual ADLRegex* copy() const;

    };
}

#endif

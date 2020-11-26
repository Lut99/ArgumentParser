/* ADLNUMBER.hpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:13:03
 * Last edited:
 *   26/11/2020, 12:26:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLNumber class is used to represent integral numbers in the
 *   Abstract Syntax Tree (AST) of the ADL. Note that it doesn't actually
 *   parse the value itself; this is left for the Tokenizer.
 * 
 *   The ADLNumber extends from the ADLValue class.
**/

#ifndef ADL_NUMBER_HPP
#define ADL_NUMBER_HPP

#include <string>
#include <vector>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    class ADLNumber: public ADLLeaf {
    public:
        /* The integral value to store. */
        long value;

        /* Constructor for the ADLNumber class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from and the value this node represents. */
        ADLNumber(const std::vector<std::string>& filenames, size_t line, size_t col, long value);

        /* Allows the ADLNumber to be copied polymorphically. */
        virtual ADLNumber* copy() const;

    };
}

#endif

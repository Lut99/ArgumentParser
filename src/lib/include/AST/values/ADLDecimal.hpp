/* ADLDECIMAL.hpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:13:03
 * Last edited:
 *   25/11/2020, 16:18:40
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLDecimal class is used to represent floating-point numbers in the
 *   Abstract Syntax Tree (AST) of the ADL. Note that it doesn't actually
 *   parse the value itself; this is left for the Tokenizer.
 * 
 *   The ADLDecimal extends from the ADLValue class.
**/

#ifndef ADL_DECIMAL_HPP
#define ADL_DECIMAL_HPP

#include <string>
#include <vector>

#include "ADLValue.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    class ADLDecimal: public ADLValue {
    public:
        /* The floating-point value to store. */
        double value;

        /* Constructor for the ADLDecimal class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from, this node's parent node and the value this node represents. */
        ADLDecimal(const std::vector<std::string>& filenames, size_t line, size_t col, ADLNode* parent, long value);

        /* Allows the ADLDecimal to be copied polymorphically. */
        virtual ADLDecimal* copy() const;

    };
}

#endif

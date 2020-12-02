/* ADLDECIMAL.hpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:13:03
 * Last edited:
 *   12/2/2020, 2:47:12 PM
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

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    class ADLDecimal: public ADLLeaf {
    public:
        /* The floating-point value to store. */
        double value;

        /* Constructor for the ADLDecimal class, which takes a breadcrumb trail of filenames where this node originated from, a DebugInfo struct linking this node to a location in the source file and the value this node represents. */
        ADLDecimal(const std::vector<std::string>& filenames, const DebugInfo& debug, double value);

        /* Prints the decimal to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLDecimal to be copied polymorphically. */
        virtual ADLDecimal* copy() const;

    };
}

#endif

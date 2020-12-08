/* ADLBOOLEAN.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 18:02:13
 * Last edited:
 *   08/12/2020, 20:50:00
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLBoolean class is used to represent boolean numbers in the
 *   Abstract Syntax Tree (AST) of the ADL. Note that it doesn't actually
 *   parse the value itself; this is left for the Tokenizer.
 * 
 *   The ADLBoolean extends from the ADLValue class.
**/

#ifndef ADL_BOOLEAN_HPP
#define ADL_BOOLEAN_HPP

#include <string>
#include <vector>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    class ADLBoolean: public ADLLeaf {
    public:
        /* The floating-point value to store. */
        double value;

        /* Constructor for the ADLBoolean class, which takes a DebugInfo struct linking this node to a location in the source file and the value this node represents. */
        ADLBoolean(const DebugInfo& debug, bool value);

        /* Prints the decimal to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLBoolean to be copied polymorphically. */
        virtual ADLBoolean* copy() const;

    };
}

#endif

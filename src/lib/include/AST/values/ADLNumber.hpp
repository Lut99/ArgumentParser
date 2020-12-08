/* ADLNUMBER.hpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 16:13:03
 * Last edited:
 *   08/12/2020, 20:50:49
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

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    class ADLNumber: public ADLLeaf {
    public:
        /* The integral value to store. */
        long value;

        /* Constructor for the ADLNumber class, which takes a DebugInfo struct linking this node to a location in the source file and the value this node represents. */
        ADLNumber(const DebugInfo& debug, long value);

        /* Prints the number to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLNumber to be copied polymorphically. */
        virtual ADLNumber* copy() const;

    };
}

#endif

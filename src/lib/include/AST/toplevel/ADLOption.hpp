/* ADLOPTION.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 16:10:35
 * Last edited:
 *   10/12/2020, 17:01:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLOption class, which is used to represent an
 *   Option declaration in the Abstract Syntax Tree (AST) of the ADL.
**/

#ifndef ADL_OPTION_HPP
#define ADL_OPTION_HPP

#include <string>
#include <ostream>

#include "ADLBranch.hpp"
#include "ADLIdentifier.hpp"
#include "ADLTypes.hpp"
#include "ADLConfigs.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLOption type is used to represent an Option declaration. */
    class ADLOption: public ADLBranch {
    public:
        /* Stores whether or not this Option is optional. */
        bool optional;
        /* Stores whether the type may be omitted when specifying the Option. */
        bool type_optional;
        /* Stores whether or not this Option is variadic. */
        bool variadic;

        /* Constructor for the ADLOption class, which takes a DebugInfo struct linking this node to the source code, the shortlabel for this Option (empty to disable), the longlabel of this Option (empty to disable), an optional list of types that make up this Option as ADLTypes, optionally a list of configuration parameters for this Positional as ADLConfigs, whether or not this Option is optional, whether or not this Option's type may be omitted and whether or not this Positional is variadic. */
        ADLOption(const DebugInfo& debug, ADLIdentifier* shortlabel = nullptr, ADLIdentifier* longlabel = nullptr, ADLTypes* types = nullptr, ADLConfigs* configs = nullptr, bool is_optional = false, bool type_is_optional = false, bool is_variadic = false);

        /* Prints the Option definition and all its configuration parameters to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLOption to be copied polymorphically. */
        virtual ADLOption* copy() const;

    };
}

#endif

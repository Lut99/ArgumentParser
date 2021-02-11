/* ADLOPTION.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 16:10:35
 * Last edited:
 *   11/02/2021, 17:55:16
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

#include "ADLDefinition.hpp"
#include "ADLIdentifier.hpp"
#include "ADLTypes.hpp"
#include "ADLConfigs.hpp"
#include "ADLSymbolTable.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLOption type is used to represent an Option declaration. */
    class ADLOption: public ADLDefinition {
    public:
        /* Stores whether or not this Option is optional. */
        bool optional;
        /* Stores whether the type may be omitted when specifying the Option. */
        bool type_optional;
        /* Stores whether or not this Option is variadic. */
        bool variadic;
        
        
        /* Constructor for the ADLOption class, which takes a DebugInfo struct linking this node to the source code, the shortlabel for this Option (empty to disable), the longlabel of this Option (empty to disable), an optional list of types that make up this Option as ADLTypes, optionally a list of configuration parameters for this Positional as ADLConfigs, whether or not this Option is optional, whether or not this Option's type may be omitted and whether or not this Positional is variadic. */
        ADLOption(const DebugInfo& debug, ADLIdentifier* shortlabel = nullptr, ADLIdentifier* longlabel = nullptr, ADLTypes* types = nullptr, ADLConfigs* configs = nullptr, bool is_optional = false, bool type_is_optional = false, bool is_variadic = false);
        
        /* Lets the Option-node have a go at trying to store the given value/node for the given property key. Returns 'true' if it was successfull, or 'false' if no such property is relevant for this class. */
        virtual bool set_property(const std::string& key, const std::string& value);

        /* Prints the Option definition and all its configuration parameters to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLOption to be copied polymorphically. */
        virtual ADLOption* copy() const;

    };
}

#endif

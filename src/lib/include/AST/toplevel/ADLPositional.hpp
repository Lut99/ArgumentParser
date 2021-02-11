/* ADLPOSITIONAL.hpp
 *   by Lut99
 *
 * Created:
 *   12/1/2020, 5:55:52 PM
 * Last edited:
 *   11/02/2021, 17:58:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLPositional class, which is used to represent
 *   a Positional argument definition in the ADL's Abstract Syntax Tree.
**/

#ifndef ADL_POSITIONAL_HPP
#define ADL_POSITIONAL_HPP

#include <string>
#include <vector>
#include <ostream>

#include "ADLDefinition.hpp"
#include "ADLIdentifier.hpp"
#include "ADLTypes.hpp"
#include "ADLConfigs.hpp"
#include "ADLSymbolTable.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLPositional class is used to represent a Positional definition. */
    class ADLPositional: public ADLDefinition {
    public:
        /* Stores whether or not this Positional is optional. */
        bool optional;
        /* Stores whether or not this Positional is variadic. */
        bool variadic;
        
        
        /* Constructor for the ADLPositional, which takes a DebugInfo struct linking this node to the source code, the identifier of this Positional, a list of types that make up this Positional as ADLTypes, whether or not this Positional is optional, whether or not this Positional is variadic and optionally a list of configuration parameters for this Positional as ADLConfigs. */
        ADLPositional(const DebugInfo& debug, ADLIdentifier* id, ADLTypes* types, bool is_optional, bool is_variadic, ADLConfigs* configs = nullptr);
        
        /* Lets the Positional-node have a go at trying to store the given value/node for the given property key. Returns 'true' if it was successfull, or 'false' if no such property is relevant for this class. */
        virtual bool set_property(const std::string& key, const std::string& value);

        /* Prints thePositional definition and all its configuration parameters to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLPositional to be copied polymorphically. */
        virtual ADLPositional* copy() const;

    };
}

#endif

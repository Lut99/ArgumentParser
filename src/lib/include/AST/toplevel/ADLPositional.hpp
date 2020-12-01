/* ADLPOSITIONAL.hpp
 *   by Lut99
 *
 * Created:
 *   12/1/2020, 5:55:52 PM
 * Last edited:
 *   12/1/2020, 6:32:49 PM
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

#include "ADLBranch.hpp"
#include "ADLTypes.hpp"
#include "ADLConfigs.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLPositional class is used to represent a Positional definition. */
    class ADLPositional: public ADLBranch {
    public:
        /* The identifier of this Positional. */
        std::string id;
        /* Stores whether or not this Positional is optional. */
        bool optional;
        /* Stores whether or not this Positional is variadic. */
        bool variadic;

        /* Constructor for the ADLPositional, which takes a breadcrumb trail of files, a DebugInfo struct linking this node to the source code, the identifier of this Positional, a list of types that make up this Positional as ADLTypes, whether or not this Positional is optional, whether or not this Positional is variadic and optionally a list of configuration parameters for this Positional as ADLConfigs. */
        ADLPositional(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& id, ADLTypes* types, bool is_optional, bool is_variadic, ADLConfigs* configs = nullptr);

        /* Prints thePositional definition and all its configuration parameters to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLPositional to be copied polymorphically. */
        virtual ADLPositional* copy() const;

    };
}

#endif

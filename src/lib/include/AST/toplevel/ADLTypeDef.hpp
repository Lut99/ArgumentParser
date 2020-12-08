/* ADLTYPE DEF.hpp
 *   by Lut99
 *
 * Created:
 *   27/11/2020, 16:03:08
 * Last edited:
 *   08/12/2020, 20:48:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the TypeDef class, which represents a custom type
 *   definition in the Abstract Syntax Tree (AST) of the ADL. It contains a
 *   list of configuration keywords (with values) as a single Configs-node.
 *   Value-wise, it carries not much more that it's typename.
**/

#ifndef ADL_TYPEDEF_HPP
#define ADL_TYPEDEF_HPP

#include <string>
#include <vector>

#include "ADLBranch.hpp"
#include "ADLConfigs.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLTypeDef class is used to represent new type definitions. */
    class ADLTypeDef: public ADLBranch {
    public:
        /* The identifier of this type definition, which can later be referenced again. */
        std::string id;

        /* Constructor for the ADLTypeDef class, which takes debug information about the node's origin, the type definition's identifier and optionally an ADLConfigs node. */
        ADLTypeDef(const DebugInfo& debug, const std::string& id, ADLConfigs* configs = nullptr);

        /* Prints the type definition and all its configuration parameters to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLTypeDef to be copied polymorphically. */
        virtual ADLTypeDef* copy() const;

    };
}

#endif

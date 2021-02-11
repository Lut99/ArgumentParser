/* ADLTYPE DEF.hpp
 *   by Lut99
 *
 * Created:
 *   27/11/2020, 16:03:08
 * Last edited:
 *   11/02/2021, 17:58:38
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

#include "ADLDefinition.hpp"
#include "ADLIdentifier.hpp"
#include "ADLConfigs.hpp"
#include "ADLSymbolTable.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLTypeDef class is used to represent new type definitions. */
    class ADLTypeDef: public ADLDefinition {
    public:
        /* Constructor for the ADLTypeDef class, which takes debug information about the node's origin, the type definition's identifier and optionally an ADLConfigs node. */
        ADLTypeDef(const DebugInfo& debug, ADLIdentifier* id, ADLConfigs* configs = nullptr);
        
        /* Lets the TypeDef-node have a go at trying to store the given value/node for the given property key. Returns 'true' if it was successfull, or 'false' if no such property is relevant for this class. */
        virtual bool set_property(const std::string& key, const std::string& value);

        /* Prints the type definition and all its configuration parameters to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLTypeDef to be copied polymorphically. */
        virtual ADLTypeDef* copy() const;

    };
}

#endif

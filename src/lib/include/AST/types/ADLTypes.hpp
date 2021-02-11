/* ADLTYPES.hpp
 *   by Lut99
 *
 * Created:
 *   12/1/2020, 6:12:29 PM
 * Last edited:
 *   11/02/2021, 16:29:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that contains the ADLTypes class, which is used as an array for
 *   type identifiers in a Positional or Option definition.
**/

#ifndef ADL_TYPES_HPP
#define ADL_TYPES_HPP

#include <string>
#include <ostream>
#include <vector>

#include "ADLBranch.hpp"
#include "ADLIdentifier.hpp"
#include "ADLTypeDef.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLTypes class, which is used to group several type identifiers together in one array. Note that, unlike ADLValues, no nested NonTerminals are present, since all children are of string type. */
    class ADLTypes: public ADLBranch {
    public:
        /* List of nodes that are the type definitions as stored in this ADLTypes. Not filled in until the check_references traversal. */
        std::vector<ADLTypeDef*> definitions;

        /* Constructor for the ADLTypes class, which takes a DebugInfo struct that links this node to the source file and optionally already one type identifier (as raw value). */
        ADLTypes(const DebugInfo& debug, ADLIdentifier* type_id = nullptr);

        /* Prints the list of identifiers to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLTypes to be copied polymorphically. */
        virtual ADLTypes* copy() const;
        
    };
}

#endif

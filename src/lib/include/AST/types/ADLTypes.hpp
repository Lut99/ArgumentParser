/* ADLTYPES.hpp
 *   by Lut99
 *
 * Created:
 *   12/1/2020, 6:12:29 PM
 * Last edited:
 *   08/12/2020, 20:48:54
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

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLTypes class, which is used to group several type identifiers together in one array. Note that, unlike ADLValues, no nested NonTerminals are present, since all children are of string type. */
    class ADLTypes: public ADLLeaf {
    public:
        /* Lists the internal list of type identifiers that this node wraps. */
        std::vector<std::string> ids;

        /* Constructor for the ADLTypes class, which takes a DebugInfo struct that links this node to the source file and optionally already one type identifier (as raw value). */
        ADLTypes(const DebugInfo& debug, const std::string& type_id = "");

        /* Prints the list of identifiers to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLTypes to be copied polymorphically. */
        virtual ADLTypes* copy() const;
        
    };
}

#endif

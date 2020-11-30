/* ADLREFERENCE.hpp
 *   by Lut99
 *
 * Created:
 *   30/11/2020, 17:49:26
 * Last edited:
 *   30/11/2020, 17:54:40
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADLReference class is used to represent a reference to another
 *   parameter of the same or another type, positional or option.
**/

#ifndef ADL_REFERENCE_HPP
#define ADL_REFERENCE_HPP

#include <string>
#include <vector>
#include <ostream>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLReference class is used to represent a reference to another property. */
    class ADLReference: public ADLLeaf {
    public:
        /* The ID (either a type ID, positional ID or Option short/longlabel) we reference. */
        std::string id;
        /* The field we reference in that type or argument. */
        std::string property;

        /* Constructor for the ADLReference class, which takes a breadcrumb trial of filenames, debug information about the node's origin, the type, positional or option to reference and the field to reference. */
        ADLReference(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& id, const std::string& property);

        /* Prints the reference to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLReference to be copied polymorphically. */
        virtual ADLReference* copy() const;

    };
}

#endif

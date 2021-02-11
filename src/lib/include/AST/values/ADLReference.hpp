/* ADLREFERENCE.hpp
 *   by Lut99
 *
 * Created:
 *   30/11/2020, 17:49:26
 * Last edited:
 *   11/02/2021, 17:06:50
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
#include <ostream>

#include "ADLBranch.hpp"
#include "ADLIdentifier.hpp"
#include "ADLConfig.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLReference class is used to represent a reference to another property. */
    class ADLReference: public ADLBranch {
    public:
        /* The ID of the definition (either a type ID, positional ID or Option short/longlabel) we reference. */
        ADLIdentifier* definition;
        /* The ID of the property we reference within that definition. */
        ADLIdentifier* property;
        /* The ADLNode* that this reference actually references. Not filled until the check_reference traversal. */
        ADLConfig* reference;

        /* Constructor for the ADLReference class, which takes debug information about the node's origin, the type, positional or option to reference, which of those type we reference, and the field to reference. */
        ADLReference(const DebugInfo& debug, ADLIdentifier* definition, ADLIdentifier* property);

        /* Prints the reference to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLReference to be copied polymorphically. */
        virtual ADLReference* copy() const;

    };
}

#endif

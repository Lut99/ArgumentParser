/* ADLSUPPRESS.hpp
 *   by Lut99
 *
 * Created:
 *   12/12/2020, 17:31:41
 * Last edited:
 *   12/12/2020, 17:41:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLSuppress class, which is used to wrap a
 *   suppress-modifier.
**/

#ifndef ADL_SUPPRESS_HPP
#define ADL_SUPPRESS_HPP

#include <string>
#include <ostream>

#include "ADLLeaf.hpp"
#include "NodeType.hpp"
#include "WarningTypes.hpp"

namespace ArgumentParser {
    /* The ADLSuppress class is used to wrap a @suppress-modifier. */
    class ADLSuppress: public ADLLeaf {
    public:
        /* The type that this ADLSuppress suppresses. */
        Exceptions::WarningType warning;
        /* The raw string value of said warning type. */
        std::string warning_name;


        /* Constructor for the ADLSuppress class, which takes debugging information, the WarningType that this node wraps and the raw string given. */
        ADLSuppress(const DebugInfo& debug, Exceptions::WarningType warning, const std::string& warning_name);

        /* Static function used to convert a raw warning name to a warning type. Returns WarningType::unknown if it wasn't known. */
        static Exceptions::WarningType parse_warning(const std::string& warning_name);

        /* Prints the Suppress-token and all its warning value to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLSuppress to be copied polymorphically. */
        virtual ADLSuppress* copy() const;

    };
}

#endif

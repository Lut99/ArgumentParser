/* ADLSUPPRESS.cpp
 *   by Lut99
 *
 * Created:
 *   12/12/2020, 17:35:56
 * Last edited:
 *   12/12/2020, 17:44:14
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLSuppress class, which is used to wrap a
 *   suppress-modifier.
**/

#include <cstring>

#include "ADLSuppress.hpp"

using namespace std;
using namespace ArgumentParser;


/***** ADLSUPPRESS CLASS *****/

/* Constructor for the ADLSuppress class, which takes debugging information, the WarningType that this node wraps and the raw string given. */
ADLSuppress::ADLSuppress(const DebugInfo& debug, Exceptions::WarningType warning, const std::string& warning_name) :
    ADLLeaf(NodeType::suppress, debug),
    warning(warning),
    warning_name(warning_name)
{}



/* Static function used to convert a raw warning name to a warning type. Returns WarningType::unknown if it wasn't known. */
Exceptions::WarningType ADLSuppress::parse_warning(const std::string& warning_name) {
    // Search the list of names
    for (size_t i = 0; i < sizeof(Exceptions::warningtype_names) / sizeof(const char*); i++) {
        if (strcmp(warning_name.c_str(), Exceptions::warningtype_names[i]) == 0) {
            // Found it; return the index as an enum
            return (Exceptions::WarningType) i;
        }
    }

    // Otherwise, we failed
    return Exceptions::WarningType::unknown;
}



/* Prints the Suppress-token and all its warning value to the given output stream. */
std::ostream& ADLSuppress::print(std::ostream& os) const {
    return os << "@suppress " << this->warning_name;
}



/* Allows the ADLSuppress to be copied polymorphically. */
ADLSuppress* ADLSuppress::copy() const { return new ADLSuppress(*this); }

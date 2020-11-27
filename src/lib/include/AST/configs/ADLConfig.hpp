/* ADLCONFIG.hpp
 *   by Lut99
 *
 * Created:
 *   27/11/2020, 16:24:48
 * Last edited:
 *   27/11/2020, 16:29:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLConfig class, which stores a single
 *   configuration argument for either type of argument definitions. Always
 *   stores a keyword, and then one or multiple values.
**/

#ifndef ADL_CONFIG_HPP
#define ADL_CONFIG_HPP

#include <string>
#include <vector>

#include "ADLBranch.hpp"
#include "ADLValues.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLConfig class wraps a single configuration key / value(s) pair. */
    class ADLConfig: public ADLBranch {
    public:
        /* The keyword stored in this ADLConfig, determining what parameter is given the values it stores. */
        std::string param;

        /* Constructor for the ADLConfig class, which takes a breadcrumb trail of filenames, debug information on the node's origin, the name of the parameter configured and a list of values as ADLValues. */
        ADLConfig(const std::vector<std::string>& filenames, const DebugInfo& debug, const std::string& param, ADLValues* values);

        /* Prints the configuration as the parameter name, all values and then a semicolon to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLConfig to be copied polymorphically. */
        virtual ADLConfig* copy() const;

    };
}

#endif

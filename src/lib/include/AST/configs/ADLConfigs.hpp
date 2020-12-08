/* ADLCONFIGS.hpp
 *   by Lut99
 *
 * Created:
 *   27/11/2020, 16:16:52
 * Last edited:
 *   08/12/2020, 20:46:07
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLConfigs class, which is used as a wrapper
 *   around ADLConfig nodes to group them in one array.
**/

#ifndef ADL_CONFIGS_HPP
#define ADL_CONFIGS_HPP

#include "ADLBranch.hpp"
#include "ADLConfig.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLConfigs class acts as an array that stores individual ADLConfig nodes. */
    class ADLConfigs: public ADLBranch {
    public:
        /* Constructor for the ADLConfigs class, which takes debug information on this node's origin and an ADLConfig node. */
        ADLConfigs(const DebugInfo& debug, ADLConfig* config);

        /* Prints the all the internal configuration parameters as a list to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLConfigs to be copied polymorphically. */
        virtual ADLConfigs* copy() const;

    };
}

#endif

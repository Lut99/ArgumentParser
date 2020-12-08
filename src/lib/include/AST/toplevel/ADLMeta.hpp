/* ADLMETA.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 17:26:20
 * Last edited:
 *   08/12/2020, 20:47:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLMeta class, which can be used to represent
 *   the near namespace where the user can set global values and define
 *   custom variables to re-use elsewhere.
**/

#ifndef ADL_META_HPP
#define ADL_META_HPP

#include <ostream>

#include "ADLBranch.hpp"
#include "ADLConfigs.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLMeta class is used to represent the toplevel Meta-namespace. */
    class ADLMeta: public ADLBranch {
    public:
        /* Constructor for the ADLMeta class, which takes debug information linking this node back to the source file and optionally properties to store in the meta namespace. */
        ADLMeta(const DebugInfo& debug, ADLConfigs* configs = nullptr);
        
        /* Prints all the toplevel properties, reflecting the AST structure. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLMeta to be copied polymorphically. */
        virtual ADLMeta* copy() const;

    };
}

#endif

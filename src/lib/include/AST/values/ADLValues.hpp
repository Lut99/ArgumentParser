/* ADLVALUES.hpp
 *   by Lut99
 *
 * Created:
 *   25/11/2020, 18:11:33
 * Last edited:
 *   27/11/2020, 14:53:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a class that is used to group multiple, consecutive
 *   ADLValue symbols into one list of them.
**/

#ifndef ADL_VALUES_HPP
#define ADL_VALUES_HPP

#include <string>
#include <vector>

#include "ADLBranch.hpp"
#include "NodeType.hpp"

namespace ArgumentParser {
    /* The ADLValues class is used to group ADLValue symbols. */
    class ADLValues: public ADLBranch {
    public:
        /* Constructor for the ADLValues class, which takes a trail of breadcrumbs for files we're parsing, a DebugInfo struct linking this node to a location in the source file and at least one value. */
        ADLValues(const std::vector<std::string>& filenames, const DebugInfo& debug, ADLNode* value);

        /* Prints each of the internal values to the given output stream, reflecting the AST structure. */
        virtual std::ostream& print(std::ostream& os) const;

        /* Allows the ADLValues to be copied polymorphically. */
        virtual ADLValues* copy() const;

    };
};

#endif

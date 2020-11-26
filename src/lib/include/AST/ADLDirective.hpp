/* ADLDIRECTIVE.hpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:41:08
 * Last edited:
 *   26/11/2020, 12:20:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the ADLDirective class, which is a special type of node that
 *   represents compiler directives. Will therefore likely be removed in
 *   one of the first AST traversals.
**/

#ifndef ADL_DIRECTIVE_HPP
#define ADL_DIRECTIVE_HPP

#include <cstdlib>
#include <string>
#include <vector>

#include "NodeType.hpp"
#include "ADLBranch.hpp"
#include "ADLValues.hpp"

namespace ArgumentParser {
    /* The ADLDirective class is used to temporarily store compiler directives before they are processed. */
    class ADLDirective : public ADLBranch {
    public:
        /* The directive that this nodes represents (i.e., the 'X' in '.X <values>'). */
        std::string directive;

        /* Constructor for the ADLDirective class, which takes a breadcrumb trail of filenames where this node originated from, the line number where this node originated from, the column number where it originated from, the directive itself and a list of values following it. */
        ADLDirective(const std::vector<std::string>& filenames, size_t line, size_t col, const std::string& directive, ADLValues* values);

        /* Allows the ADLDirective to be copied polymorphically. */
        virtual ADLDirective* copy() const;

    };
}

#endif

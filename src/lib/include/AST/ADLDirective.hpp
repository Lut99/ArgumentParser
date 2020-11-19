/* ADLDIRECTIVE.hpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:41:08
 * Last edited:
 *   19/11/2020, 14:42:23
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

#include "NodeType.hpp"
#include "ADLNode.hpp"

namespace ArgumentParser {
    /* The ADLDirective class is used to temporarily store compiler directives before they are processed. */
    class ADLDirective : public ADLNode {

    };
}

#endif

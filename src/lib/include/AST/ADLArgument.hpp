/* ADLARGUMENT.hpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:33:27
 * Last edited:
 *   19/11/2020, 14:37:04
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLArgument class, which is the baseclass for the
 *   representations of both argument types (Positionals and Options) in the
 *   Abstract Syntax Tree (AST) of the ADL.
**/

#ifndef ADL_ARGUMENT_HPP
#define ADL_ARGUMENT_HPP

#include "NodeType.hpp"
#include "ADLNode.hpp"

namespace ArgumentParser {
    /* The ADLArgument class is the baseclass for both types of argument definitions. */
    class ADLArgument : public ADLNode {
        
    };
}

#endif

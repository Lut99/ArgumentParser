/* ADLPOSITIONAL.hpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:35:37
 * Last edited:
 *   19/11/2020, 14:37:27
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLPositional class, which represents a
 *   Positional definition from ADL in it's Abstract Syntax Tree (AST).
 *   Extends from ADLArgument.
**/

#ifndef ADL_POSITIONAL_HPP
#define ADL_POSITIONAL_HPP

#include "NodeType.hpp"
#include "ADLArgument.hpp"

namespace ArgumentParser {
    /* The ADLPositional class is used to represent a Positional definition in ADL's AST. */
    class ADLPositional : public ADLArgument {

    };
}

#endif

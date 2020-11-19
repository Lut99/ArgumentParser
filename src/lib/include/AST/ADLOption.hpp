/* ADLOPTION.hpp
 *   by Lut99
 *
 * Created:
 *   19/11/2020, 14:38:09
 * Last edited:
 *   19/11/2020, 14:38:59
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the ADLOption class, which represents an Option
 *   definition from ADL in it's Abstract Syntax Tree (AST). Extends from
 *   ADLArgument.
**/

#ifndef ADL_OPTION_HPP
#define ADL_OPTION_HPP

#include "NodeType.hpp"
#include "ADLArgument.hpp"

namespace ArgumentParser {
    /* The ADLOption class is used to represent an Option definition in ADL's AST. */
    class ADLOption : public ADLArgument {

    };
}

#endif

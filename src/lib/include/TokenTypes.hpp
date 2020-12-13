/* TOKEN TYPES.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 18:16:59
 * Last edited:
 *   13/12/2020, 15:27:02
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains all information relating to the tokens of the
 *   Tokenizer.
**/

#ifndef TOKEN_TYPES_HPP
#define TOKEN_TYPES_HPP

#include <string>

namespace ArgumentParser {
    /* Enum for the Tokens, which is used to identify each separate token. */
    enum class TokenType {
        identifier = 0,
        shortlabel = 1,
        longlabel = 2,
        type = 3,

        config = 4,

        string = 5,
        number = 6,
        decimal = 7,
        boolean = 8,
        regex = 9,
        reference = 10,
        snippet = 11,

        l_square = 12,
        r_square = 13,
        l_curly = 14,
        r_curly = 15,
        semicolon = 16,
        triple_dot = 17,

        macro = 18,
        macro_and = 19,
        macro_or = 20,
        
        suppress = 21,
        warning = 22,
        error = 23,

        empty = 24,
    };

    /* Dictionary that maps a tokentype to a capitalized string. */
    const static std::string tokentype_names[] = {
        "Identifier",
        "Shortlabel",
        "Longlabel",
        "Type",

        "Config",

        "String",
        "Number",
        "Decimal",
        "Boolean",
        "Regex",
        "Reference",
        "Snippet",

        "LSquare",
        "RSquare",
        "LCurly",
        "RCurly",
        "Semicolon",
        "TripleDot",

        "Macro",
        "MacroAnd",
        "MacroOr",

        "Suppress",
        "Warning",
        "Error",

        "Empty"
    };
}

#endif

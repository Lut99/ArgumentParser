/* TOKEN TYPES.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 18:16:59
 * Last edited:
 *   03/12/2020, 23:08:30
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
        snippet = 10,

        l_square = 11,
        r_square = 12,
        l_curly = 13,
        r_curly = 14,
        semicolon = 15,
        triple_dot = 16,

        macro = 17,

        empty = 18,
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
        "Snippet",

        "LSquare",
        "RSquare",
        "LCurly",
        "RCurly",
        "Semicolon",
        "TripleDot",

        "Macro",

        "Empty"
    };
}

#endif

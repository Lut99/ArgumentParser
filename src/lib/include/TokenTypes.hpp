/* TOKEN TYPES.hpp
 *   by Lut99
 *
 * Created:
 *   03/12/2020, 18:16:59
 * Last edited:
 *   12/12/2020, 15:10:23
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
        suppress = 19,
        warning = 20,
        error = 21,

        empty = 22,
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
        "Suppress",
        "Warning",
        "Error",

        "Empty"
    };
}

#endif

/* WARNING TYPES.hpp
 *   by Lut99
 *
 * Created:
 *   12/12/2020, 17:22:33
 * Last edited:
 *   12/12/2020, 18:03:10
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains all the warning types that have been defined in the
 *   ADL compiler.
**/

#ifndef WARNING_TYPES_HPP
#define WARNING_TYPES_HPP

#include <string>

namespace ArgumentParser::Exceptions {
    /* Static list of all warning types defined. */
    enum class WarningType {
        int_overflow = 0,
        int_underflow = 1,
        float_overflow = 2,

        duplicate_define = 3,
        missing_define = 4,

        empty_statement = 5,
        stray_semicolon = 6,
        stray_suppress = 7,
        
        custom = 8,

        unknown = 9
    };

    /* Maps all WarningTypes to their string equivalent. */
    __attribute__((unused)) static const char* warningtype_names[] = {
        "integer-overflow",
        "integer-underflow",
        "float-overflow",

        "duplicate-define",
        "missing-define",

        "empty-statement",
        "stray-semicolon",
        "stray-suppress",

        "custom",

        "unknown"
    };
}

#endif

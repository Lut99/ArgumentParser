/* WARNING TYPES.hpp
 *   by Lut99
 *
 * Created:
 *   12/12/2020, 17:22:33
 * Last edited:
 *   13/12/2020, 15:00:37
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
#include <unordered_map>

namespace ArgumentParser::Exceptions {
    /* C-type that is used to interpret WarningTypes. */
    using warningtype_t = uint16_t;

    /* Static list of all warning types defined. */
    enum class WarningType {
        all = -1,
        unknown = 0x1,

        int_overflow = 0x2,
        int_underflow = 0x4,
        float_overflow = 0x8,

        duplicate_define = 0x10,
        missing_define = 0x20,

        empty_statement = 0x40,
        stray_semicolon = 0x80,
        stray_suppress = 0x100,
        
        custom = 0x200
    };

    /* Maps all WarningTypes to their string equivalent. */
    // __attribute__((unused)) 
    static const std::unordered_map<WarningType, std::string> warningtype_names = {
        { WarningType::unknown, "unknown" },

        { WarningType::int_overflow, "integer-overflow" },
        { WarningType::int_underflow, "integer-underflow" },
        { WarningType::float_overflow, "float-overflow" },

        { WarningType::duplicate_define, "duplicate-define" },
        { WarningType::missing_define, "missing-define" },

        { WarningType::empty_statement, "empty-statement" },
        { WarningType::stray_semicolon, "stray-semicolon" },
        { WarningType::stray_suppress, "stray-suppress" },

        { WarningType::custom, "custom" }
    };



    /* Given a WarningType that may consist of multiple ones, tries to extract all the possible WarningTypes and pretty prints them in a string. */
    std::string extract_type_names(const WarningType warnings, const std::string& concat_word = "and");

    /* Overloads the &-operator for the NodeTypes. */
    inline constexpr warningtype_t operator&(WarningType w1, WarningType w2) {
        return (warningtype_t) w1 & (warningtype_t) w2;
    }
    /* Overloads the |-operator for the NodeTypes. */
    inline constexpr WarningType operator|(WarningType w1, WarningType w2) {
        return (WarningType) ((warningtype_t) w1 | (warningtype_t) w2);
    }
}

#endif

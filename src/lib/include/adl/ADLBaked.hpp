/* ADLBAKED.hpp
 *   by Lut99
 *
 * Created:
 *   12/5/2020, 1:04:17 PM
 * Last edited:
 *   12/5/2020, 4:23:58 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a const static C++-map, which will contain the
 *   pre-baked ADL system files, ready to be included from a user file. For
 *   now, the files are included in plain tekst and parsed with the user
 *   file, but later we might want to pre-compile this and then include it
 *   if the ArgumentParser will support compiling individual files and then
 *   linking them together.
 * 
 *   The contents of this file will be inserted by a script, hence the weird
 *   comment at the place where one would expect dictionary values.
**/

#ifndef ADL_BAKED_HPP
#define ADL_BAKED_HPP

#include <cstddef>

namespace ArgumentParser {
    namespace System {
        static const size_t n_files = 1;

        static const char* names[] = {
            "stdtypes"
        };
        static const char* files[] = {
            "/* STDTYPES.adl\n *   by Lut99\n *\n * This file contains the often-used system types for the ADL. The types it\n * adds are:\n *  - 8-bit signed integers\n *  - 8-bit unsigned integers\n *  - 16-bit signed integers\n *  - 16-bit unsigned integers\n *  - 32-bit signed integers\n *  - 32-bit unsigned integers\n *  - 64-bit signed integers\n *  - 64-bit unsigned integers\n *  - single-precision (32-bit) floating-points\n *  - double-precision (64-bit) floating-points\n *  - booleans (true/false)\n *  - strings (simply raw text)\n *  - characters (single, readable character)\n */\n\n\n/***** COMMON CODE *****/\n\nmeta {\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n\n\n/***** INTEGERS *****/\n\n/* Implements an 8-bit signed integer as ADL type. */\n<int8> {\n    .name \"8-bit integer\";\n    .pattern r\"-?[0-9]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* Implements an 8-bit unsigned integer as ADL type. */\n<uint8> {\n    .name \"8-bit unsigned integer\";\n    .pattern r\"[0-9]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* Implements a 16-bit signed integer as ADL type. */\n<int16> {\n    .name \"16-bit integer\";\n    .pattern r\"-?[0-9]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* Implements a 16-bit unsigned integer as ADL type. */\n<uint16> {\n    .name \"16-bit unsigned integer\";\n    .pattern r\"[0-9]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* Implements a 32-bit signed integer as ADL type. */\n<int32> {\n    .name \"32-bit integer\";\n    .pattern r\"-?[0-9]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* Implements a 32-bit unsigned integer as ADL type. */\n<uint32> {\n    .name \"32-bit unsigned integer\";\n    .pattern r\"[0-9]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* Implements a 64-bit signed integer as ADL type. */\n<int64> {\n    .name \"64-bit integer\";\n    .pattern r\"-?[0-9]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* Implements a 64-bit unsigned integer as ADL type. */\n<uint64> {\n    .name \"64-bit unsigned integer\";\n    .pattern r\"[0-9]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n\n\n/***** FLOATING-POINT *****/\n\n/* 32-bit, single-precision floating-point. */\n<float> {\n    .name \"Single-precision floating point\";\n    .pattern r\"-?[0-9]+\\.\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* 64-bit, double-precision floating-point. */\n<double> {\n    .name \"Double-precision floating point\";\n    .pattern r\"-?[0-9]+\\.\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n\n\n/***** BOOLEANS *****/\n\n/* Boolean (true/false) value. */\n<bool> {\n    .name \"Boolean\";\n    .pattern r\"(true)|(false)\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n\n\n/***** STRINGS *****/\n\n/* String (multi-character) value. */\n<string> {\n    .name \"String\";\n    .pattern r\"[^]+\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n\n/* Char (single-character) value. */\n<char> {\n    .name \"Character\";\n    .pattern r\"[^]\";\n    .source ++{\n        /* TBD */\n    }++;\n}\n"
        };
    }
}

#endif

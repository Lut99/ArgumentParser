/* ADLPARSER.hpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:37:52 PM
 * Last edited:
 *   03/12/2020, 23:12:19
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADL Parser takes care of parsing the tokens that are returned from
 *   the ADL Tokenizer. See the Argument Definition Language specification
 *   for more information on the grammar that the parser implements. The
 *   output is a tree as found in ADLTree.hpp.
**/

#ifndef ADL_PARSER_HPP
#define ADL_PARSER_HPP

#include <vector>
#include <string>

#include "ADLExceptions.hpp"
#include "ADLTree.hpp"

namespace ArgumentParser {
    namespace Exceptions {
        /* Baseclass exception for all Parser-related errors. */
        class ParseError : public ADLCompileError {
        public:
            /* Constructor for the ParseError class, which takes a DebugInfo struct linking this warning to a location in the source file and optionally a message. */
            ParseError(const DebugInfo& debug, const std::string& message = "") :
                ADLCompileError(debug, message)
            {}

        };

        /* Baseclass exception for all Parser-related warnings. */
        class ParseWarning : public ADLCompileWarning {
        public:
            /* Constructor for the ParseWarning class, which takes a DebugInfo struct linking this warning to a location in the source file and optionally a message. */
            ParseWarning(const std::string& type, const DebugInfo& debug, const std::string& message = "") :
                ADLCompileWarning("parse-" + type, debug, message)
            {}

        };

    }

    /* Static "class" that is used to parse a file - and recursively all included files. */
    namespace Parser {
        /* Parses a single file. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but collects them in a vector which is then thrown. Use std::print_error on each of them to print them neatly. Warnings are always printed by the function, never thrown. */
        ADLTree* parse(const std::string& filename);
    };
    
}

#endif

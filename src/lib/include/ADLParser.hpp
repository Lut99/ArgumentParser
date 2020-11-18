/* ADLPARSER.hpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:37:52 PM
 * Last edited:
 *   15/11/2020, 14:18:03
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
            /* Constructor for the ParseError class, which takes a list of files we tried to parse (breadcrumb-style), the line number where the error occurred, the column number, the actual, optionally a message and optionally a pretty message. */
            ParseError(const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const std::string& message = "") :
                ADLCompileError(filenames, line, col, raw_line, message)
            {}

        };

        /* Baseclass exception for all Parser-related warnings. */
        class ParseWarning : public ADLCompileWarning {
        public:
            /* Constructor for the ParseError class, which takes a list of files we tried to parse (breadcrumb-style), the line number where the error occurred, the column number, the actual, optionally a message and optionally a pretty message. */
            ParseWarning(const std::string& type, const std::vector<std::string>& filenames, const size_t line, const size_t col, const std::string& raw_line, const std::string& message = "") :
                ADLCompileWarning("parse-" + type, filenames, line, col, raw_line, message)
            {}

        };

    }

    /* Static "class" that is used to parse a file - and recursively all included files. */
    namespace Parser {
        /* Parses a given file, including any included files. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but instead collects them as much as possible and therefore throws a vector of ParseExceptions. Use Exceptions::print_exceptions() to print them neatly. */
        ADLTree* parse(const std::string& filename);
    };
    
}

#endif

/* ADLPARSER.hpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:37:52 PM
 * Last edited:
 *   26/11/2020, 14:28:03
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

#include "ADLTokenizer.hpp"
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

        /* Exception for when an terminal symbol couldn't be matched at all any grammar rule. */
        class IllegalSymbolError: public ADLCompileError {
        public:
            /* Constructor for the IllegalSymbolError class, which takes a breadcrumb trail of filenames we are parsing and a token from which we will deduce stuff. */
            IllegalSymbolError(const std::vector<std::string>& filenames, const Token* token) :
                ADLCompileError(filenames, token->line, token->col, token->raw_line, "Expected positional identifier, shortlabel, longlabel or type identifier; not '" + token->raw + "'.")
            {}

        };

        /* Baseclass exception for when a non-terminal symbol is has been given without the necessary surrounding ones. */
        class StraySymbolError: public ADLCompileError {
        public:
            /* Constructor for the StraySymbolError class, which takes a breadcrumb trail of filenames we are parsing, a node from which we will deduce stuff and optionally a message. */
            StraySymbolError(const std::vector<std::string>& filenames, const ADLNode* node, const std::string& message) :
                ADLCompileError(filenames, node->line, node->col, node->raw_line, message)
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
        /* Parses a single file. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but collects them in a vector which is then thrown. Use std::print_error on each of them to print them neatly. Warnings are always printed by the function, never thrown. */
        ADLTree* parse(const std::string& filename);
    };
    
}

#endif

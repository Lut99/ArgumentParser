/* ADLPARSER.hpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:37:52 PM
 * Last edited:
 *   06/12/2020, 13:51:46
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

        /* Exception for when a starting curly bracket is missing. */
        class MissingLCurlyError: public ParseError {
        public:
            /* Constructor for the the MissingLCurlyError class, which takes a debug info struct for where we expected the lcurly to be. */
            MissingLCurlyError(const DebugInfo& debug) :
                ParseError(debug, "Expected a left curly bracket to start the definition body.")
            {}

            /* Copies the MissingLCurlyError polymorphically. */
            virtual MissingLCurlyError* copy() const { return new MissingLCurlyError(*this); }

        };
        /* Exception for when a semicolon is missing. */
        class MissingSemicolonError: public ParseError {
        public:
            /* Constructor for the the MissingSemicolonError class, which takes a debug info struct for where we expected the semicolon to be. */
            MissingSemicolonError(const DebugInfo& debug) :
                ParseError(debug, "Expected a semicolon to end a property definition.")
            {}

            /* Copies the MissingSemicolonError polymorphically. */
            virtual MissingSemicolonError* copy() const { return new MissingSemicolonError(*this); }

        };

        /* Exception for when a property definition is found without values. */
        class EmptyConfigError: public ParseError {
        public:
            /* Constructor for the the EmptyConfigError class, which takes a debug info struct to the config parameter. */
            EmptyConfigError(const DebugInfo& debug) :
                ParseError(debug, "Property definition cannot be empty; specify at least one value.")
            {}

            /* Copies the EmptyConfigError polymorphically. */
            virtual EmptyConfigError* copy() const { return new EmptyConfigError(*this); }

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

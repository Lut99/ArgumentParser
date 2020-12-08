/* ADLPARSER.hpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:37:52 PM
 * Last edited:
 *   08/12/2020, 21:36:20
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
        class ParseError: public ADLCompileError {
        public:
            /* Constructor for the ParseError class, which takes a DebugInfo struct linking this warning to a location in the source file and optionally a message. */
            ParseError(const DebugInfo& debug, const std::string& message = "") :
                ADLCompileError(debug, message)
            {}

        };

        /* General error for when we found a symbol we didn't define a special case for. */
        class GeneralError: public ParseError {
        public:
            /* Constructor for the GeneralError class, which only takes a DebugInfo struct linking this error to a location in the source file. */
            GeneralError(const DebugInfo& debug) :
                ParseError(debug, "Unexpected symbols.")
            {}

            /* Copies the GeneralError polymorphically. */
            virtual GeneralError* copy() const { return new GeneralError(*this); }

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
        /* Exception for when a Positional has missing types. */
        class MissingTypesError: public ParseError {
        public:
            /* Constructor for the the MissingTypesError class, which takes a debug info struct for where we expected the types to be. */
            MissingTypesError(const DebugInfo& debug) :
                ParseError(debug, "Missing specification of the Positional's types.")
            {}

            /* Copies the MissingTypesError polymorphically. */
            virtual MissingTypesError* copy() const { return new MissingTypesError(*this); }

        };

        /* Exception for when a body is defined but no identifier whatsoever is given. */
        class NamelessBodyError: public ParseError {
        public:
            /* Constructor for the NamelessBodyError class, which only takes a DebugInfo struct linking this error to a location in the source file. */
            NamelessBodyError(const DebugInfo& debug) :
                ParseError(debug, "Definition body needs an identifier to name it.")
            {}

            /* Copies the NamelessBodyError polymorphically. */
            virtual NamelessBodyError* copy() const { return new NamelessBodyError(*this); }

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
        /* Exception for when an optional ID is given, without the actual ID. */
        class EmptyOptionalIDError: public ParseError {
        public:
            /* Constructor for the EmptyOptionalIDError class, which only takes a DebugInfo struct relating this exception to the source code. */
            EmptyOptionalIDError(const DebugInfo& debug) :
                ParseError(debug, "Missing argument name when trying to define an optional argument.")
            {}

            /* Copies the EmptyOptionalIDError polymorphically. */
            virtual EmptyOptionalIDError* copy() const { return new EmptyOptionalIDError(*this); }

        };

        /* Exception for when a variadic marker (triple dot) is given, but it doesn't follow a Types dict. */
        class StrayVariadicException: public ParseError {
        public:
            /* Constructor for the the StrayVariadicException class, which takes a debug info struct to the config parameter. */
            StrayVariadicException(const DebugInfo& debug) :
                ParseError(debug, "Variadic marker must follow at least one type identifier, but none is given.")
            {}

            /* Copies the StrayVariadicException polymorphically. */
            virtual StrayVariadicException* copy() const { return new StrayVariadicException(*this); }

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

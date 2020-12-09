/* ADLPARSER.hpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:37:52 PM
 * Last edited:
 *   12/9/2020, 5:33:13 PM
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
        /* Exception for when a starting square bracket is not terminated properly. */
        class UnterminatedLCurlyError: public ParseError {
        public:
            /* Constructor for the the UnterminatedLCurlyError class, which takes a debug info struct for where we expected the lcurly to be. */
            UnterminatedLCurlyError(const DebugInfo& debug) :
                ParseError(debug, "Unterminated left curly bracket.")
            {}

            /* Copies the UnterminatedLCurlyError polymorphically. */
            virtual UnterminatedLCurlyError* copy() const { return new UnterminatedLCurlyError(*this); }
 
        };
        /* Exception for when a starting square bracket is missing. */
        class MissingLSquareError: public ParseError {
        public:
            /* Constructor for the the MissingLSquareError class, which takes a debug info struct for where we expected the lcurly to be. */
            MissingLSquareError(const DebugInfo& debug) :
                ParseError(DebugInfo(debug.filenames, debug.line2, debug.col2 + 1, debug.raw_line), "Expected a left square bracket to start optional brackets.")
            {}

            /* Copies the MissingLSquareError polymorphically. */
            virtual MissingLSquareError* copy() const { return new MissingLSquareError(*this); }

        };
        /* Exception for when a starting square bracket is not terminated properly. */
        class UnterminatedLSquareError: public ParseError {
        public:
            /* Constructor for the the UnterminatedLSquareError class, which takes a debug info struct for where we expected the lcurly to be. */
            UnterminatedLSquareError(const DebugInfo& debug) :
                ParseError(debug, "Unterminated left square bracket.")
            {}

            /* Copies the UnterminatedLSquareError polymorphically. */
            virtual UnterminatedLSquareError* copy() const { return new UnterminatedLSquareError(*this); }
 
        };
        /* Exception for when a semicolon is missing. */
        class MissingSemicolonError: public ParseError {
        public:
            /* Constructor for the the MissingSemicolonError class, which takes a debug info struct for where we expected the semicolon to be. */
            MissingSemicolonError(const DebugInfo& debug) :
                ParseError(DebugInfo(debug.filenames, debug.line2, debug.col2 + 1, debug.raw_line), "Expected a semicolon to end a property definition.")
            {}

            /* Copies the MissingSemicolonError polymorphically. */
            virtual MissingSemicolonError* copy() const { return new MissingSemicolonError(*this); }

        };
        /* Exception for when a property name misses in a config definition. */
        class MissingConfigError: public ParseError {
        public:
            /* Constructor for the the MissingConfigError class, which takes a debug info struct for where we expected the semicolon to be. */
            MissingConfigError(const DebugInfo& debug) :
                ParseError(debug, "Missing property name for property statement.")
            {}

            /* Copies the MissingConfigError polymorphically. */
            virtual MissingConfigError* copy() const { return new MissingConfigError(*this); }

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

        /* Exception for when something optional was given without anything valid in it. */
        class InvalidOptionalError: public ParseError {
        public:
            /* Constructor for the InvalidOptionalError class, which only takes a DebugInfo struct relating this exception to the source code. */
            InvalidOptionalError(const DebugInfo& debug) :
                ParseError(debug, "Only arguments (positional, option) or types can be optional.")
            {}

            /* Copies the InvalidOptionalError polymorphically. */
            virtual InvalidOptionalError* copy() const { return new InvalidOptionalError(*this); }

        };
        /* Exception for when an identifier was expected, but we didn't got it. */
        class MissingArgumentIdentifierError: public ParseError {
        public:
            /* Constructor for the MissingArgumentIdentifierError class, which only takes a DebugInfo struct relating this exception to the source code. */
            MissingArgumentIdentifierError(const DebugInfo& debug) :
                ParseError(debug, "Expected argument identifier.")
            {}

            /* Copies the MissingArgumentIdentifierError polymorphically. */
            virtual MissingArgumentIdentifierError* copy() const { return new MissingArgumentIdentifierError(*this); }

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

        /* Warning for when a given statement is empty. */
        class EmptyStatementWarning: public ParseWarning {
        public:
            /* Constructor for the the EmptyStatementWarning class, which takes a debug info struct to the config parameter. */
            EmptyStatementWarning(const DebugInfo& debug) :
                ParseWarning("empty-statement", debug, "Empty property statement encountered.")
            {}

            /* Copies the EmptyStatementWarning polymorphically. */
            virtual EmptyStatementWarning* copy() const { return new EmptyStatementWarning(*this); }

        };
        /* Warning for when a semicolon follows anything unexpected. */
        class StraySemicolonWarning: public ParseWarning  {
        public:
            /* Constructor for the the StraySemicolonWarning class, which takes a debug info struct to the config parameter. */
            StraySemicolonWarning(const DebugInfo& debug) :
                ParseWarning("stray-statement", debug, "Unnecessary semicolon encountered.")
            {}

            /* Copies the StraySemicolonWarning polymorphically. */
            virtual StraySemicolonWarning* copy() const { return new StraySemicolonWarning(*this); }

        };

    }

    /* Static "class" that is used to parse a file - and recursively all included files. */
    namespace Parser {
        /* Parses a single file. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but collects them in a vector which is then thrown. Use std::print_error on each of them to print them neatly. Warnings are always printed by the function, never thrown. */
        ADLTree* parse(const std::string& filename);
    };
    
}

#endif

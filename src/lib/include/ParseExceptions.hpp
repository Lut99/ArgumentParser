/* PARSE EXCEPTIONS.hpp
 *   by Lut99
 *
 * Created:
 *   12/9/2020, 5:58:13 PM
 * Last edited:
 *   2/9/2021, 3:38:05 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ParseExceptions file contains all the exceptions used by the ADL
 *   parser. Since the traversals often derive from the Parser's baseclass,
 *   they are included separately from the ADLParser itself.
**/

#ifndef PARSE_EXCEPTIONS_HPP

#include "ADLExceptions.hpp"

namespace ArgumentParser::Exceptions {
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
    
    /* Exception for when a suppress modifier does not have a value. */
    class EmptySuppressError: public ParseError {
    public:
        /* Constructor for the the EmptySuppressError class, which takes a debug info struct to the config parameter. */
        EmptySuppressError(const DebugInfo& debug) :
            ParseError(debug, "@suppress modifier requires a warning identifier to suppress.")
        {}

        /* Copies the EmptySuppressError polymorphically. */
        virtual EmptySuppressError* copy() const { return new EmptySuppressError(*this); }

    };
    /* Exception for when a warning modifier does not have a value. */
    class EmptyWarningError: public ParseError {
    public:
        /* Constructor for the the EmptyWarningError class, which takes a debug info struct to the config parameter. */
        EmptyWarningError(const DebugInfo& debug) :
            ParseError(debug, "@warning modifier requires a warning identifier or custom warning message to throw.")
        {}

        /* Copies the EmptyWarningError polymorphically. */
        virtual EmptyWarningError* copy() const { return new EmptyWarningError(*this); }

    };
    /* Exception for when a error modifier does not have a value. */
    class EmptyErrorError: public ParseError {
    public:
        /* Constructor for the the EmptyErrorError class, which takes a debug info struct to the config parameter. */
        EmptyErrorError(const DebugInfo& debug) :
            ParseError(debug, "@error modifier requires a custom error message to throw.")
        {}

        /* Copies the EmptyErrorError polymorphically. */
        virtual EmptyErrorError* copy() const { return new EmptyErrorError(*this); }

    };
    /* Exception for when a string follows a suppress token. */
    class SuppressStringError: public ParseError {
    public:
        /* Constructor for the the SuppressStringError class, which takes a debug info struct to the config parameter. */
        SuppressStringError(const DebugInfo& debug) :
            ParseError(debug, "@suppress modifier does not take a custom warning message.")
        {}

        /* Copies the SuppressStringError polymorphically. */
        virtual SuppressStringError* copy() const { return new SuppressStringError(*this); }

    };
    /* Exception for when an identifier follows a warning token. */
    class WarningIdentifierError: public ParseError {
    public:
        /* Constructor for the the WarningIdentifierError class, which takes a debug info struct to the config parameter. */
        WarningIdentifierError(const DebugInfo& debug) :
            ParseError(debug, "@warning modifier does not take a warning identifier; only custom warnings are allowed.")
        {}

        /* Copies the WarningIdentifierError polymorphically. */
        virtual WarningIdentifierError* copy() const { return new WarningIdentifierError(*this); }

    };
    /* Exception for when an identifier follows an error token. */
    class ErrorIdentifierError: public ParseError {
    public:
        /* Constructor for the the ErrorIdentifierError class, which takes a debug info struct to the config parameter. */
        ErrorIdentifierError(const DebugInfo& debug) :
            ParseError(debug, "@error modifier does not take a warning identifier.")
        {}

        /* Copies the ErrorIdentifierError polymorphically. */
        virtual ErrorIdentifierError* copy() const { return new ErrorIdentifierError(*this); }

    };
    /* Exception for when an unkown warning type is given. */
    class UnknownWarningError: public ParseError {
    public:
        const std::string given;

        /* Constructor for the the UnknownWarningError class, which takes a debug info struct to the config parameter and the raw name that was unknown. */
        UnknownWarningError(const DebugInfo& debug, const std::string& given_warning_name) :
            ParseError(debug, "Unknown warning '" + given_warning_name + "'."),
            given(given_warning_name)
        {}

        /* Copies the UnknownWarningError polymorphically. */
        virtual UnknownWarningError* copy() const { return new UnknownWarningError(*this); }

    };

    /* Custom error that a user can throw with @error. */
    class CustomError: public ParseError {
    public:
        /* Constructor for the CustomError class, which takes debug information and the custom message the user wants to display. */
        CustomError(const DebugInfo& debug, const std::string& message) :
            ParseError(debug, message)
        {}

        /* Polymorphically copies the CustomError class. */
        virtual CustomError* copy() const { return new CustomError(*this); }

    };



    /* Baseclass exception for all Parser-related warnings. */
    class ParseWarning : public ADLCompileWarning {
    public:
        /* Constructor for the ParseWarning class, which takes a DebugInfo struct linking this warning to a location in the source file and optionally a message. */
        ParseWarning(WarningType type, const DebugInfo& debug, const std::string& message = "") :
            ADLCompileWarning(type, debug, message)
        {}

    };

    /* Warning for when a given statement is empty. */
    class EmptyStatementWarning: public ParseWarning {
    public:
        /* Constructor for the the EmptyStatementWarning class, which takes a debug info struct to the config parameter. */
        EmptyStatementWarning(const DebugInfo& debug) :
            ParseWarning(WarningType::empty_statement, debug, "Empty property statement encountered.")
        {}

        /* Copies the EmptyStatementWarning polymorphically. */
        virtual EmptyStatementWarning* copy() const { return new EmptyStatementWarning(*this); }

    };
    /* Warning for when a semicolon follows anything unexpected. */
    class StraySemicolonWarning: public ParseWarning  {
    public:
        /* Constructor for the the StraySemicolonWarning class, which takes a debug info struct to the config parameter. */
        StraySemicolonWarning(const DebugInfo& debug) :
            ParseWarning(WarningType::stray_semicolon, debug, "Unnecessary semicolon encountered.")
        {}

        /* Copies the StraySemicolonWarning polymorphically. */
        virtual StraySemicolonWarning* copy() const { return new StraySemicolonWarning(*this); }

    };
    /* Exception for when a suppress-modifier is at a non-sensible location. */
    class StraySuppressWarning: public ParseWarning {
    public:
        /* Constructor for the StraySuppressWarning class, which takes debugging information where the modifier illegally occurred. */
        StraySuppressWarning(const DebugInfo& debug) :
            ParseWarning(WarningType::stray_suppress, debug, "Unexpected modifier encountered; doesn't do anything.")
        {}

        /* Copies the StraySuppressWarning polymorphically. */
        virtual StraySuppressWarning* copy() const { return new StraySuppressWarning(*this); }

    };
    
    /* Custom warning that a user can throw with @warning. */
    class CustomWarning: public ParseWarning {
    public:
        /* Constructor for the CustomWarning class, which takes debug information and the custom message the user wants to display. */
        CustomWarning(const DebugInfo& debug, const std::string& message) :
            ParseWarning(WarningType::custom, debug, message)
        {}

        /* Polymorphically copies the CustomWarning class. */
        virtual CustomWarning* copy() const { return new CustomWarning(*this); }

    };



    /* Baseclass exception for all Parser-related notes, accompanying any of the above errors or warnings. */
    class ParseNote: public ADLNote {
    public:
        /* Constructor for the ParseNote class, which takes a DebugInfo struct linking this note to a location in the source file and optionally a message. */
        ParseNote(const DebugInfo& debug, const std::string& message = "") :
            ADLNote(debug, message)
        {}

    };

}

#endif

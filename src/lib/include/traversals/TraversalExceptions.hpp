/* TRAVERSAL EXCEPTIONS.hpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 13:08:33
 * Last edited:
 *   01/01/2021, 15:59:55
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains all exceptions for all traversals. Since they can
 *   very well re-use previous exceptions, and derive from the same
 *   baseclass (which derives from the ParseError class), they have been
 *   separated in their own file instead of with their own traversals.
**/

#ifndef TRAVERSAL_EXCEPTIONS_HPP
#define TRAVERSAL_EXCEPTIONS_HPP

#include "ParseExceptions.hpp"
#include "DebugInfo.hpp"

namespace ArgumentParser::Exceptions {
    /* Baseclass exception for all Traversal errors, derived from the ParseError. */
    class TraversalError: public ParseError {
    public:
        /* Constructor for the TraversalError class, which takes debug information and optionally a message. */
        TraversalError(const DebugInfo& debug, const std::string& message = "") :
            ParseError(debug, message)
        {}
    };

    /* Baseclass error for when a symbol is defined twice. */
    class DuplicateSymbolError: public ParseError {
    public:
        /* The identifier which was defined twice or more. */
        const std::string identifier;

        /* Constructor for the DuplicateSymbolError class, which takes debug information, the identifier in question which was given twice and optionally a message. */
        DuplicateSymbolError(const DebugInfo& debug, const std::string& identifier, const std::string& message = "") :
            ParseError(debug, message),
            identifier(identifier)
        {}
    };
    /* Error for when an argument (Positional or Option) is defined multiple times. */
    class DuplicateArgumentError: public DuplicateSymbolError {
    public:
        /* Constructor for the DuplicateArgumentError class, which takes debugging information and the identifier that was defined twice. */
        DuplicateArgumentError(const DebugInfo& debug, const std::string& identifier) :
            DuplicateSymbolError(debug, identifier, "Argument '" + identifier + "' is defined more than once.")
        {}

        /* Allows the DuplicateArgumentError to be copied polymorphically. */
        virtual DuplicateArgumentError* copy() const { return new DuplicateArgumentError(*this); }

    };
    /* Error for when a type is defined multiple times. */
    class DuplicateTypeError: public DuplicateSymbolError {
    public:
        /* Constructor for the DuplicateTypeError class, which takes debugging information and the identifier that was defined twice. */
        DuplicateTypeError(const DebugInfo& debug, const std::string& identifier) :
            DuplicateSymbolError(debug, identifier, "Type '" + identifier + "' is defined more than once.")
        {}

        /* Allows the DuplicateTypeError to be copied polymorphically. */
        virtual DuplicateTypeError* copy() const { return new DuplicateTypeError(*this); }

    };
    /* Error for when a property is defined multiple times in the same namespace. */
    class DuplicatePropertyError: public DuplicateSymbolError {
    public:
        /* Constructor for the DuplicatePropertyError class, which takes debugging information and the identifier that was defined twice. */
        DuplicatePropertyError(const DebugInfo& debug, const std::string& identifier) :
            DuplicateSymbolError(debug, identifier, "Property '" + identifier + "' is already specified for this meta, argument or type definition.")
        {}

        /* Allows the DuplicatePropertyError to be copied polymorphically. */
        virtual DuplicatePropertyError* copy() const { return new DuplicatePropertyError(*this); }

    };



    /* Baseclass exception for all Traversal warnings, derived from the ParseWarning. */
    class TraversalWarning: public ParseWarning {
    public:
        /* Constructor for the TraversalWarning class, which takes the type of the warning, debug information and optionally a message. */
        TraversalWarning(WarningType type, const DebugInfo& debug, const std::string& message = "") :
            ParseWarning(type, debug, message)
        {}
        
    };

}

#endif

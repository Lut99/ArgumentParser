/* TRAVERSAL EXCEPTIONS.hpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 13:08:33
 * Last edited:
 *   10/12/2020, 16:21:48
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
    /* Error for when a Positional is defined multiple times. */
    class DuplicatePositionalError: public DuplicateSymbolError {
    public:
        /* Constructor for the DuplicatePositionalError class, which takes debugging information and the identifier that was defined twice. */
        DuplicatePositionalError(const DebugInfo& debug, const std::string& identifier) :
            DuplicateSymbolError(debug, identifier, "Positional '" + identifier + "' is defined more than once.")
        {}

        /* Allows the DuplicatePositionalError to be copied polymorphically. */
        virtual DuplicatePositionalError* copy() const { return new DuplicatePositionalError(*this); }

    };
    /* Error for when an Option is defined multiple times. */
    class DuplicateOptionError: public DuplicateSymbolError {
    public:
        /* Constructor for the DuplicateOptionError class, which takes debugging information and the identifier that was defined twice. */
        DuplicateOptionError(const DebugInfo& debug, const std::string& identifier) :
            DuplicateSymbolError(debug, identifier, "Option '" + identifier + "' is defined more than once.")
        {}

        /* Allows the DuplicateOptionError to be copied polymorphically. */
        virtual DuplicateOptionError* copy() const { return new DuplicateOptionError(*this); }

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
        TraversalWarning(const std::string& type, const DebugInfo& debug, const std::string& message = "") :
            ParseWarning(type, debug, message)
        {}
        
    };

}

#endif

/* TRAVERSAL EXCEPTIONS.hpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 13:08:33
 * Last edited:
 *   11/02/2021, 17:39:47
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
    class DuplicateSymbolError: public TraversalError {
    public:
        /* The identifier which was defined twice or more. */
        const std::string identifier;

        /* Constructor for the DuplicateSymbolError class, which takes debug information, the identifier in question which was given twice and optionally a message. */
        DuplicateSymbolError(const DebugInfo& debug, const std::string& identifier, const std::string& message = "") :
            TraversalError(debug, message),
            identifier(identifier)
        {}
    };
    /* Error for when an argument (Positional or Option) is defined multiple times. */
    class DuplicateArgumentError: public DuplicateSymbolError {
    public:
        /* Constructor for the DuplicateArgumentError class, which takes debugging information and the identifier that was defined twice. */
        DuplicateArgumentError(const DebugInfo& debug, const std::string& identifier) :
            DuplicateSymbolError(debug, identifier, "Argument '" + identifier + "' is already declared.")
        {}

        /* Allows the DuplicateArgumentError to be copied polymorphically. */
        virtual DuplicateArgumentError* copy() const { return new DuplicateArgumentError(*this); }

    };
    /* Error for when a type is defined multiple times. */
    class DuplicateTypeError: public DuplicateSymbolError {
    public:
        /* Constructor for the DuplicateTypeError class, which takes debugging information and the identifier that was defined twice. */
        DuplicateTypeError(const DebugInfo& debug, const std::string& identifier) :
            DuplicateSymbolError(debug, identifier, "Type '" + identifier + "' is already declared.")
        {}

        /* Allows the DuplicateTypeError to be copied polymorphically. */
        virtual DuplicateTypeError* copy() const { return new DuplicateTypeError(*this); }

    };
    /* Error for when a property is defined multiple times in the same namespace. */
    class DuplicatePropertyError: public DuplicateSymbolError {
    public:
        /* Constructor for the DuplicatePropertyError class, which takes debugging information and the identifier that was defined twice. */
        DuplicatePropertyError(const DebugInfo& debug, const std::string& identifier) :
            DuplicateSymbolError(debug, identifier, "Property '" + identifier + "' is already declared for this meta, argument or type definition.")
        {}

        /* Allows the DuplicatePropertyError to be copied polymorphically. */
        virtual DuplicatePropertyError* copy() const { return new DuplicatePropertyError(*this); }

    };

    /* Baseclass exception for when a symbol is referenced but not defined. */
    class UnknownSymbolError: public TraversalError {
    public:
        /* The identifier which was referenced but not defined. */
        const std::string identifier;

        /* Constructor for the UnknownSymbolError class, which takes debug information, the identifier in question and optionally a message. */
        UnknownSymbolError(const DebugInfo& debug, const std::string& identifier, const std::string& message = "") :
            TraversalError(debug, message),
            identifier(identifier)
        {}
        
    };
    /* Exception for when a meta-definition is referenced but not defined. */
    class UnknownMetaError: public UnknownSymbolError {
    public:
        /* Constructor for the UnknownMetaError class, which takes debugging information and the identifier that was referenced but not defined. */
        UnknownMetaError(const DebugInfo& debug, const std::string& identifier) :
            UnknownSymbolError(debug, identifier, "No meta-definition specified.")
        {}

        /* Allows the UnknownMetaError to be copied polymorphically. */
        virtual UnknownMetaError* copy() const { return new UnknownMetaError(*this); }

    };
    /* Exception for when an argument is referenced but not defined. */
    class UnknownArgumentError: public UnknownSymbolError {
    public:
        /* Constructor for the UnknownArgumentError class, which takes debugging information and the identifier that was referenced but not defined. */
        UnknownArgumentError(const DebugInfo& debug, const std::string& identifier) :
            UnknownSymbolError(debug, identifier, "No argument '" + identifier + "' found; did you misspell it?")
        {}

        /* Allows the UnknownArgumentError to be copied polymorphically. */
        virtual UnknownArgumentError* copy() const { return new UnknownArgumentError(*this); }

    };
    /* Exception for when a type is referenced but not defined. */
    class UnknownTypeError: public UnknownSymbolError {
    public:
        /* Constructor for the UnknownTypeError class, which takes debugging information and the identifier that was referenced but not defined. */
        UnknownTypeError(const DebugInfo& debug, const std::string& identifier) :
            UnknownSymbolError(debug, identifier, "No type '" + identifier + "' found; did you misspell it?")
        {}

        /* Allows the UnknownTypeError to be copied polymorphically. */
        virtual UnknownTypeError* copy() const { return new UnknownTypeError(*this); }

    };
    /* Exception for when a property is referenced, but it's (supposed) parent class is not defined. */
    class UnknownPropertyError: public UnknownSymbolError {
    public:
        /* The ID of the definition the user tried to find this property in. */
        const std::string definition;

        /* Constructor for the UnknownPropertyError class, which takes debugging information and the identifier that was referenced but not defined. */
        UnknownPropertyError(const DebugInfo& debug, const std::string& definition, const std::string& identifier) :
            UnknownSymbolError(debug, identifier, "Definition '" + definition + "' has no property '" + identifier + "'; did you misspell it?"),
            definition(definition)
        {}

        /* Allows the UnknownPropertyError to be copied polymorphically. */
        virtual UnknownPropertyError* copy() const { return new UnknownPropertyError(*this); }

    };



    /* Baseclass exception for all Traversal warnings, derived from the ParseWarning. */
    class TraversalWarning: public ParseWarning {
    public:
        /* Constructor for the TraversalWarning class, which takes the type of the warning, debug information and optionally a message. */
        TraversalWarning(WarningType type, const DebugInfo& debug, const std::string& message = "") :
            ParseWarning(type, debug, message)
        {}
        
    };

    /* Warning for when the meta-definition is actually empty. */
    class EmptyMetaWarning: public TraversalWarning {
    public:
        /* Constructor for the EmptyMetaWarning, which only takes the debug info where the meta definition is declared. */
        EmptyMetaWarning(const DebugInfo& debug) :
            TraversalWarning(WarningType::empty_meta, debug, "Empty meta-definition encountered; consider removing it")
        {}

        /* Allows the EmptyMetaWarning to be copied polymorphically. */
        virtual EmptyMetaWarning* copy() const { return new EmptyMetaWarning(*this); }

    };
    /* Warning for when a certain property is referenced zero times, nor has any special meaning to the compiler. */
    class UnusedPropertyWarning: public TraversalWarning {
    public:
        /* The identifier of the property that wasn't referenced. */
        const std::string identifier;

        /* Constructor for the UnusedPropertyWarning, which only takes the debug info where the property is declared and its identifier. */
        UnusedPropertyWarning(const DebugInfo& debug, const std::string& identifier) :
            TraversalWarning(WarningType::unused_property, debug, "Property is neither used by the compiler, nor referenced; consider removing it"),
            identifier(identifier)
        {}

        /* Allows the UnusedPropertyWarning to be copied polymorphically. */
        virtual UnusedPropertyWarning* copy() const { return new UnusedPropertyWarning(*this); }
 
    };



    /* Baseclass exception for all Traversal notes accompanying errors or warnings. */
    class TraversalNote: public ParseNote {
    public:
        /* Constructor for the TraversalNote class, which takes debug information and optionally a message. */
        TraversalNote(const DebugInfo& debug, const std::string& message = "") :
            ParseNote(debug, message)
        {}
        
    };

    /* Note accompanying the duplicate errors, saying where the previous declaration was. */
    class DuplicateSymbolNote: public TraversalNote {
    public:
        /* Constructor for the DuplicateSymbolNote class, which takes debugging information where the symbol was previously declared. */
        DuplicateSymbolNote(const DebugInfo& debug) :
            TraversalNote(debug, "Previously declared here:")
        {}

    };

}

#endif

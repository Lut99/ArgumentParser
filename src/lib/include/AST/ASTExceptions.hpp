/* ASTEXCEPTIONS.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 21:02:41
 * Last edited:
 *   26/11/2020, 12:19:04
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This files contains programming exceptions only relating to the ADL's
 *   Abstract Syntax Tree (AST).
**/

#ifndef AST_EXCEPTIONS_HPP
#define AST_EXCEPTIONS_HPP

#include <exception>
#include <string>

#include "NodeType.hpp"

namespace ArgumentParser::Exceptions {
    /* Baseclass exceptions for all ASTExceptions. */
    class ASTException : public std::exception {
    protected:
        /* Prepends the context to the message in a uniform way. */
        inline std::string generate(const std::string& context, const std::string& trav_id, const std::string message) {
            return context + ": " + trav_id + ": " + message;
        }

    public:
        /* String describing the context where this exception was called. */
        const std::string context;
        /* String unique to the traversal where the error occurred. */
        const std::string trav_id;
        /* String describing the error. */
        const std::string message;

        /* Constructor for the ASTException class, which takes a context, the id of the traversal and optionally a message. */
        ASTException(const std::string& context, const std::string& trav_id, const std::string& message = "") :
            context(context),
            trav_id(trav_id),
            message(message)
        {}

        /* Override for std::exception's what() function, for uncaught-exception compatibility. */
        virtual const char* what() const noexcept { return this->message.c_str(); }

    };

    /* Exception for when a node is added to a node that doesn't allow it. */
    class IllegalChildException: public ASTException {
    public:
        /* The types (conjoined in one NodeType) that are allowed. */
        const NodeType expected;
        /* The type that was given. */
        const NodeType given;

        /* Constructor for the IllegalChildException class, which takes a context, the id of the traversal where the error occurred and a NodeType denoting the expected types and a NodeType denoting the given types. */
        IllegalChildException(const std::string& context, const std::string& trav_id, const NodeType expected, const NodeType given) :
            ASTException(context, trav_id, generate(context, trav_id, "Cannot accept node of type " + nodetype_name.at(given) + " as child (only allows " + extract_type_names(expected) + ").")),
            expected(expected),
            given(given)
        {}

    };
    /* Exception for when too many nodes are attempted to be added to a branch node. */
    class MaximumChildrenException: public ASTException {
    public:
        /* Maximum number of children. */
        const size_t max;

        /* Constructor for the MaximumChildrenException class, which takes a context, a traversal ID and the maximum number of children for this node. */
        MaximumChildrenException(const std::string& context, const std::string& trav_id, const size_t max) :
            ASTException(context, trav_id, generate(context, trav_id, "Too many nodes given as child; cannot accept more than " + std::to_string(max) + " nodes.")),
            max(max)
        {}

    };

}

#endif

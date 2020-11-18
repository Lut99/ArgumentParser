/* EXCEPTIONS.hpp
 *   by Lut99
 *
 * Created:
 *   18/11/2020, 21:02:41
 * Last edited:
 *   18/11/2020, 21:22:32
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This files contains programming exceptions only relating to the ADL's
 *   Abstract Syntax Tree (AST).
**/

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

#include "NodeType.hpp"

namespace ArgumentParser::Exceptions {
    /* Baseclass exceptions for all ASTExceptions. */
    class ASTException : public std::exception {
    protected:
        /* Prepends the context to the message in a uniform way. */
        inline std::string generate(const std::string context, const std::string message) {
            return context + ": " + message;
        }

    public:
        /* String describing the context where this exception was called. */
        const std::string context;
        /* String describing the error. */
        const std::string message;

        /* Constructor for the ASTException class, which takes a context and optionally a message. */
        ASTException(const char* context, const std::string& message = "") :
            context(std::string(context)),
            message(message)
        {}

        /* Override for std::exception's what() function, for uncaught-exception compatibility. */
        virtual const char* what() const noexcept { return this->message.c_str(); }

    };

    /* Exception that is thrown whenever a node is replaced with an illegal node. */
    class IllegalNewNodeException : public ASTException {
    public:
        /* The NodeTypes that were allowed to be given. */
        const NodeType expected;
        /* The NodeType that we got. */
        const NodeType given;

        /* Constructor for the IllegalNewNodeException class, which takes a context, a possibly OR'ed NodeType that describes which types are allowed and the type that we were illegally given. */
        IllegalNewNodeException(const char* context, const NodeType expected, const NodeType given) :
            ASTException(context, generate(context, "Illegal node type return from traversal function: expected " + extract_type_names(expected) + ", got " + nodetype_name.at(given) + ".")),
            expected(expected),
            given(given)
        {}

    };

}

#endif

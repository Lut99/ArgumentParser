/* SYMBOL STACK.hpp
 *   by Lut99
 *
 * Created:
 *   13/11/2020, 15:33:42
 * Last edited:
 *   14/11/2020, 14:35:25
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines a stack of symbols, which are used to parse as a shift-reduce
 *   parser.
**/

#ifndef SYMBOLSTACK_HPP
#define SYMBOLSTACK_HPP

#include <cstdlib>

#include "ADLTokenizer.hpp"
#include "ADLParser.hpp"

namespace ArgumentParser::Parser {
    /* Base class for both terminals and non-terminals. */
    class Symbol {
    protected:
        /* Constructor for the Symbol class, which takes whether or not this is a terminal symbol. */
        Symbol(bool is_terminal) :
            is_terminal(is_terminal)
        {}

    public:
        /* The type of this symbol (either terminal or nonterminal). */
        bool is_terminal;

    };
    /* Used for Terminal symbols, i.e., raw and probably unparsed tokens. */
    class Terminal : public Symbol {
    private:
        /* The internal token for this Terminal. */
        Token _token;

    public:
        /* The type of the internal token. */
        TokenType type;

        /* Constructor for a Terminal symbol, which takes an unparsed Token from the Tokenizer. */
        Terminal(const Token& token) :
            Symbol(true),
            _token(token),
            type(token.type)
        {}

        /* Returns a reference to the internal token. */
        inline Token& token() { return this->_token; }
        /* Returns a constant reference to the internal token. */
        inline const Token& token() const { return this->_token; }

    };
    /* Used for non-terminal symbols, which are linked to ADLNodes. */
    class NonTerminal : public Symbol {
    private:
        /* Internal reference to a ADLNode. Note that this is heap-allocated, but not managed by this NonTerminal as we work bottom-up. */
        ADLNode* _node;

    public:
        /* Type of the internal node. */
        NodeType type;

        /* Constructor for the NonTerminal, which takes a reference to a node. */
        NonTerminal(ADLNode* node) :
            Symbol(false),
            _node(node),
            type(node->type)
        {}

        /* Returns the internal pointer. */
        inline ADLNode* node() const { return this->_node; }

    };

    /* Stack used to store, access and manage symbols. */
    class SymbolStack {
    private:
        /* Pointer to the list of Symbols. */
        Symbol** symbols;
        /* Number of symbols currently on the stack. */
        size_t length;
        /* Maximum number of symbols before we have to reallocate. */
        size_t max_length;

        /* Used to double the internal capacity. */
        void resize();

    public:
        /* Constructor for the SymbolStack, which optionally takes an initial size. */
        SymbolStack(size_t init_size = 64);
        /* Copy constructor for the SymbolStack class. */
        SymbolStack(const SymbolStack& other);
        /* Move constructor for the SymbolStack class. */
        SymbolStack(SymbolStack&& other);
        /* Destructor for the SymbolStack class. */
        ~SymbolStack();

        /* Adds a new terminal on the stack, based on the given token. */
        void add_terminal(const Token& token);
        /* Adds a new non-terminal on the stack, based on the given node. */
        void add_nonterminal(ADLNode* node);

        /* Gets the top symbol from the stack. */
        inline Symbol* head() const { return this->symbols[0]; }
        /* Gets the last symbol from the stack. */
        inline Symbol* tail() const { return this->symbols[this->length - 1]; }

        /* Gets & removes the top symbol from the stack. */
        Symbol* pop_head();
        /* Gets & removes the last symbol from the stack. */
        Symbol* pop_tail();

        /* Get the number of elements current on the stack. */
        inline size_t size() const { return this->length; }
        /* Get the maximum number of elements we can accept before we need to resize. */
        inline size_t capacity() const { return this->max_length; }

        /* Copy assignment oeprator for the SymbolStack class. */
        inline SymbolStack& operator=(const SymbolStack& other) { return *this = SymbolStack(other); }
        /* Move assignment operator for the SymbolStack class. */
        SymbolStack& operator=(SymbolStack&& other);
        /* Swap operator for the SymbolStack class. */
        friend void swap(SymbolStack& ss1, SymbolStack& ss2);

    };

    /* Swap operator for the SymbolStack class. */
    void swap(SymbolStack& ss1, SymbolStack& ss2);

}

#endif

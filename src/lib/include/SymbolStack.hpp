/* SYMBOL STACK.hpp
 *   by Lut99
 *
 * Created:
 *   13/11/2020, 15:33:42
 * Last edited:
 *   25/11/2020, 18:05:14
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
        Symbol(bool is_terminal);

    public:
        /* The type of this symbol (either terminal or nonterminal). */
        bool is_terminal;

        /* Also, declare the destructor as virtual. */
        virtual ~Symbol() = default;
        
        /* Swap operator for the Symbol class. */
        friend void swap(Symbol& s1, Symbol& s2);

    };
    /* Swap operator for the Symbol class. */
    void swap(Symbol& s1, Symbol& s2);
    
    /* Used for Terminal symbols, i.e., raw and probably unparsed tokens. */
    class Terminal : public Symbol {
    private:
        /* The internal token for this Terminal. */
        Token* _token;

    public:
        /* Constructor for a Terminal symbol, which takes an unparsed Token from the Tokenizer. */
        Terminal(Token* token);
        /* Copy constructor for the Terminal symbol. */
        Terminal(const Terminal& other);
        /* Move constructor for the Terminal symbol. */
        Terminal(Terminal&& other);
        /* Destructor for the Terminal symbol. */
        ~Terminal();

        /* Returns a reference to the internal token. */
        inline Token*& token() { return this->_token; }
        /* Returns the internal pointer. */
        inline Token* token() const { return this->_token; }
        /* Returns the internal type of the token. */
        inline TokenType type() const { return this->_token->type; }

        /* Returns the line number stored in this token. */
        inline size_t line() const { return this->_token->line; }
        /* Returns the column number stored in this token. */
        inline size_t col() const { return this->_token->col; }
        /* Returns the raw value stored in this token. */
        inline std::string raw() const { return this->_token->raw; }
        /* Returns the parsed value stored in this token. */
        template <class T> inline T value() const { return ((ValueToken<T>*) this->_token)->value; }

        /* Copy assignment operator for the Terminal symbol. */
        inline Terminal& operator=(const Terminal& other) { return *this = Terminal(other); }
        /* Move assignment operator for the Terminal symbol. */
        Terminal& operator=(Terminal&& other);
        /* Swap operator for the Terminal symbol. */
        friend void swap(Terminal& t1, Terminal& t2);

    };
    /* Swap operator for the Terminal symbol. */
    void swap(Terminal& t1, Terminal& t2);
    
    /* Used for non-terminal symbols, which are linked to ADLNodes. */
    class NonTerminal : public Symbol {
    private:
        /* Internal reference to a ADLNode. Note that this is heap-allocated, but not managed by this NonTerminal as we work bottom-up. */
        ADLNode* _node;

    public:
        /* Constructor for the NonTerminal, which takes a reference to a node. */
        NonTerminal(ADLNode* node) :
            Symbol(false),
            _node(node)
        {}

        /* Returns a reference to the internal token. */
        inline ADLNode*& token() { return this->_node; }
        /* Returns the internal pointer. */
        inline ADLNode* node() const { return this->_node; }
        /* Returns the internal type of the token. */
        inline NodeType type() const { return this->_node->type; }

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
        void add_terminal(Token* token);
        /* Replaces the first N symbols (deallocating them) with the new NonTerminal. */
        void replace(size_t N, NonTerminal* symbol);

        /* Returns the i'th symbol from the top of the stack. Note that it doesn't perform any form of memory-safe checking, so use size() to be sure you don't go out of bounds. */
        inline Symbol* operator[](size_t i) const { return this->symbols[this->length - 1 - i]; }

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

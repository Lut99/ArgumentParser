/* SYMBOL STACK.hpp
 *   by Lut99
 *
 * Created:
 *   13/11/2020, 15:33:42
 * Last edited:
 *   27/11/2020, 14:30:46
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
#include <ostream>

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

        /* Virtual function that lets the Terminal or NonTerminals print themselves to a given output stream. */
        virtual std::ostream& print(std::ostream& os) const = 0;
        
        /* Swap operator for the Symbol class. */
        friend void swap(Symbol& s1, Symbol& s2);

        /* Copies the Symbol polymorphically. */
        virtual Symbol* copy() const = 0;

    };
    /* Swap operator for the Symbol class. */
    void swap(Symbol& s1, Symbol& s2);
    /* Prints the derived Symbol class to the given output stream. */
    inline std::ostream& operator<<(std::ostream& os, const Symbol& s) { return s.print(os); }
    
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

        /* Returns the debug information of this token. */
        inline const DebugInfo& debug() const { return this->_token->debug; }
        /* Returns the raw value stored in this token. */
        inline std::string raw() const { return this->_token->raw; }
        /* Returns the parsed value stored in this token. */
        template <class T> inline T value() const { return ((ValueToken<T>*) this->_token)->value; }

        /* Lets the Terminal print itself to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const { return os << "Terminal(" << tokentype_names[(int) this->_token->type] << ")"; }

        /* Copies the Terminal polymorphically. */
        virtual Terminal* copy() const { return new Terminal(*this); };

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
        inline ADLNode*& node() { return this->_node; }
        /* Returns the internal pointer, optionally pre-casted to the correct type. */
        template <class T = ADLNode, typename = std::enable_if_t<std::is_base_of<ADLNode, T>::value> >
        inline T* node() const { return (T*) this->_node; }
        /* Returns the internal type of the token. */
        inline NodeType type() const { return this->_node->type; }

        /* Lets the NonTerminal print itself to the given output stream. */
        virtual std::ostream& print(std::ostream& os) const { return os << "NonTerminal(" << nodetype_name.at(this->_node->type) << ")"; }

        /* Copies the NonTerminal polymorphically. */
        virtual NonTerminal* copy() const { return new NonTerminal(*this); };

        /* Returns the debug information of this token. */
        inline const DebugInfo& debug() const { return this->_node->debug; }

    };



    /* Static & constant Terminal that wraps an empty token, used for when no more tokens are available. */
    const static Terminal t_empty(new Token(TokenType::empty, di_empty, ""));



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
        /* Constant iterator over the SymbolStack class. */
        class const_iterator {
        private:
            /* Stack object we iterate over. */
            const SymbolStack& stack;
            /* Current position in the stack. */
            size_t i;
            /* Marks if we went out-of-bounds. */
            bool out_of_bounds;

            /* Private constructor that initializes the const_iterator with the out_of_bounds flag. */
            const_iterator(const SymbolStack& stack, size_t i, bool out_of_bounds);

        public:
            /* Constructor for the const_iterator class, which takes a SymbolStack object and the starting position from the top of the stack (i.e., stored i = stack.size() - 1 - given i). */
            const_iterator(const SymbolStack& stack, size_t i = 0);

            /* Returns true if both iterators point to the same symbolstack and are at the same location. */
            inline bool operator==(const const_iterator& other) { return &this->stack == &other.stack && this->i == other.i && this->out_of_bounds == other.out_of_bounds; }
            /* Returns true if both iterators point to a different symbolstack or are at a different location. */
            inline bool operator!=(const const_iterator& other) { return &this->stack != &other.stack || this->i != other.i || this->out_of_bounds != other.out_of_bounds; }

            /* Increments the iterator by one. */
            const_iterator& operator++();
            /* Increments the iterator by one, after returning the value. */
            const_iterator operator++(int);
            /* Increments the iterator by N values. */
            const_iterator operator+(size_t N);
            /* Increments the iterator by N values. */
            const_iterator& operator+=(size_t N);
            /* Allows random access in the iterator. */
            const_iterator operator[](size_t i);

            /* Derefencres the iterator. */
            Symbol* operator*() const;

        };



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
        /* Removes the N first symbols from the stack (deallocating them). */
        void remove(size_t N);

        /* Returns the i'th symbol from the top of the stack. Note that it doesn't perform any form of memory-safe checking, so use size() to be sure you don't go out of bounds. */
        inline Symbol* operator[](size_t i) const { return this->symbols[i]; }
        /* Returns the i'th symbol from the stack, but this one performs out-of-bounds checking and returns the empty Symbol (i.e., terminal wrapping an empty token) if it's out-of-bounds. */
        Symbol* peek(size_t i) const;

        /* Get the number of elements current on the stack. */
        inline size_t size() const { return this->length; }
        /* Get the maximum number of elements we can accept before we need to resize. */
        inline size_t capacity() const { return this->max_length; }

        /* Returns an iterator pointing to the start of the SymbolStack (i.e., the top). */
        const_iterator begin() const { return const_iterator(*this, 0); }
        /* Returns an iterator pointing past the end of the SymbolStack (i.e., the bottom). */
        const_iterator end() const { return const_iterator(*this, this->length); }

        /* Copy assignment oeprator for the SymbolStack class. */
        inline SymbolStack& operator=(const SymbolStack& other) { return *this = SymbolStack(other); }
        /* Move assignment operator for the SymbolStack class. */
        SymbolStack& operator=(SymbolStack&& other);
        /* Swap operator for the SymbolStack class. */
        friend void swap(SymbolStack& ss1, SymbolStack& ss2);

    };
    /* Swap operator for the SymbolStack class. */
    void swap(SymbolStack& ss1, SymbolStack& ss2);
    /* Allows the SymbolStack to be printed to a given output stream. */
    std::ostream& operator<<(std::ostream& os, const SymbolStack& ss);

}

#endif

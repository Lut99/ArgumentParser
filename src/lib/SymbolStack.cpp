/* SYMBOL STACK.cpp
 *   by Lut99
 *
 * Created:
 *   13/11/2020, 15:33:50
 * Last edited:
 *   27/11/2020, 17:15:25
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Defines a stack of symbols, which are used to parse as a shift-reduce
 *   parser.
**/

#include "SymbolStack.hpp"

using namespace std;
using namespace ArgumentParser::Parser;


/***** SYMBOL CLASS *****/

/* Constructor for the Symbol class, which takes whether or not this is a terminal symbol. */
Symbol::Symbol(bool is_terminal) :
    is_terminal(is_terminal)
{}

/* Swap operator for the Symbol class. */
void ArgumentParser::Parser::swap(Symbol& s1, Symbol& s2) {
    using std::swap;

    swap(s1.is_terminal, s2.is_terminal);
}





/***** TERMINAL CLASS *****/

/* Constructor for a Terminal symbol, which takes an unparsed Token from the Tokenizer. */
Terminal::Terminal(Token* token) :
    Symbol(true),
    _token(token)
{}

/* Copy constructor for the Terminal symbol. */
Terminal::Terminal(const Terminal& other) :
    Symbol(other)
{
    // Copy the token explicitly
    this->_token = other._token->copy();
}

/* Move constructor for the Terminal symbol. */
Terminal::Terminal(Terminal&& other) :
    Symbol(std::move(other)),
    _token(std::move(other._token))
{
    // Set the other's token to nullptr so it doesn't deallocate it
    other._token = nullptr;
}

/* Destructor for the Terminal symbol. */
Terminal::~Terminal() {
    // Only deallocate the internal token if not null
    if (this->_token != nullptr) { delete this->_token; }
}



/* Move assignment operator for the Terminal symbol. */
Terminal& Terminal::operator=(Terminal&& other) {
    if (this != &other) { swap(*this, other); }
    return *this;
}

/* Swap operator for the Terminal symbol. */
void ArgumentParser::Parser::swap(Terminal& t1, Terminal& t2) {
    using std::swap;

    swap((Symbol&) t1, (Symbol&) t2);
    swap(t1._token, t2._token);
}





/***** SYMBOLSTACK CONST_ITERATOR *****/

/* Private constructor that initializes the const_iterator with the out_of_bounds flag. */
SymbolStack::const_iterator::const_iterator(const SymbolStack& stack, size_t i, bool out_of_bounds) :
    stack(stack),
    i(i),
    out_of_bounds(out_of_bounds)
{}

/* Constructor for the const_iterator class, which takes a SymbolStack object and the starting position from the top of the stack (i.e., stored i = stack.size() - 1 - given i). */
SymbolStack::const_iterator::const_iterator(const SymbolStack& stack, size_t i) :
    stack(stack),
    i(stack.size() - 1 - i),
    out_of_bounds(stack.size() == 0 || i >= stack.size())
{}



/* Increments the iterator by one. */
SymbolStack::const_iterator& SymbolStack::const_iterator::operator++() {
    // If already out of bounds, do nothing
    if (this->out_of_bounds) { return *this; }

    // Otherwise, check if we will go out of bounds
    if (this->i == 0) {
        this->out_of_bounds = true;
        return *this;
    }

    // If that's not the case, just normally subtract one
    --this->i;
    return *this;
}

/* Increments the iterator by one, after returning the value. */
SymbolStack::const_iterator SymbolStack::const_iterator::operator++(int) {
    const_iterator before = *this;
    ++*this;
    return before;
}

/* Increments the iterator by N values. */
SymbolStack::const_iterator SymbolStack::const_iterator::operator+(size_t N) {
    // If already out of bounds, do nothing
    if (this->out_of_bounds) { return const_iterator(this->stack, this->i, true); }

    // Otherwise, check if we will go out of bounds
    if (this->i < N) {
        return const_iterator(this->stack, this->i, true);
    }

    // If that's not the case, just normally subtract one
    return const_iterator(this->stack, i - N, false);
}

/* Increments the iterator by N values. */
SymbolStack::const_iterator& SymbolStack::const_iterator::operator+=(size_t N) {
    // If already out of bounds, do nothing
    if (this->out_of_bounds) { return *this; }

    // Otherwise, check if we will go out of bounds
    if (this->i < N) {
        this->out_of_bounds = true;
        return *this;
    }

    // If that's not the case, just normally subtract one
    this->i -= N;
    return *this;
}

/* Allows random access in the iterator. */
SymbolStack::const_iterator SymbolStack::const_iterator::operator[](size_t i) {
    // Simply create a new iterator and let the public constructor deal with it
    return const_iterator(this->stack, i);
}



/* Derefencres the iterator. */
Symbol* SymbolStack::const_iterator::operator*() const {
    // If this iterator points to something out-of-bounds, then return the empty symbol
    if (this->out_of_bounds) {
        return (Symbol*) &t_empty;
    }

    // Otherwise, return an actualy symbol
    return this->stack[this->i];
}





/***** SYMBOLSTACK CLASS *****/

/* Constructor for the SymbolStack, which optionally takes an initial size. */
SymbolStack::SymbolStack(size_t init_size) :
    length(0),
    max_length(init_size)
{
    // Create a list of max_size length
    this->symbols = new Symbol*[this->max_length];
}

/* Copy constructor for the SymbolStack class. */
SymbolStack::SymbolStack(const SymbolStack& other) :
    length(other.length),
    max_length(other.max_length)
{
    // Create a new list and copy all pointers
    this->symbols = new Symbol*[other.max_length];
    for (size_t i = 0; i < other.length; i++) {
        this->symbols[i] = other.symbols[i]->copy();
    }
}

/* Move constructor for the SymbolStack class. */
SymbolStack::SymbolStack(SymbolStack&& other) :
    symbols(other.symbols),
    length(other.length),
    max_length(other.length)
{
    // Set the other to nullptr
    other.symbols = nullptr;
}

/* Destructor for the SymbolStack class. */
SymbolStack::~SymbolStack() {
    // Simply delete the internal list if it isn't stolen
    if (this->symbols != nullptr) {
        // Don't forget to free all pointers themselves
        for (size_t i = 0; i < this->size(); i++) {
            delete this->symbols[i];
        }
        delete[] this->symbols;
    }
}



/* Used to double the internal capacity. */
void SymbolStack::resize() {
    // Declare a new vector
    size_t new_max_length = this->max_length * 2;
    Symbol** new_symbols = new Symbol*[new_max_length];
    
    // Copy the data
    memcpy(new_symbols, symbols, sizeof(Symbol*) * this->length);

    // Free the old one
    delete[] this->symbols;

    // Overwrite the data in the struct
    this->max_length = new_max_length;
    this->symbols = new_symbols;
}



/* Adds a new terminal on the stack, based on the given token. */
void SymbolStack::add_terminal(Token* token) {
    // Create a new Terminal
    Terminal* term = new Terminal(token);

    // Make sure there is enough space left
    if (this->length >= this->max_length) { this->resize(); }

    // Add the new terminal symbol
    this->symbols[this->length++] = (Symbol*) term;
}

/* Replaces the first N symbols (deallocating them) with the new NonTerminal. */
void SymbolStack::replace(size_t N, NonTerminal* symbol) {
    this->remove(N);

    // Insert the new symbol at that place
    this->symbols[this->length++] = symbol;

    // Done!
}

/* Removes the N first symbols from the stack (deallocating them). */
void SymbolStack::remove(size_t N) {
    // Loop & deallocate
    for (size_t i = 0; i < N; i++) {
        delete this->symbols[this->length - 1 - i];
    }
    // Decrease the length
    this->length -= N;
}



/* Returns the i'th symbol from the stack, but this one performs out-of-bounds checking and returns the empty Symbol (i.e., terminal wrapping an empty token) if it's out-of-bounds. */
Symbol* SymbolStack::peek(size_t i) const {
    if (i >= this->length) {
        // Simply return the empty token
        return (Symbol*) &t_empty;
    }

    // Otherwise, return the correct pointer
    return this->symbols[i];
}



/* Move assignment operator for the SymbolStack class. */
SymbolStack& SymbolStack::operator=(SymbolStack&& other) {
    if (this != &other) { swap(*this, other); }
    return *this;
}

/* Swap operator for the SymbolStack class. */
void ArgumentParser::Parser::swap(SymbolStack& ss1, SymbolStack& ss2) {
    using std::swap;

    swap(ss1.symbols, ss2.symbols);
    swap(ss1.length, ss2.length);
    swap(ss1.max_length, ss2.max_length);
}



/* Allows the SymbolStack to be printed to a given output stream. */
std::ostream& ArgumentParser::Parser::operator<<(std::ostream& os, const SymbolStack& ss) {
    os << "stack[";
    for (size_t i = 0; i < ss.size(); i++) {
        if (i > 0) { os << ' '; }
        os << (*ss[i]);
    }
    return os << "]";
}

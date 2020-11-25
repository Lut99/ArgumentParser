/* SYMBOL STACK.cpp
 *   by Lut99
 *
 * Created:
 *   13/11/2020, 15:33:50
 * Last edited:
 *   25/11/2020, 18:03:08
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





/***** SYMBOLSTACK CLASS *****/

/* Constructor for the SymbolStack, which optionally takes an initial size. */
SymbolStack::SymbolStack(size_t init_size) :
    length(init_size),
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
    this->symbols = new Symbol*[this->max_length];
    memcpy(this->symbols, other.symbols, sizeof(Symbol*) * this->length);
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
    if (this->symbols != nullptr) { delete[] this->symbols; }
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
    // Loop & deallocate
    size_t i = this->length - 1;
    for (; i >= this->length - N; i--) {
        delete this->symbols[i];
    }
    // Decrease the length
    this->length -= N;

    // Insert the new symbol at that place
    this->symbols[this->length++] = symbol;

    // Done!
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

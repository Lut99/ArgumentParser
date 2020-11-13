/* SYMBOL STACK.cpp
 *   by Lut99
 *
 * Created:
 *   13/11/2020, 15:33:50
 * Last edited:
 *   13/11/2020, 15:55:29
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
void SymbolStack::add_terminal(const Token& token) {
    // Create a new Terminal
    Terminal* term = new Terminal(token);

    // Make sure there is enough space left
    if (this->length >= this->max_length) { this->resize(); }

    // Add the new terminal symbol
    this->symbols[this->length++] = (Symbol*) term;
}

/* Adds a new non-terminal on the stack, based on the given node. */
void SymbolStack::add_nonterminal(ADLNode* node) {
    // Create a new NonTerminal
    NonTerminal* term = new NonTerminal(node);

    // Make sure there is enough space left
    if (this->length >= this->max_length) { this->resize(); }

    // Add the new terminal symbol
    this->symbols[this->length++] = (Symbol*) term;
}



/* Gets & removes the top symbol from the stack. */
Symbol* SymbolStack::pop_head() {
    // Get the first symbol
    Symbol* head = this->head();

    // Copy all the memory one element back
    --this->length;
    if (this->length > 0) { memmove(this->symbols, this->symbols + 1, sizeof(Symbol*) * this->length); }

    // Return the head
    return head;
}

/* Gets & removes the last symbol from the stack. */
Symbol* SymbolStack::pop_tail() {
    // Get the last symbol
    Symbol* tail = this->tail();

    // Simply reduce the length by one
    --this->length;

    // Return the tail
    return tail;
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

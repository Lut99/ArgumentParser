/* ADLPARSER.cpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:38:51 PM
 * Last edited:
 *   25/11/2020, 20:33:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The ADL Parser takes care of parsing the tokens that are returned from
 *   the ADL Tokenizer. See the Argument Definition Language specification
 *   for more information on the grammar that the parser implements. The
 *   output is a tree as found in ADLTree.hpp.
**/

#include <iostream>

#include "ADLDirective.hpp"

#include "ADLString.hpp"
#include "ADLRegex.hpp"
#include "ADLNumber.hpp"
#include "ADLDecimal.hpp"

#include "SymbolStack.hpp"
#include "ADLParser.hpp"

using namespace std;
using namespace ArgumentParser;
using namespace ArgumentParser::Parser;


/***** HELPER FUNCTIONS *****/

/* Looks at the next symbol of the stack, returning a pointer to it. */
#define PEEK(SYMBOL, STACK, I) \
    if ((I) >= (STACK).size()) { /* Do something with that no more symbols can be found. */ std::cerr << "WHOA NOTHING LEFT" << std::endl; exit(EXIT_FAILURE); } \
    (SYMBOL) = (STACK)[(I)++];

/* Tries to match the top of the stack and the lookahead with one of the hardcoded grammar rules. Returns whether it succeeded or not. */
bool reduce(const std::vector<std::string>& filenames, Token* lookahead, SymbolStack& stack) {
    // Keeps track of where we are looking on the stack
    size_t stack_i = 0;
    // Place holder for the to-be-examined symbol
    Symbol* symbol;
    
start:
    {
        // Start by look at the top of the stack
        PEEK(symbol, stack, stack_i);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            // Do different actions based on the type of symbol
            Terminal* term = (Terminal*) symbol;
            if (term->type() == TokenType::directive) {
                // Replace the Terminal with a directive that has no values
                size_t line = term->line();
                size_t col = term->col();
                stack.replace(1, new NonTerminal(
                    new ADLDirective(filenames, line, col, term->raw(), new ADLValues(filenames, line, col))
                ));
                return true;
                
            } else if (term->type() == TokenType::string) {
                // Replace it with it's NonTerminal counterpart & be done with it
                stack.replace(1, new NonTerminal(
                    new ADLString(filenames, term->line(), term->col(), term->raw())
                ));
                return true;

            } else if (term->type() == TokenType::regex) {
                // Replace it with it's NonTerminal counterpart & be done with it
                stack.replace(1, new NonTerminal(
                    new ADLRegex(filenames, term->line(), term->col(), term->raw())
                ));
                return true;

            } else if (term->type() == TokenType::number) {
                // Replace it with it's NonTerminal counterpart & be done with it
                stack.replace(1, new NonTerminal(
                    new ADLNumber(filenames, term->line(), term->col(), term->value<long>())
                ));
                return true;

            } else if (term->type() == TokenType::decimal) {
                // Replace it with it's NonTerminal counterpart & be done with it
                stack.replace(1, new NonTerminal(
                    new ADLDecimal(filenames, term->line(), term->col(), term->value<double>())
                ));
                return true;

            } else {
                // Unknown symbol; hard-error, since we can't continue beyond this point
                throw Exceptions::IllegalToplevelSymbol(filenames, term->token());
            }

        } else {
            // Do different actions based on the type of symbol
            NonTerminal* term = (NonTerminal*) symbol;
            if (term->type() == NodeType::file) {

            } else if (term->type() == NodeType::directive) {

            } else if (term->type() == NodeType::values) {

            } else if (term->type() & ArgumentParser::values) {
                
            }

        }
    }

}

/* Private implementation of the parse function, which only parses a single file. Returns a root node if parsing was successfull, but collects and throws ParseExceptions (or TokenizeExceptions) otherwise. */
ADLTree* shift_reduce(std::vector<std::string> filenames) {
    // Let's create a Tokenizer for our file
    Tokenizer in(filenames);

    // Parse as a shift-reduce parser - in every iteration, fetch a token and attempt to reduce the stack of tokens to a tree of nodes
    Token* lookahead = in.pop();
    SymbolStack stack;
    while (stack[0]->is_terminal || ((NonTerminal*) stack[0])->type() != NodeType::root) {
        // Check to see if we can match any grammar rule (reduce)
        bool success = reduce(filenames, lookahead, stack);

        // If we couldn't, then shift a new symbol
        stack.add_terminal(lookahead);
        lookahead = in.pop();
    }

}





/***** PARSER CLASS *****/

/* Parses a single file. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but collects them in a vector which is then thrown. Use std::print_error on each of them to print them neatly. Warnings are always printed by the function, never thrown. */
ADLTree* Parser::parse(const std::string& filename) {
    // Declare a new root node
    ADLTree* root = new ADLTree();

    // Populate it with a file node
    shift_reduce({ filename });

    // Return it
    return root;
}

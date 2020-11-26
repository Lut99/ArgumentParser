/* ADLPARSER.cpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:38:51 PM
 * Last edited:
 *   26/11/2020, 16:28:29
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
    (SYMBOL) = (STACK)[(I)--];

/* Tries to match the top of the stack and the lookahead with one of the hardcoded grammar rules. Returns whether it succeeded or not. */
bool reduce(const std::vector<std::string>& filenames, Token* lookahead, SymbolStack& stack) {
    // Keeps track of where we are looking on the stack
    size_t stack_i = stack.size() - 1;
    // Placeholder for the to-be-examined symbol
    Symbol* symbol;
    // Placeholder for a recently parsed NonTerminal
    ADLNode* prev_nonterm;

start:
    {
        // Start by look at the top of the stack
        PEEK(symbol, stack, stack_i);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            // Do different actions based on the type of symbol
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::directive:
                    // Cast it to a directive non-terminal, after which values can be added later
                    stack.replace(1, new NonTerminal(
                        new ADLDirective(filenames, term->debug(), term->raw())
                    ));
                    return true;
                
                case TokenType::string:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLString(filenames, term->debug(), term->raw());
                    goto value_start;
                
                case TokenType::regex:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLRegex(filenames, term->debug(), term->raw());
                    goto value_start;
                
                case TokenType::number:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLNumber(filenames, term->debug(), term->value<long>());
                    goto value_start;

                case TokenType::decimal:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLDecimal(filenames, term->debug(), term->value<double>());
                    goto value_start;
                
                default:
                    // No matching rule found
                    return false;

            }
        } else {
            // Do different actions based on the type of symbol
            NonTerminal* term = (NonTerminal*) symbol;
            switch(term->type()) {
                case NodeType::directive:
                    // First, check if any values are in the lookahead; if so, don't merge anything but instead shift
                    if (IS_VALUE(lookahead)) { return false; }

                    // Otherwise, try to merge the directive with a possible ADLFile or create a new one
                    prev_nonterm = term->node();
                    goto directive_start;

                case NodeType::values:
                    // First, check if any values are in the lookahead; if so, don't merge anything but instead shift
                    if (IS_VALUE(lookahead)) { return false; }

                    // Otherwise, try to merge the ADLValues with a possible ADLDirective
                    prev_nonterm = term->node();
                    goto values_start;

                default:
                    // No matching rule found
                    return false;
            }
        }
    }



value_start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, stack, stack_i);

        // Do different things based on whether it is a terminal or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && ((NonTerminal*) term)->type() == NodeType::values) {
            // Merge this value to the previously found Values
            term->node<ADLValues>()->add_node(prev_nonterm);
            return true;
        } else {
            // The one before this is definitely not an ADLValues; so just wrap the previously parsed on in such a NonTerminal and we're done
            stack.replace(1, new NonTerminal(
                new ADLValues(filenames, prev_nonterm->debug, prev_nonterm)
            ));
            return true;
        }
    }



directive_start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, stack, stack_i);

        // Do different things based on whether it is a terminal or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && ((NonTerminal*) term)->type() == NodeType::file) {
            // Merge this value to the previously found ADLFile
            term->node<ADLFile>()->add_node(prev_nonterm);
            return true;
        } else {
            // The one before this is definitely not an ADLValues; so just wrap the previously parsed on in such a NonTerminal and we're done
            stack.replace(1, new NonTerminal(
                new ADLFile(filenames, prev_nonterm)
            ));
            return true;
        }
    }



values_start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, stack, stack_i);

        // Do different things based on whether it is a terminal or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && ((NonTerminal*) term)->type() == NodeType::directive) {
            // Merge this value to the previously found ADLFile
            term->node<ADLDirective>()->add_node(prev_nonterm);
            return true;
        } else {
            // Otherwise, no such luck (since we can't simply promote a value to a directive)
            return false;
        }
    }

}

/* Analyses a stack that is done in principle but didn't reduce to a single ADLFile* node and prints out each of the errors. */
void analyze_errors(const std::vector<std::string>& filenames, const SymbolStack& stack) {
    for (size_t i = 0; i < stack.size(); i++) {
        Symbol* s = stack[i];
        if (s->is_terminal) {
            // If there are terminals left, then we apparently didn't expect it at that position
            throw Exceptions::IllegalSymbolError(filenames, ((Terminal*) s)->token());
        } else {
            NonTerminal* term = (NonTerminal*) s;
            if (term->type() == NodeType::values) {
                // Found stray values; that means one or more values have been given out of place
                throw Exceptions::StrayValuesSymbol(filenames, term->node<ADLValues>());
            } else if (term->type() == NodeType::directive) {
                // Found a directive that was apparently not in the top-level scope
                throw Exceptions::MisplacedDirectiveSymbol(filenames, term->node<ADLDirective>());
            }
        }
    }
}





/***** PARSER CLASS *****/

/* Parses a single file. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but collects them in a vector which is then thrown. Use std::print_error on each of them to print them neatly. Warnings are always printed by the function, never thrown. */
ADLFile* ArgumentParser::Parser::parse(const std::vector<std::string>& filenames) {
    // Let's create a Tokenizer for our file
    Tokenizer in(filenames);

    // Parse as a shift-reduce parser - in every iteration, fetch a token and attempt to reduce the stack of tokens to a tree of nodes
    Token* lookahead = in.pop();
    SymbolStack stack;
    bool changed = true;
    while (!in.eof() || changed) {
        // Check to see if we can match any grammar rule (reduce)
        bool success = reduce(filenames, lookahead, stack);
        changed = success;

        // If we couldn't, then shift a new symbol if there are any left
        if (!success && lookahead->type != TokenType::empty) {
            stack.add_terminal(lookahead);
            lookahead = in.pop();

            // Mark that the stack still changed
            changed = true;
        }

        #ifdef DEBUG
        cout << "Current stack: " << stack << endl;
        #endif
    }

    #ifdef DEBUG
    cout << "Done parsing, checking it we were able to do everything..." << endl;
    #endif

    // Check if we parsed everything
    if (stack.size() != 1 || stack[0]->is_terminal || ((NonTerminal*) stack[0])->type() != NodeType::file) {
        analyze_errors(filenames, stack);
        return nullptr;
    }

    // If we were successfull, return the only symbol as ADLFile!
    return ((NonTerminal*) stack[0])->node<ADLFile>();
}

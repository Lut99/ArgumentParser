/* ADLPARSER.cpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:38:51 PM
 * Last edited:
 *   30/11/2020, 15:13:16
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

#include "ADLTypeDef.hpp"

#include "ADLConfigs.hpp"
#include "ADLConfig.hpp"

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
#define PEEK(SYMBOL, STACK_ITER) \
    (SYMBOL) = *((STACK_ITER)++);
/* Looks at the next symbol of the stack for the error analyzer. */
#define PEEK_ERR(SYMBOL, STACK, I) \
    if ((I) >= (STACK).size()) { (SYMBOL) = (Symbol*) &t_empty; } \
    else { (SYMBOL) = (STACK)[(I)++]; }

/* Tries to match the top of the stack and the lookahead with one of the hardcoded grammar rules. Returns whether it succeeded or not. */
bool reduce(const std::vector<std::string>& filenames, Token* lookahead, SymbolStack& stack) {
    // Iterates over each stack symbol, returning the empty terminal if it went out-of-range
    SymbolStack::const_iterator iter = stack.begin();
    // Placeholder for the to-be-examined symbol
    Symbol* symbol;
    // Placeholder for a recently parsed Terminal s.t. debug information may be kept
    Token* prev_term;
    // Placeholder for a recently parsed NonTerminal
    ADLNode* prev_nonterm;
    // Placeholder for a slightly less recently parsed NonTerminal
    ADLNode* prev_prev_nonterm;

start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            // Do different actions based on the type of symbol
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::r_curly:
                    // See if we can parse as any of the definitions
                    prev_term = term->token();
                    goto definition_start;

                case TokenType::semicolon:
                    // Check if we can parse this as a config
                    prev_term = term->token();
                    goto config_start;

                case TokenType::string:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLString(filenames, term->debug(), term->raw());
                    goto value_merge;
                
                case TokenType::regex:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLRegex(filenames, term->debug(), term->raw());
                    goto value_merge;
                
                case TokenType::number:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLNumber(filenames, term->debug(), term->value<long>());
                    goto value_merge;

                case TokenType::decimal:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLDecimal(filenames, term->debug(), term->value<double>());
                    goto value_merge;
                
                default:
                    // No matching rule found
                    return false;

            }
        } else {
            // Do different actions based on the type of symbol
            NonTerminal* term = (NonTerminal*) symbol;
            switch(term->type()) {
                case NodeType::type_def:
                    // Merge with a possible exisitng Files node
                    prev_nonterm = term->node();
                    goto toplevel_merge;

                default:
                    // No matching rule found
                    return false;
            }
        }
    }



definition_start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a terminal or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && term->type() == NodeType::configs) {
            // If it's a list of configurations, we're on the good way!
            prev_nonterm = term->node();
            goto definition_configs;

        } else {
            // No matching rule found
            return false;
            
        }
    }



definition_configs:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a terminal or not
        Terminal* term = (Terminal*) symbol;
        if (symbol->is_terminal && term->type() == TokenType::l_curly) {
            // Yes! Very good! Now continue
            goto definitions_body;
            
        } else {
            // Not what we're looking for
            return false;

        }
    }



definitions_body:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            // Do different actions based on the type of symbol
            Terminal* term = (Terminal*) symbol;
            if (term->type() == TokenType::type) {
                // If we see a type token, that means that this was a TypeDefinition, so we can parse it as such

                // Compute the typedef's end and start debug info
                DebugInfo debug(term->debug().line1, term->debug().col1, term->raw());
                debug.line2 = prev_term->debug.line2;
                debug.col2 = prev_term->debug.col2;

                // Replace the symbols on the stack
                stack.replace(4, new NonTerminal(
                    new ADLTypeDef(filenames, debug, term->raw(), (ADLConfigs*) prev_nonterm)
                ));
                return true;
                
            } else {
                // Cannot do anything with this nonterminal
                return false;
            }

        } else {
            // Do different actions based on the type of symbol
            NonTerminal* term = (NonTerminal*) symbol;
            /* TBD */
            return false;

        }
    }



config_start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a terminal or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && term->type() == NodeType::values) {
            // Store this as the previous nonterm and then go to the config's final stage
            prev_nonterm = term->node();
            goto config_values;

        } else {
            // Not what we're looking for
            return false;
        }
    }



config_values:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a terminal or not
        Terminal* term = (Terminal*) symbol;
        if (symbol->is_terminal && term->type() == TokenType::identifier) {
            // If it's a normal identifier, that must be the keyword, and thus we're done

            // Construct the debug information
            DebugInfo debug(term->debug().line1, term->debug().col1, term->raw());
            debug.line2 = prev_term->debug.line2;
            debug.col2 = prev_term->debug.col2;

            // Create a new ADLConfig node and set it as the prevous value, then try to merge it with an existing ADLConfigs node
            prev_nonterm = new ADLConfig(filenames, debug, term->raw(), (ADLValues*) prev_nonterm);
            goto config_merge;

        } else {
            // Not what we're looking for, so we're done
            return false;
            
        }
    }



config_merge:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a configs node or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && term->type() == NodeType::configs) {
            // Merge this configs node and the one we parsed earlier together
            term->node<ADLConfigs>()->add_node(prev_nonterm);
            stack.remove(3);

            // Update the value's debug info though
            term->node<ADLConfigs>()->debug.line2 = prev_nonterm->debug.line2;
            term->node<ADLConfigs>()->debug.col2 = prev_nonterm->debug.col2;

            return true;

        } else {
            // The one before this is definitely not an ADLValues; so just wrap the previously parsed on in such a NonTerminal and we're done
            stack.replace(3, new NonTerminal(
                new ADLConfigs(filenames, prev_nonterm->debug, (ADLConfig*) prev_nonterm)
            ));
            return true;
            
        }
    }



value_merge:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a terminal or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && term->type() == NodeType::values) {
            // Merge this value to the previously found Values
            term->node<ADLValues>()->add_node(prev_nonterm);
            stack.remove(1);

            // Update the value's debug info though
            term->node<ADLValues>()->debug.line2 = prev_nonterm->debug.line2;
            term->node<ADLValues>()->debug.col2 = prev_nonterm->debug.col2;

            return true;
        } else {
            // The one before this is definitely not an ADLValues; so just wrap the previously parsed on in such a NonTerminal and we're done
            stack.replace(1, new NonTerminal(
                new ADLValues(filenames, prev_nonterm->debug, prev_nonterm)
            ));
            return true;
        }
    }



toplevel_merge:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter);

        // Do different things based on whether it is a terminal or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && term->type() == NodeType::file) {
            // Merge this value to the previously found File node
            term->node<ADLFile>()->add_node(prev_nonterm);
            stack.remove(1);
            return true;
            
        } else {
            // The one before this is definitely not an ADLValues; so just wrap the previously parsed on in such a NonTerminal and we're done
            stack.replace(1, new NonTerminal(
                new ADLFile(filenames, prev_nonterm)
            ));
            return true;
            
        }
    }

}

/* Analyses a stack that is done in principle but didn't reduce to a single ADLFile* node and prints out each of the errors. */
void analyze_errors(const std::vector<std::string>& filenames, const SymbolStack& stack) {
//     size_t stack_i = 0;

//     // Placeholder for the symbol we read from the stack
//     Symbol* symbol;

//     // Go through another final state machine to arrive at the correct error messages
// start:
//     {
//         // Get the next symbol from the stack
//         PEEK_ERR(symbol, stack, stack_i);

//         // Do different things depending on if it's a terminal or not
//         if (symbol->is_terminal) {

//         } else {
            
//         }
//     }
}





/***** PARSER CLASS *****/

/* Parses a single file. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but collects them in a vector which is then thrown. Use std::print_error on each of them to print them neatly. Warnings are always printed by the function, never thrown. */
ADLFile* ArgumentParser::Parser::parse(const std::vector<std::string>& filenames) {
    // Let's create a Tokenizer for our file
    Tokenizer in(filenames);

    // Initialize the stack
    SymbolStack stack;
    #ifdef DEBUG
    cout << "         " << stack << endl;
    #endif

    // Parse as a shift-reduce parser - in every iteration, fetch a token and attempt to reduce the stack of tokens to a tree of nodes
    Token* lookahead = in.pop();
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
        cout << (success ? "Reduce : " : "Shift  : ") << stack << endl;
        #endif
    }

    // Don't forget to free the lookahead
    delete lookahead;

    #ifdef DEBUG
    cout << endl;
    #endif

    // Check if we parsed everything
    if (stack.size() != 1 || stack[0]->is_terminal || ((NonTerminal*) stack[0])->type() != NodeType::file) {
        // Print errors to the user
        analyze_errors(filenames, stack);

        // Free the stack
        for (size_t i = 0; i < stack.size(); i++) {
            if (!stack[i]->is_terminal) {
                delete ((NonTerminal*) stack[i])->node();
            }
        }

        // Return that we failed
        return nullptr;
    }

    // If we were successfull, return the only symbol as ADLFile!
    return ((NonTerminal*) stack[0])->node<ADLFile>();
}

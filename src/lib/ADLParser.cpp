/* ADLPARSER.cpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:38:51 PM
 * Last edited:
 *   12/9/2020, 5:47:34 PM
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

#include "ADLMeta.hpp"
#include "ADLTypeDef.hpp"
#include "ADLPositional.hpp"
#include "ADLOption.hpp"

#include "ADLTypes.hpp"

#include "ADLConfigs.hpp"
#include "ADLConfig.hpp"

#include "ADLString.hpp"
#include "ADLRegex.hpp"
#include "ADLNumber.hpp"
#include "ADLDecimal.hpp"
#include "ADLBoolean.hpp"
#include "ADLReference.hpp"
#include "ADLSnippet.hpp"

#include "SymbolStack.hpp"
#include "ADLPreprocessor.hpp"
#include "ADLParser.hpp"

using namespace std;
using namespace ArgumentParser;
using namespace ArgumentParser::Parser;


/***** HELPER FUNCTIONS *****/

/* Looks at the next symbol of the stack, returning a pointer to it. */
#define PEEK(SYMBOL, STACK_ITER, N_SYMBOLS) \
    (SYMBOL) = *((STACK_ITER)++); \
    ++(N_SYMBOLS);
/* Looks at the next symbol of the stack for the error analyzer. */
#define PEEK_ERR(SYMBOL, STACK, I) \
    if ((I) >= (STACK).size()) { (SYMBOL) = (Symbol*) &t_empty; } \
    else { (SYMBOL) = (STACK)[(I)++]; }

/* Tries to match the top of the stack and the lookahead with one of the hardcoded grammar rules. Returns whether it succeeded or not. */
std::string reduce(const std::string& filename, Token* lookahead, SymbolStack& stack) {
    // Iterates over each stack symbol, returning the empty terminal if it went out-of-range
    SymbolStack::const_iterator iter = stack.begin();
    // Placeholder for the to-be-examined symbol
    Symbol* symbol;
    // Keeps track of how many symbols we match in this rule
    size_t n_symbols = 0;
    // Placeholder for a recently parsed Terminal s.t. debug information may be kept
    Token* prev_term;
    // Placeholder for a slightly less recently parsed Terminal s.t. debug information may be kept
    Token* prev_prev_term;
    // Placeholder for a recently parsed NonTerminal
    ADLNode* prev_nonterm;
    // Placeholder for a slightly less recently parsed NonTerminal
    ADLNode* prev_prev_nonterm;
    // Placeholder for any return-rules that might occur
    std::string applied_rule;
    // Stores if the types of an Option are optional or not
    bool types_optional = false;
    // Stores if the argument is variadic or not
    bool variadic = false;

    // Start
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            // Do different actions based on the type of symbol
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::r_curly:
                    // See if we can parse as any of the definitions
                    prev_term = term->token();
                    goto definition_start;
                
                case TokenType::type:
                    // If the tokentype is preceded by an identifier, shortlabel, longlabel or TYPES list, promote it to a TYPES list
                    prev_term = term->token();
                    goto types_merge;

                case TokenType::semicolon:
                    // Check if we can parse this as a config
                    prev_term = term->token();
                    goto config_start;

                case TokenType::string:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLString(term->debug(), term->raw());
                    applied_rule = "string";
                    goto value_merge;
                
                case TokenType::regex:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLRegex(term->debug(), term->raw());
                    applied_rule = "regex";
                    goto value_merge;
                
                case TokenType::number:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLNumber(term->debug(), term->value<long>());
                    applied_rule = "number";
                    goto value_merge;

                case TokenType::decimal:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLDecimal(term->debug(), term->value<double>());
                    applied_rule = "decimal";
                    goto value_merge;
                
                case TokenType::boolean:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLBoolean(term->debug(), term->value<bool>());
                    applied_rule = "boolean";
                    goto value_merge;
                
                case TokenType::reference:
                    {
                        // First, split the reference into identifier & property
                        size_t string_pos = 0;
                        for (size_t i = 0; i < term->raw().size(); i++) { if (term->raw()[i] == '.') { string_pos = i; break; } }
                        std::string identifier = term->raw().substr(0, string_pos);
                        std::string property = term->raw().substr(string_pos + 1);

                        // Infer the type of the reference
                        TokenType reference_type = TokenType::identifier;
                        if (identifier[0] == '-' && identifier[1] == '-') {
                            // Longlabel
                            reference_type = TokenType::longlabel;
                            identifier = identifier.substr(2);
                        } else if (identifier[0] == '-') {
                            // Shortlabel
                            reference_type = TokenType::shortlabel;
                            identifier = identifier[1];
                        } else if (identifier[0] == '<') {
                            // Type
                            reference_type = TokenType::type;
                            identifier = identifier.substr(1, identifier.size() - 2);
                        }

                        // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                        prev_nonterm = new ADLReference(term->debug(), identifier, reference_type, property);
                        applied_rule = "reference";
                        goto value_merge;
                    }

                case TokenType::snippet:
                    // Store the non-terminal equivalent of this terminal as the previous symbol, and then move to the next state
                    prev_nonterm = new ADLSnippet(term->debug(), term->raw());
                    applied_rule = "snippet";
                    goto value_merge;
                
                default:
                    // No matching rule found
                    return "";

            }
        } else {
            // Do different actions based on the type of symbol
            NonTerminal* term = (NonTerminal*) symbol;
            switch(term->type()) {
                case NodeType::meta:
                case NodeType::type_def:
                case NodeType::positional:
                case NodeType::option:
                    // Merge with a possible existing Files node
                    prev_nonterm = term->node();
                    goto toplevel_merge;

                default:
                    // No matching rule found
                    return "";
            }
        }
    }





/***** DEFINITION BODY PARSING *****/
definition_start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::l_curly:
                    // That means no configuration parameter for this argument
                    prev_nonterm = nullptr;
                    goto definitions_body;
                
                case TokenType::config:
                    // Found config without matching values or semicolon
                    Exceptions::log(Exceptions::EmptyConfigError(term->debug()));
                    stack.remove(n_symbols);
                    return "";
                
                default:
                    // In every other case, we assume there to be a missing left curly
                    Exceptions::log(Exceptions::MissingLCurlyError(DebugInfo(term->debug().filenames, term->debug().line2, term->debug().col2 + 1, term->debug().raw_line)));
                    stack.remove(n_symbols);
                    return "";

            }
        } else {
            NonTerminal* nterm = (NonTerminal*) symbol;
            switch(nterm->type()) {
                case NodeType::configs:
                    // If it's a list of configurations, we're on the good way!
                    prev_nonterm = nterm->node();
                    goto definition_configs;
                
                case NodeType::values:
                    // Missing semicolon
                    Exceptions::log(Exceptions::MissingSemicolonError(DebugInfo(nterm->debug().filenames, nterm->debug().line2, nterm->debug().col2 + 1, nterm->debug().raw_line)));
                    stack.remove(n_symbols);
                    return "";

                default:
                    // In every other case, we assume there to be a missing left curly
                    Exceptions::log(Exceptions::MissingLCurlyError(DebugInfo(nterm->debug().filenames, nterm->debug().line2, nterm->debug().col2 + 1, nterm->debug().raw_line)));
                    stack.remove(n_symbols);
                    return "";

            }
        }
    }



definition_configs:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Prepare a Debug struct to put the correct error location in
        DebugInfo debug = di_empty;

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::l_curly:
                    // Yes! Very good! Now continue, after temporarily storing the left curly for errors
                    goto definitions_body;
                
                case TokenType::config:
                case TokenType::semicolon:
                    // Fail silently, since errors with this should be handled in other states
                    return "";
                
                default:
                    // Read the debug info from the terminal to error
                    debug = term->debug();

            }
        } else {
            NonTerminal* nterm = (NonTerminal*) symbol;
            if (nterm->type() == NodeType::values) {
                // Fail silently, since errors with this should be handled in other states
                return "";
            }

            // Read the debug as nonterminal
            debug = nterm->debug();
        }

        // If here, it wasn't an LCurly
        Exceptions::log(Exceptions::MissingLCurlyError(debug));
        stack.remove(n_symbols);
        return "";
    }



definitions_body:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::identifier:
                    // It's the meta-construct; add it
                    {  
                        // ...IF it has the correct identifier; otherwise, we treat it as Positional without types
                        if (term->raw() != "meta") {
                            Exceptions::log(Exceptions::MissingTypesError(term->debug()));
                            stack.remove(n_symbols);
                            return "";
                        }

                        // Collect the debug information for the new definition
                        DebugInfo debug = term->debug();
                        debug.line2 = prev_term->debug.line2;
                        debug.col2 = prev_term->debug.col2;
                        debug.raw_line = prev_term->debug.raw_line;

                        // Replace the symbols on the stack with the new definition
                        stack.replace(n_symbols, new NonTerminal(
                            new ADLMeta(debug, (ADLConfigs*) prev_nonterm)
                        ));
                        return "meta";
                    }

                case TokenType::type:
                    // Alright, this is definitely a type definition
                    {
                        // Collect the debug information for the new definition
                        DebugInfo debug = term->debug();
                        debug.line2 = prev_term->debug.line2;
                        debug.col2 = prev_term->debug.col2;
                        debug.raw_line = prev_term->debug.raw_line;

                        // Replace the symbols on the stack with the new definition
                        stack.replace(n_symbols, new NonTerminal(
                            new ADLTypeDef(debug, term->raw(), (ADLConfigs*) prev_nonterm)
                        ));
                        return "typedef";
                    }

                case TokenType::triple_dot:
                    // The argument (which it definitely is, now) is variadic; mark it as such
                    variadic = true;
                    goto definitions_variadic;
                
                case TokenType::shortlabel:
                case TokenType::longlabel:
                    // It's an option; start to try parsing it

                    // First, mark that there is no type
                    prev_prev_nonterm = prev_nonterm;
                    prev_nonterm = nullptr;

                    // Then, store this symbol and move to parsing it
                    prev_prev_term = prev_term;
                    prev_term = term->token();
                    goto definitions_option;
                
                case TokenType::r_square:
                    // Something will be optional, but what? We'll see...
                    goto definitions_optional_start;
                
                default:
                    // Not for us, so assume that the user didn't provide us with an identifier
                    Exceptions::log(Exceptions::MissingArgumentIdentifierError(term->debug()));
                    stack.remove(n_symbols);
                    return "";

            }
        } else {
            NonTerminal* nterm = (NonTerminal*) symbol;
            if (nterm->type() == NodeType::types) {
                // We have types which aren't optional, so store this and try to parse that
                prev_prev_nonterm = prev_nonterm;
                prev_nonterm = nterm->node();
                goto definitions_types;

            } else {
                // No success, so assume that the user didn't provide us with an identifier
                Exceptions::log(Exceptions::MissingArgumentIdentifierError(nterm->debug()));
                stack.remove(n_symbols);
                return "";
            }
        }
    }





/***** DEFINITIONS NON-OPTIONAL TYPE PARSING *****/
definitions_variadic:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Prepare a Debug struct to put the correct error location in
        DebugInfo debug = di_empty;

        // We now only accept an ADLTypes token
        NonTerminal* nterm = (NonTerminal*) symbol;
        if (symbol->is_terminal) {
            debug = ((Terminal*) symbol)->debug();

        } else {
            if (nterm->type() == NodeType::types) {
                // OK! Store it and resume on the 'normal' path
                prev_prev_nonterm = prev_nonterm;
                prev_nonterm = nterm->node();
                goto definitions_types;
            } else {
                debug = nterm->debug();
            }
        }
        
        // No success, so we didn't encounter the one thing we expect
        Exceptions::log(Exceptions::StrayVariadicException(debug));
        stack.remove(n_symbols);
        return "";
    }



definitions_option:
    {
        // Start by looking at the top of the stack
        int temp = 0;
        PEEK(symbol, iter, temp);

        // Prepare placeholders for the options we'll use
        std::string shortlabel = "";
        std::string longlabel = "";
        DebugInfo debug = prev_prev_term->debug;

        // Determine if we're going to use this next symbol or not
        Terminal* term = (Terminal*) symbol;
        if (prev_term->type == TokenType::shortlabel) {
            shortlabel = prev_term->raw;
            if (symbol->is_terminal && term->type() == TokenType::longlabel) {
                longlabel = term->raw();
                debug.line1 = term->debug().line1;
                debug.col1 = term->debug().col1;
                ++n_symbols;
            } else {
                debug.line1 = prev_term->debug.line1;
                debug.col1 = prev_term->debug.col1;
            }
        } else {
            longlabel = prev_term->raw;
            if (symbol->is_terminal && term->type() == TokenType::shortlabel) {
                shortlabel = term->raw();
                debug.line1 = term->debug().line1;
                debug.col1 = term->debug().col1;
                ++n_symbols;
            } else {
                debug.line1 = prev_term->debug.line1;
                debug.col1 = prev_term->debug.col1;
            }
        }

        // Use the found values to create an ADLOption!
        stack.replace(n_symbols, new NonTerminal(
            new ADLOption(debug, shortlabel, longlabel, (ADLTypes*) prev_nonterm, (ADLConfigs*) prev_prev_nonterm, false, types_optional, variadic)
        ));
        return std::string("option") + (prev_nonterm == nullptr ? "-typeless" : "");
        
    }



definitions_types:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Determine the actual node type depending on what we find
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::identifier:
                    // It's a positional, and we can parse it immediately
                    {
                        // Collect the debug information for the new definition
                        DebugInfo debug = term->debug();
                        debug.line2 = prev_term->debug.line2;
                        debug.col2 = prev_term->debug.col2;
                        debug.raw_line = prev_term->debug.raw_line;

                        // Apply the grammar rule!
                        stack.replace(n_symbols, new NonTerminal(
                            new ADLPositional(debug, term->raw(), (ADLTypes*) prev_nonterm, false, variadic, (ADLConfigs*) prev_prev_nonterm)
                        ));
                        return "positional";
                    }
                
                case TokenType::shortlabel:
                case TokenType::longlabel:
                    // Time to parse it as non-optional option
                    prev_prev_term = prev_term;
                    prev_term = term->token();
                    goto definitions_option;
                
                case TokenType::r_square:
                    // It's an optional argument of some sort; check it out
                    prev_prev_term = term->token();
                    goto definitions_types_optional;
                
                default:
                    // We can only conclude the user forgot the identifier
                    Exceptions::log(Exceptions::MissingArgumentIdentifierError(term->debug()));
                    stack.remove(n_symbols);
                    return "";

            }
            
        } else {
            // We can only conclude the user forgot the identifier
            Exceptions::log(Exceptions::MissingArgumentIdentifierError(((NonTerminal*) symbol)->debug()));
            stack.remove(n_symbols);
            return "";
        }

    }



definitions_types_optional:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Determine the actual node type depending on what we find
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::identifier:
                    // It's a positional, and we can parse it immediately
                    {
                        // Check if the next symbol is actually optional
                        PEEK(symbol, iter, n_symbols);
                        if (!symbol->is_terminal || ((Terminal*) symbol)->type() != TokenType::l_square) {
                            // It wasn't
                            DebugInfo debug;
                            if (symbol->is_terminal) { debug = ((Terminal*) symbol)->debug(); }
                            else { debug = ((NonTerminal*) symbol)->debug(); }
                            Exceptions::log(Exceptions::MissingLSquareError(debug));
                            stack.remove(n_symbols);
                            return "";
                        }

                        // Collect the debug information for the new definition
                        DebugInfo debug = term->debug();
                        debug.line2 = prev_term->debug.line2;
                        debug.col2 = prev_term->debug.col2;
                        debug.raw_line = prev_term->debug.raw_line;

                        // Apply the grammar rule!
                        stack.replace(n_symbols, new NonTerminal(
                            new ADLPositional(debug, term->raw(), (ADLTypes*) prev_nonterm, true, variadic, (ADLConfigs*) prev_prev_nonterm)
                        ));
                        return "positional-optional";
                    }
                
                case TokenType::shortlabel:
                case TokenType::longlabel:
                    // Time to parse it as optional option
                    prev_prev_term = prev_term;
                    prev_term = term->token();
                    goto definitions_optional_option;
                
                case TokenType::l_square:
                    // No identifier given; just a closing leftsquare
                    {
                        DebugInfo debug = prev_prev_term->debug;
                        debug.line2 = term->debug().line2;
                        debug.col2 = term->debug().col2;
                        Exceptions::log(Exceptions::EmptyOptionalIDError(debug));
                        stack.remove(n_symbols);
                        return "";
                    }
                
                default:
                    // Otherwise, we treat it as an invalid optional body
                    Exceptions::log(
                        Exceptions::InvalidOptionalError(term->debug())
                    );
                    stack.remove(n_symbols);
                    return "";

            }
            
        } else {
            // Otherwise, we treat it as an invalid optional body
            Exceptions::log(
                Exceptions::InvalidOptionalError(((NonTerminal*) symbol)->debug())
            );
            stack.remove(n_symbols);
            return "";
        }

    }





/***** DEFINITIONS OPTIONAL TYPE PARSING *****/
definitions_optional_start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::shortlabel:
                case TokenType::longlabel:
                    // It's an option; start to try parsing it

                    // First, mark that there is no type
                    prev_prev_nonterm = prev_nonterm;
                    prev_nonterm = nullptr;

                    // Then, store this symbol and move to parsing it
                    prev_prev_term = prev_term;
                    prev_term = term->token();
                    goto definitions_optional_option;

                case TokenType::triple_dot:
                    // The argument (which it definitely is, now) is variadic; mark it as such
                    variadic = true;
                    goto definitions_optional_variadic;
                
                default:
                    // Not for us, i.e., invalid identifier(s) entered in optional space
                    Exceptions::log(Exceptions::InvalidOptionalError(term->debug()));
                    stack.remove(n_symbols);
                    return "";

            }
        } else {
            NonTerminal* nterm = (NonTerminal*) symbol;
            if (nterm->type() == NodeType::types) {
                // We have types which are optional, so store this and try to parse that
                prev_prev_nonterm = prev_nonterm;
                prev_nonterm = nterm->node();
                goto definitions_optional_types;

            } else {
                // Not for us, i.e., invalid identifier(s) entered in optional space
                Exceptions::log(Exceptions::InvalidOptionalError(nterm->debug()));
                stack.remove(n_symbols);
                return "";
            }
        }
    }



definitions_optional_option:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Prepare placeholders for the options we'll use
        std::string shortlabel = "";
        std::string longlabel = "";
        DebugInfo debug = prev_prev_term->debug;

        // Determine if we're going to use this next symbol or not
        Terminal* term = (Terminal*) symbol;
        if (prev_term->type == TokenType::shortlabel) {
            shortlabel = prev_term->raw;
            if (symbol->is_terminal && term->type() == TokenType::longlabel) {
                longlabel = term->raw();
                debug.line1 = term->debug().line1;
                debug.col1 = term->debug().col1;
            } else {
                debug.line1 = prev_term->debug.line1;
                debug.col1 = prev_term->debug.col1;
            }
        } else {
            longlabel = prev_term->raw;
            if (symbol->is_terminal && term->type() == TokenType::shortlabel) {
                shortlabel = term->raw();
                debug.line1 = term->debug().line1;
                debug.col1 = term->debug().col1;
            } else {
                debug.line1 = prev_term->debug.line1;
                debug.col1 = prev_term->debug.col1;
            }
        }

        // See if the next item is actually the mandatory closing bracket
        if (!shortlabel.empty() && !longlabel.empty()) {
            // We used the current symbol; grab a new one for comparison instead
            PEEK(symbol, iter, n_symbols);
        }
        if (!symbol->is_terminal || ((Terminal*) symbol)->type() != TokenType::l_square) {
            // It isn't; alert of the missing LCurly
            if (symbol->is_terminal) { debug = ((Terminal*) symbol)->debug(); }
            else { debug = ((NonTerminal*) symbol)->debug(); }
            Exceptions::log(Exceptions::MissingLSquareError(debug));
            stack.remove(n_symbols);
            return "";
        }

        // Use the found values to create an ADLOption!
        stack.replace(n_symbols, new NonTerminal(
            new ADLOption(debug, shortlabel, longlabel, (ADLTypes*) prev_nonterm, (ADLConfigs*) prev_prev_nonterm, true, types_optional, variadic)
        ));
        return std::string("option-optional") + (prev_nonterm == nullptr ? "-typeless" : "");
        
    }



definitions_optional_variadic:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Prepare a Debug struct to put the correct error location in
        DebugInfo debug = di_empty;

        // We now only accept an ADLTypes token
        NonTerminal* nterm = (NonTerminal*) symbol;
        if (symbol->is_terminal) {
            debug = ((Terminal*) symbol)->debug();

        } else {
            if (nterm->type() == NodeType::types) {
                // OK! Store it and resume on the 'normal' path
                prev_prev_nonterm = prev_nonterm;
                prev_nonterm = nterm->node();
                goto definitions_optional_types;
            } else {
                debug = nterm->debug();
            }
        }

        // No success
        Exceptions::log(Exceptions::MissingTypesError(debug));
        stack.remove(n_symbols);
        return "";
    }



definitions_optional_types:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Prepare the DebugInfo struct used to store information about a possible invalid symbol
        DebugInfo debug = di_empty;

        // We now only accept a closing bracket
        Terminal* term = (Terminal*) symbol;
        if (symbol->is_terminal) {
            if (term->type() == TokenType::l_square) {
                // Sweet! Go on to a special state which only allows parsing of Options, after marking the type as optional
                types_optional = true;
                goto definitions_types_options;
            } else {
                debug = term->debug();
            }
        } else {
            debug = ((NonTerminal*) symbol)->debug();
        }

        // No success
        Exceptions::log(Exceptions::MissingLSquareError(debug));
        stack.remove(n_symbols);
        return "";
    }



definitions_types_options:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // We now only accept (optional) shortlabels and / or longlabels
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch(term->type()) {
                case TokenType::shortlabel:
                case TokenType::longlabel:
                    // Non-optional Option; parse it as such (but this time, with types)
                    prev_prev_term = prev_term;
                    prev_term = term->token();
                    goto definitions_option;

                case TokenType::r_square:
                    // Go into one final state, searching for the idenfitier
                    prev_prev_term = term->token();
                    goto definitions_types_options_optional;

                default:
                    // We assume the thing to be nameless
                    Exceptions::log(Exceptions::MissingArgumentIdentifierError(term->debug()));
                    stack.remove(n_symbols);
                    return "";
            }
        }

        // We assume the thing to be nameless
        Exceptions::log(Exceptions::MissingArgumentIdentifierError(((NonTerminal*) symbol)->debug()));
        stack.remove(n_symbols);
        return "";
    }



definitions_types_options_optional:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // We now only accept shortlabels and / or longlabels
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch (term->type()) {
                case TokenType::shortlabel:
                case TokenType::longlabel:
                    // Sweet, parse as optional Option (but with types, this time)
                    prev_prev_term = prev_term;
                    prev_term = term->token();
                    goto definitions_optional_option;
                
                case TokenType::l_square:
                    // Empty optional tokens
                    {
                        DebugInfo debug = prev_prev_term->debug;
                        debug.line2 = term->debug().line2;
                        debug.col2 = term->debug().col2;
                        Exceptions::log(Exceptions::EmptyOptionalIDError(debug));
                        stack.remove(n_symbols);
                        return "";
                    }

                default:
                    // Expected an identifier here, but it wasn't
                    Exceptions::log(Exceptions::MissingArgumentIdentifierError(term->debug()));
                    stack.remove(n_symbols - 1);
                    return "";

            }
        } else {
            // Expected an identifier here, but it wasn't
            Exceptions::log(Exceptions::MissingArgumentIdentifierError(((NonTerminal*) symbol)->debug()));
            stack.remove(n_symbols - 1);
            return "";
        }
    }





/***** CONFIG RULES *****/
config_start:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Do different things based on whether it is a terminal or not
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch (term->type()) {
                case TokenType::l_curly:
                case TokenType::semicolon:
                    // Simply an empty statement; we'll allow it (but with warning)
                    Exceptions::log(Exceptions::EmptyStatementWarning(prev_term->debug));
                    stack.remove(1);
                    return "";
                
                case TokenType::config:
                    // Missing value!
                    Exceptions::log(Exceptions::EmptyConfigError(term->debug()));
                    stack.remove(2);
                    return "";
                
                default:
                    // Unexpected symbol to follow a termination; let the user know
                    Exceptions::log(Exceptions::StraySemicolonWarning(term->debug()));
                    stack.remove(1);
                    return "";

            }
        } else {
            NonTerminal* nterm = (NonTerminal*) symbol;
            switch(nterm->type()) {
                case NodeType::values:
                    // Store this as the previous nonterm and then go to the config's final stage
                    prev_nonterm = nterm->node();
                    goto config_values;
                
                default:
                    // Unexpected symbol to follow a termination; let the user know
                    Exceptions::log(Exceptions::StraySemicolonWarning(nterm->debug()));
                    stack.remove(1);
                    return "";

            }
        }
    }



config_values:
    {
        // Start by looking at the top of the stack
        PEEK(symbol, iter, n_symbols);

        // Do different things based on whether it is a terminal or not
        Terminal* term = (Terminal*) symbol;
        if (symbol->is_terminal && term->type() == TokenType::config) {
            // If it's a configuration identifier, that must be the keyword, and thus we're done

            // Construct the debug information
            DebugInfo debug = term->debug();
            debug.line2 = prev_term->debug.line2;
            debug.col2 = prev_term->debug.col2;

            // Create a new ADLConfig node and set it as the prevous value, then try to merge it with an existing ADLConfigs node
            prev_nonterm = new ADLConfig(debug, term->raw(), (ADLValues*) prev_nonterm);
            goto config_merge;
        }

        // Else it's clearly in the incorrect place
        DebugInfo debug = prev_nonterm->debug;
        debug.line2 = prev_term->debug.line2;
        debug.col2 = prev_term->debug.col2;
        Exceptions::log(Exceptions::MissingConfigError(debug));

        // Also remove the two symbols from the stack, for the simple reason as to not infuriate later errors
        stack.remove(2);
        return "";
    }





/***** MERGING / PROMOTION RULES *****/
types_merge:
    {
        // Start by looking at the top of the stack
        int temp = 0;
        PEEK(symbol, iter, temp);

        // Do different things based on whether it is a terminal or not
        Terminal* term = (Terminal*) symbol;
        NonTerminal* nterm = (NonTerminal*) symbol;
        if (symbol->is_terminal && (term->type() == TokenType::identifier ||
                                    term->type() == TokenType::shortlabel ||
                                    term->type() == TokenType::longlabel ||
                                    term->type() == TokenType::r_square ||
                                    term->type() == TokenType::l_square)) {
            // Create a new ADLTypes nonterminal
            stack.replace(n_symbols, new NonTerminal(
                new ADLTypes(prev_term->debug, prev_term->raw)
            ));
            return "types-new";

        } else if (nterm->type() == NodeType::types) {
            // Merge it with the previous types list
            nterm->node<ADLTypes>()->ids.push_back(prev_term->raw);

            // Update the debug info of the types list we merged with
            nterm->node<ADLTypes>()->debug.line2 = prev_term->debug.line2;
            nterm->node<ADLTypes>()->debug.col2 = prev_term->debug.col2;

            // Remove it from the stack (deallocating the token as well) and we're done
            stack.remove(n_symbols);
            return "types-merge";

        } else {
            // Don't promote it, since it will be used to parse as typedef instead
            return "";
        }
    }


    
config_merge:
    {
        // Start by looking at the top of the stack
        int temp = 0;
        PEEK(symbol, iter, temp);

        // Do different things based on whether it is a configs node or not
        if (!symbol->is_terminal) {
            NonTerminal* nterm = (NonTerminal*) symbol;
            switch (nterm->type()) {
                case NodeType::configs:
                    // Merge this configs node and the one we parsed earlier together
                    nterm->node<ADLConfigs>()->add_node(prev_nonterm);
                    stack.remove(n_symbols);

                    // Update the value's debug info though
                    nterm->node<ADLConfigs>()->debug.line2 = prev_nonterm->debug.line2;
                    nterm->node<ADLConfigs>()->debug.col2 = prev_nonterm->debug.col2;
                    nterm->node<ADLConfigs>()->debug.raw_line = prev_nonterm->debug.raw_line;

                    return "config-merge";
                
                case NodeType::values:
                    // Probably a missing semicolon
                    Exceptions::log(Exceptions::MissingSemicolonError(nterm->debug()));
                    stack.remove(n_symbols);
                    return "";
                
                default:
                    break;

            }
        }

        // The one before this is definitely not an ADLValues; so just wrap the previously parsed on in such a NonTerminal and we're done
        stack.replace(n_symbols, new NonTerminal(
            new ADLConfigs(prev_nonterm->debug, (ADLConfig*) prev_nonterm)
        ));
        return "config-new";
    }



value_merge:
    {
        // Start by looking at the top of the stack
        int temp = 0;
        PEEK(symbol, iter, temp);

        // Do different things based on whether it is a terminal or not
        NonTerminal* term = (NonTerminal*) symbol;
        if (!symbol->is_terminal && term->type() == NodeType::values) {
            // Merge this value to the previously found Values
            term->node<ADLValues>()->add_node(prev_nonterm);
            stack.remove(n_symbols);

            // Update the value's debug info though
            term->node<ADLValues>()->debug.line2 = prev_nonterm->debug.line2;
            term->node<ADLValues>()->debug.col2 = prev_nonterm->debug.col2;

            return applied_rule + "-merge";
        } else {
            // The one before this is definitely not an ADLValues; so just wrap the previously parsed on in such a NonTerminal and we're done
            stack.replace(n_symbols, new NonTerminal(
                new ADLValues(prev_nonterm->debug, prev_nonterm)
            ));
            return applied_rule + "-new";
        }
    }



toplevel_merge:
    {
        // Start by looking at the top of the stack
        int temp = 0;
        PEEK(symbol, iter, temp);

        // Do different things based on whether it is a terminal or not
        NonTerminal* nterm = (NonTerminal*) symbol;
        if (!symbol->is_terminal && nterm->type() == NodeType::root) {
            // Merge this value to the previously found File node
            nterm->node<ADLTree>()->add_node(prev_nonterm);

            // Update the Tree's debug information
            nterm->node<ADLTree>()->debug.line2 = prev_nonterm->debug.line2;
            nterm->node<ADLTree>()->debug.col2 = prev_nonterm->debug.col2;
            nterm->node<ADLTree>()->debug.raw_line = prev_nonterm->debug.raw_line;

            // We're done here
            stack.remove(n_symbols);
            return "toplevel-merge";
            
        } else {
            // The one before this is definitely not an ADLValues; so just wrap the previously parsed on in such a NonTerminal and we're done
            stack.replace(n_symbols, new NonTerminal(
                new ADLTree(filename, prev_nonterm)
            ));
            return "toplevel-new";
            
        }
    }

}

/* Analyses a stack that is done in principle but didn't reduce to a single ADLFile* node and prints out each of the errors. */
void analyze_errors(const SymbolStack& stack) {
    size_t stack_i = 0;

    // Placeholder for the symbol we read from the stack
    Symbol* symbol;

    // Go through another final state machine to arrive at the correct error messages
start:
    {
        // Get the next symbol from the stack
        PEEK_ERR(symbol, stack, stack_i);
        if (symbol->is_terminal && ((Terminal*) symbol)->type() == TokenType::empty) {
            // We're done
            return;
        }

        // Do different things depending on if it's a terminal or not
        if (symbol->is_terminal) {
            Terminal* term = (Terminal*) symbol;
            switch (term->type()) {
                case TokenType::l_curly:
                    // Unterminated left curly!
                    Exceptions::log(Exceptions::UnterminatedLCurlyError(term->debug()));
                    goto start;

                default:
                    // Just skip
                    Exceptions::log(Exceptions::GeneralError(term->debug()));
                    goto start;

            }
        } else {
            NonTerminal* nterm = (NonTerminal*) symbol;
            
            if (nterm->type() == NodeType::root) {
                // Error-less process
                goto start;
            }

            // Just skip
            Exceptions::log(Exceptions::GeneralError(nterm->debug()));
            goto start;
        }
    }
}





/***** PARSER CLASS *****/

/* Parses a single file. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but collects them in a vector which is then thrown. Use std::print_error on each of them to print them neatly. Warnings are always printed by the function, never thrown. */
ADLTree* ArgumentParser::Parser::parse(const std::string& filename) {
    // Let's create a Tokenizer for our file
    Preprocessor in(filename, {});

    // Initialize the stack
    SymbolStack stack;
    #ifdef DEBUG
    cout << "[   ADLParser   ] " << "         " << stack << endl;
    #endif

    // Parse as a shift-reduce parser - in every iteration, fetch a token and attempt to reduce the stack of tokens to a tree of nodes
    Token* lookahead = in.pop();
    bool changed = true;
    while (!in.eof() || changed) {
        // Check to see if we can match any grammar rule (reduce)
        std::string applied_rule = reduce(filename, lookahead, stack);
        changed = !applied_rule.empty();

        // If we couldn't, then shift a new symbol if there are any left
        if (applied_rule.empty() && lookahead->type != TokenType::empty) {
            stack.add_terminal(lookahead);
            lookahead = in.pop();

            // Mark that the stack still changed
            changed = true;
        }

        #ifdef DEBUG
        cout << "[   ADLParser   ] " << (!applied_rule.empty() ? "Reduce : " : "Shift  : ") << stack;
        if (!applied_rule.empty()) { cout << " // " << applied_rule; }
        cout << endl;
        #endif
    }

    // Don't forget to free the lookahead
    delete lookahead;

    #ifdef DEBUG
    cout << endl;
    #endif

    // Check if we parsed everything
    if (stack.size() != 1 || stack[0]->is_terminal || ((NonTerminal*) stack[0])->type() != NodeType::root) {
        // Print errors to the user
        analyze_errors(stack);

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
    return ((NonTerminal*) stack[0])->node<ADLTree>();
}

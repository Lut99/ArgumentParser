/* ADLPARSER.cpp
 *   by Lut99
 *
 * Created:
 *   11/12/2020, 5:38:51 PM
 * Last edited:
 *   18/11/2020, 20:21:35
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

#include "ADLTokenizer.hpp"
#include "ADLParser.hpp"
#include "SymbolStack.hpp"

using namespace std;
using namespace ArgumentParser;
using namespace ArgumentParser::Parser;


/***** HELPER FUNCTIONS *****/

/* Matches any of the grammar rules to the given list of tokens. Creates a node if we found any, or nullptr otherwise. */
ADLNode* match_grammar(const std::vector<Token>& buffer, const std::vector<std::string>& filenames) {
    return nullptr;
}


/* Private implementation of the parse function, which only parses a single file and appends its data to the given root node. */
void parse(ADLTree* root, std::vector<std::string> filenames) {
    // Let's create a Tokenizer for our file
    Tokenizer in(filenames);

    // Parse as a shift-reduce parser - in every iteration, fetch a token and attempt to reduce the stack of tokens to a tree of nodes
    SymbolStack stack;
    while (!in.eof()) {
        // Shift - get a new token on the stack
        Token t = in.pop();
        if (t.type == TokenType::empty) { break; }
        stack.add_terminal(t);

        // Reduce - try to match a grammar rule to the stack, and replace the value in it
    }

}





/***** PARSER CLASS *****/

/* Parses a given file, including any included files. Returns a single root node, from which the entire parsed tree is build. Does not immediately throw exceptions, but instead collects them as much as possible and therefore throws a vector of ParseExceptions. Use Exceptions::print_exceptions() to print them neatly. */
ADLTree Parser::parse(const std::string& filename) {
    // Declare a new root node
    ADLTree root;

    // Populate it with a file node
    ::parse(&root, { filename });

    // Return it
    return root;
}

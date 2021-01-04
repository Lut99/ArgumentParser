/* ADLCOMPILER.cpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 17:24:35
 * Last edited:
 *   04/01/2021, 18:05:39
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The entry point of the ADL Compiler. Handles arguments (yes, compiled
 *   with a previous version of itself :) ) and the general parsing process
 *   by calling the parsers and then each traversal, stopping them if
 *   errors occur.
**/

#include <iostream>

#include "ADLParser.hpp"

#include "BuildSymbolTable.hpp"

using namespace std;
using namespace ArgumentParser;


/* Entry point for the compiler */
int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <file_to_compile>" << endl;
        return EXIT_SUCCESS;
    }

    // Simply call the parser
    ADLTree* tree = Parser::parse(argv[1]);
    
    // Stop if exceptions have been thrown
    if (Exceptions::error_handler.errors() > 0) {
        return EXIT_FAILURE;
    }

    // Next, start with the first traversal
    SymbolTable main_table = traversal_build_symbol_table(tree);
    if (Exceptions::error_handler.errors() > 0) {
        return EXIT_FAILURE;
    }

    // Print the table
    std::cout << "--------------------------------------" << std::endl;
    std::vector<SymbolTable> to_print;
    for (size_t i = 0; i < main_table.size(); i++) {
        std::cout << main_table[i].id << " (" << nodetype_name.at(main_table[i].node_type) << ")" << std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;

    // Alright, it's parsed!
    return EXIT_SUCCESS;
}

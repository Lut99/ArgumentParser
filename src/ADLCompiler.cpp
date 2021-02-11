/* ADLCOMPILER.cpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 17:24:35
 * Last edited:
 *   11/02/2021, 17:35:58
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
#include "CheckReferences.hpp"

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
    tree->symbol_table = traversal_build_symbol_table(tree);
    if (Exceptions::error_handler.errors() > 0) {
        return EXIT_FAILURE;
    }

    #ifdef DEBUG
    // Print the table
    std::cout << std::endl << "Symbol table:" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    tree->symbol_table.print(cout);
    std::cout << "--------------------------------------" << std::endl << std::endl;
    #endif

    // Move to the second traversal; this one checks the references
    traversal_check_references(tree);
    if (Exceptions::error_handler.errors() > 0) {
        return EXIT_FAILURE;
    }

    // Alright, it's parsed!
    return EXIT_SUCCESS;
}

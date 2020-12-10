/* ADLCOMPILER.cpp
 *   by Lut99
 *
 * Created:
 *   10/12/2020, 17:24:35
 * Last edited:
 *   10/12/2020, 17:30:26
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
    /* TBD */

    // Alright, it's parsed!
    return EXIT_SUCCESS;
}

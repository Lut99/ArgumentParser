/* TEST TOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   10/11/2020, 18:03:09
 * Last edited:
 *   12/5/2020, 3:41:33 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   In this file, we test the tokenizer functionality by tokenizing a test
 *   file, and then printing all tokens in said file.
**/

#include <iostream>
#include <fstream>

#include "ADLTokenizer.hpp"

using namespace std;
using namespace ArgumentParser;


int main() {
    // Open the test.adl file
    try {
        Tokenizer tokenizer(new std::ifstream("tests/test.adl"), { "tests/test.adl" });

        size_t last_line = 0;
        while (!tokenizer.eof()) {
            // Get the token
            Token* t;
            try {
                t = tokenizer.pop();
            } catch (Exceptions::ADLCompileError& e) {
                Exceptions::print_error(cerr, e);
                exit(EXIT_FAILURE);
            }

            // If the line changed, then go to a newline
            if (last_line != t->debug.line1) {
                last_line = t->debug.line1;
                cout << endl;
            } else {
                cout << " ";
            }

            // Write it
            cout << *t;

            // Deallocate the token
            delete t;
        }
    } catch(Exceptions::ADLError& e) {
        Exceptions::print_error(cerr, e);
        return EXIT_FAILURE;
    }
}

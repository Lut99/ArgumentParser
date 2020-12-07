/* TEST TOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   10/11/2020, 18:03:09
 * Last edited:
 *   07/12/2020, 21:06:11
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
            t = tokenizer.pop();

            // If the line changed, then go to a newline
            bool at_least_one = false;
            while (last_line < t->debug.line1) {
                ++last_line;
                at_least_one = true;
                cout << endl;
            }
            if (at_least_one) {
                cout << " ";
            }

            // Write it
            cout << t->debug.line1 << ":" << t->debug.col1 << ":" << *t;

            // Deallocate the token
            delete t;
        }
    } catch(Exceptions::ExceptionHandler& e) {
        return EXIT_FAILURE;
    }
}

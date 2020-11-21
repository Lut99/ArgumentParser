/* TEST TOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   10/11/2020, 18:03:09
 * Last edited:
 *   21/11/2020, 13:37:50
 * Auto updated?
 *   Yes
 *
 * Description:
 *   In this file, we test the tokenizer functionality by tokenizing a test
 *   file, and then printing all tokens in said file.
**/

#include <iostream>

#include "ADLTokenizer.hpp"

using namespace std;
using namespace ArgumentParser;


int main() {
    // Open the test.adl file
    Tokenizer tokenizer({ "tests/test.adl" });

    size_t last_line = 0;
    while (!tokenizer.eof()) {
        // Get the token
        Token t;
        try {
            t = tokenizer.pop();
        } catch (Exceptions::ADLCompileError& e) {
            Exceptions::print_error(cerr, e);
            exit(EXIT_FAILURE);
        }

        // If the line changed, then go to a newline
        if (last_line != t.line) {
            last_line = t.line;
            cout << endl;
        } else {
            cout << " ";
        }

        // Write it
        cout << t;
    }
}

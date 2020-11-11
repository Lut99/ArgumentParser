/* TEST TOKENIZER.cpp
 *   by Lut99
 *
 * Created:
 *   10/11/2020, 18:03:09
 * Last edited:
 *   11/11/2020, 15:41:55
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
    Tokenizer tokenizer("tests/test.adl");

    bool first = false;
    size_t last_line = 0;
    while (!tokenizer.eof()) {
        // Print preceding space
        if (!first) { first = true; }
        else { cout << " "; }

        // Get & write a token
        Token t = tokenizer.pop();
        cout << t;

        // If the line changed, then go to a newline
        if (last_line != t.line) {
            last_line = t.line;
            cout << endl;
            first = false;
        }
    }
}

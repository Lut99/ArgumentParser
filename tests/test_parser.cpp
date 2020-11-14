/* TEST PARSER.cpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 14:30:28
 * Last edited:
 *   14/11/2020, 14:39:29
 * Auto updated?
 *   Yes
 *
 * Description:
 *   In this file, we can test-parse the test.adl file.
**/

#include <iostream>

#include "ADLParser.hpp"

using namespace std;
using namespace ArgumentParser;


int main() {
    // Parse the file
    try {
        ADLTree* root = Parser::parse("tests/test.adl");
        delete root;
    } catch (Exceptions::ParseException& e) {
        cerr << e.get_pretty_message();
        exit(EXIT_FAILURE);
    }
}

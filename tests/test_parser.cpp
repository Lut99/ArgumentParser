/* TEST PARSER.cpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 14:30:28
 * Last edited:
 *   14/11/2020, 18:03:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   In this file, we can test-parse the test.adl file.
**/

#include <iostream>

#include "ADLParser.hpp"
#include "ADLExceptions.hpp"

using namespace std;
using namespace ArgumentParser;


int main() {
    // Parse the file
    try {
        ADLTree* root = Parser::parse("tests/test.adl");
        delete root;
    } catch (Exceptions::ADLError& e) {
        Exceptions::print_error(cerr, e);
        exit(EXIT_FAILURE);
    }
}

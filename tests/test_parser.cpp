/* TEST PARSER.cpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 14:30:28
 * Last edited:
 *   26/11/2020, 16:29:17
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
        ADLFile* root = Parser::parse({ "tests/test.adl" });
        if (root == nullptr) { return EXIT_FAILURE; }
        delete root;
        return EXIT_SUCCESS;
    } catch (Exceptions::ADLError& e) {
        Exceptions::print_error(cerr, e);
        return EXIT_FAILURE;
    }
}

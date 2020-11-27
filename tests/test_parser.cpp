/* TEST PARSER.cpp
 *   by Lut99
 *
 * Created:
 *   14/11/2020, 14:30:28
 * Last edited:
 *   27/11/2020, 15:00:13
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


int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " path_to_testfile" << endl;
        return EXIT_SUCCESS;
    }

    // Parse the file
    try {
        ADLTree tree;

        ADLFile* file = Parser::parse({ argv[1] });
        if (file == nullptr) { return EXIT_FAILURE; }
        tree.add_node(file);

        // If successfull, print the tree!
        tree.print(cout);

        return EXIT_SUCCESS;
    } catch (Exceptions::ADLError& e) {
        Exceptions::print_error(cerr, e);
        return EXIT_FAILURE;
    }
}

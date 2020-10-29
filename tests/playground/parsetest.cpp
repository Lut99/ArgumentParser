/* PARSETEST.cpp
 *   by Lut99
 *
 * Created:
 *   24/07/2020, 15:28:42
 * Last edited:
 *   10/29/2020, 1:00:51 AM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   A file that allows freestyle parser testing.
**/

#include <iostream>

#include "../../src/ArgumentParser.hpp"

using namespace std;
using namespace Lut99;


int main(int argc, const char** argv) {
    // Create the parser
    ArgumentParser parser;

    // Add some test arguments
    parser.add_option<String>('t', "test")
        .set_category("Test")
        .set_description("A test option, to test the ArgumentParser.");
    parser.add_flag('1', "first")
        .set_category("Test")
        .set_description("The first in a series of test flags.");
    parser.add_flag('2', "second")
        .set_category("Test")
        .set_description("The second in a series of test flags.");
    
    // Parse the lot
    Arguments args;
    try {
        args = parser.parse(argc, argv);
    } catch (HelpHandledException& e) {
        cout << e.get_help_message();
        return EXIT_SUCCESS;
    } catch (ParseException& e) {
        cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Get the test arguments and display them
    cout << "Hello, world! (" << args.get<std::string>("test") << ")" << std::endl;
    if (args.is_given('1')) { cout << "First flag specified." << std::endl; }
    if (args.is_given('2')) { cout << "Second flag specified." << std::endl; }

    return EXIT_SUCCESS;
}

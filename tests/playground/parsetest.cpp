/* PARSETEST.cpp
 *   by Lut99
 *
 * Created:
 *   24/07/2020, 15:28:42
 * Last edited:
 *   04/11/2020, 17:38:09
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


std::any custom_parser(Tokenizer& input) {
    // Check if there are enough values
    if (input.peek(0).type != TokenType::value || input.peek(1).type != TokenType::value) {
        throw NotEnoughValuesException("name", 2, input.peek(0).type == TokenType::value);
    }

    // First, parse the normal name
    std::string name = std::any_cast<std::string>(parse_string(input));
    // Then, the last name
    name += " " + std::any_cast<std::string>(parse_string(input));
    // Return
    return name;
}


using FirstLast = Type<std::string, custom_parser, STR_T("FirstLast")>;


int main(int argc, const char** argv) {
    // Create the parser
    ArgumentParser parser;

    // Add some test arguments
    parser.add_positional<FirstLast>("name")
        .set_category("Personal")
        .set_description("The name (as first and last name) of the user.");
    parser.add_positional<UInt>("age")
        .set_category("Personal")
        .set_description("The age of the user. May be left out.")
        .set_optional(true);
    parser.add_positional<Char>("gender")
        .set_category("Personal")
        .set_description("The gender of the user (m/f). May be left out.")
        .set_optional(true);

    parser.add_positional<Int>("favourite_numbers")
        .set_category("Favourites")
        .set_description("Favourite numbers of the user. Can be given more than once to indicate more numbers.")
        .set_optional(true)
        .set_variadic(true);
    parser.add_option<String>('f', "favourite_movies")
        .set_category("Favourites")
        .set_description("Favourite movies of the author. Can be given multiple times to give more preferences.")
        .set_variadic(true);
    parser.add_option<FirstLast>('n', "fav_name")
        .set_category("Favourites")
        .set_description("Fovourite people.")
        .set_variadic(true);

    parser.add_option<String>('t', "test")
        .set_category("Test")
        .set_description("A test option, to test the ArgumentParser.");
    parser.add_flag('1', "first")
        .set_category("Test")
        .set_description("The first in a series of test flags.");
    parser.add_flag('2', "second")
        .set_category("Test")
        .set_description("The second in a series of test flags.");
    parser.add_flag("third")
        .set_category("Test")
        .set_description("The third in a series of test flags.");
    
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
    if (args.contains("test")) { cout << "Hello, world! (" << args.get<std::string>("test") << ")" << std::endl; }
    if (args.is_given('1')) { cout << "First flag specified." << std::endl; }
    if (args.is_given('2')) { cout << "Second flag specified." << std::endl; }
    cout << std::endl;

    cout << "Hiya " << args.get<std::string>("name") << "!" << std::endl;
    if (args.contains("age")) {
        cout << "You are " << args.get<unsigned int>("age") << " years old." << std::endl;
    }
    if (args.contains("gender")) {
        cout << "You are a " << (args.get<char>("gender") == 'm' ? "male" : "female") << "." << std::endl;
    }
    cout << std::endl;

    if (args.contains("favourite_numbers")) {
        std::vector<int> numbers = args.getv<int>("favourite_numbers");
        cout << "Your favourite number" << (numbers.size() > 1 ? "s are: " : " is: ");
        for (size_t i = 0; i < numbers.size(); i++) {
            if (i > 0) {
                if (i == numbers.size() - 1) { cout << " and "; }
                else { cout << ", "; }
            }
            cout << numbers[i];
        }
        cout << std::endl;
    }
    if (args.contains("favourite_movies")) {
        std::vector<std::string> movies = args.getv<std::string>("favourite_movies");
        cout << "Your favourite movie" << (movies.size() > 1 ? "s are: " : " is: ");
        for (size_t i = 0; i < movies.size(); i++) {
            if (i > 0) {
                if (i == movies.size() - 1) { cout << " and "; }
                else { cout << ", "; }
            }
            cout << movies[i];
        }
        cout << std::endl;
    }
    if (args.contains("fav_name")) {
        std::vector<std::string> movies = args.getv<std::string>("fav_name");
        cout << "Your favourite person" << (movies.size() > 1 ? "s are: " : " is: ");
        for (size_t i = 0; i < movies.size(); i++) {
            if (i > 0) {
                if (i == movies.size() - 1) { cout << " and "; }
                else { cout << ", "; }
            }
            cout << movies[i];
        }
        cout << std::endl;
    }

    return EXIT_SUCCESS;
}

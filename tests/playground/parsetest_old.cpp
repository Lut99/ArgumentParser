/* PARSETEST.cpp
 *   by Lut99
 *
 * Created:
 *   24/07/2020, 15:28:42
 * Last edited:
 *   09/09/2020, 17:33:18
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

    parser.add_positional<String>("name")
        .set_description("The name of the user using this program.")
        .set_category("Personal Data");
    parser.add_positional<Int>("age")
        .set_description("The age of the user using this program.")
        .set_category("Personal Data");
    parser.add_positional<Char>("gender")
        .set_description("The gender of the user using this program. Enter only 'm' for male or 'f' for female.")
        .set_category("Personal Data");
    parser.add_option<Int>('f', "family")
        .set_description("The age of a family member. Can be specified more than once to specify multiple family members.")
        .set_category("Personal Data")
        .set_any_number(true);
    
    parser.add_positional<String>("food")
        .set_description("The favourite food of the user using this program.")
        .set_category("Favourites")
        .set_default("spaghetti")
        .set_optional(true);
    parser.add_positional<String>("colour")
        .set_description("The favourite colour of the user using this program.")
        .set_category("Favourites")
        .set_default("blue")
        .set_optional(true);
    parser.add_positional<String>("song")
        .set_description("The favourite song of the user using this program.")
        .set_category("Favourites")
        .set_optional(true);

    parser.add_option<String>('c', "class")
        .set_description("Your favourite Star Wars Battlefront II class")
        .set_category("Star Wars Battlefront II");
    parser.add_option<UInt>('b', "battlecount")
        .set_description("The number of battles you played in Star Wasr Battlefront II")
        .set_category("Star Wars Battlefront II");
    parser.add_option<Float>('k', "killdeath")
        .set_description("The ratio of kills / deaths in Star Wars Battlefront II")
        .set_category("Star Wars Battlefront II")
        .set_optional(true)
        .set_default((float) 0.75);
    parser.add_positional<String>("map")
        .set_description("Your favourite Star Wars Battlefront II map. Can be given multiple times to specify multiple maps.")
        .set_category("Star Wars Battlefront II")
        .set_optional(true)
        .set_any_number(true);
    
    parser.validate();
    Arguments args;
    try {
        args = parser.parse(argc, argv);
    } catch(HelpHandledException& e) {
        cout << e.get_help_message();
        return EXIT_SUCCESS;
    } catch(ParseException& e) {
        cout << std::endl << e.what() << std::endl << std::endl;
        return EXIT_FAILURE;
    }

    cout << "Hello, " << args.get<int>("age") << " year-old, " << (args.get<char>("gender") == 'm' ? "male" : "female") << " " << args.get<std::string>("name") << "!" << endl;
    cout << "Your family members are ";
    std::vector<int> ages = args.getp<int>("family");
    for (size_t i = 0; i < ages.size(); i++) {
        if (i > 0) {
            if (i == ages.size() - 1) {
                cout << " and ";
            } else {
                cout << ", ";
            }
        }
        cout << ages[i];
    }
    cout << " years old." << endl << endl;

    cout << "You like " << args.get<std::string>("food") << " food, and the colour " << args.get<std::string>("colour") << "." << endl << endl;
    
    cout << "Your favourite Star Wars Battlefront II class is " << args.get<std::string>("class") << " over " << args.get<unsigned int>("battlecount") << " battles with a k/d of " << args.get<float>("killdeath") << "." << endl;
    if (args.contains("map")) {
        cout << "Your favourite SW:BF II maps are: ";
        std::vector<std::string> maps = args.getp<std::string>("map");
        for (size_t i = 0; i < maps.size(); i++) {
            if (i > 0) {
                if (i == maps.size() - 1) {
                    cout << " and ";
                } else {
                    cout << ", ";
                }
            }
            cout << maps[i];
        }
        cout << "." << std::endl;
    }

    return EXIT_SUCCESS;
}

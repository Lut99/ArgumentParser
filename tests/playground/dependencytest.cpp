/* DEPENDENCYTEST.cpp
 *   by Lut99
 *
 * Created:
 *   25/08/2020, 17:07:54
 * Last edited:
 *   26/08/2020, 15:05:11
 * Auto updated?
 *   Yes
 *
 * Description:
 *   A file for me to tests the functionality of the ArgumentParser's
 *   dependency system.
**/

#include <iostream>
#include <string>
#include "../../src/ArgumentParser.hpp"

using namespace std;
using namespace Lut99;


int main(int argc, const char** argv) {
    // Create the ArgumentParser
    Arguments args;
    try {
        ArgumentParser parser;

        // Add some dependent arguments
        parser.add_option<String>('f', "first_name")
            .set_description("The first name of the user.")
            .set_category("Personal Information")
            .set_optional(true)
            .set_default("John");
        parser.add_option<String>('l', "last_name")
            .set_description("The last name of the user.")
            .set_category("Personal Information")
            .set_optional(true)
            .set_default("Smith");
        parser.add_option<UInt>('a', "age")
            .set_description("The age of the user.")
            .set_category("Personal Information")
            .set_optional(true)
            .set_default((unsigned int) 42);
        
        parser.add_flag('1', "one_way")
            .set_description("When given, the application will go one way.")
            .set_category("Behaviour");
        parser.add_flag('2', "other_way")
            .set_description("When given, the application will go another way.")
            .set_category("Behaviour");
        
        parser.add_positional<String>("best_animal")
            .set_description("The favourite animal of the user.")
            .set_category("Favourites")
            .set_optional(true)
            .set_default("ring-tailed lemur");
        parser.add_positional<String>("worst_animal")
            .set_description("The least-favourite animal of the user.")
            .set_category("Favourites")
            .set_optional(true)
            .set_default("insects");
        parser.add_positional<String>("colour")
            .set_description("The favourite colour of the user.")
            .set_category("Favourites")
            .set_optional(true)
            .set_default("blue");
        
        parser.define_relation(ArgumentRelation::include, "first_name", "last_name");
        parser.define_relation(ArgumentRelation::include, "best_animal", "worst_animal");
        parser.define_relation(ArgumentRelation::exclude, "one_way", "other_way");

        // Parse dat arguments
        args = parser.parse(argc, argv);

    } catch (HelpHandledException& e) {
        cout << e.get_help_message();
        return EXIT_SUCCESS;
    } catch (ParseException& e) {
        cerr << endl << e.what() << endl << endl;
        return EXIT_FAILURE;
    }

    // Use the arguments in a sentence
    cout << "Hello, " << args.get<string>("first_name") << " " << args.get<string>("last_name") << "! My sources tell me you're " << args.get<unsigned int>("age") << " years old!" << endl << endl;

    cout << "Your favourite animal is a " << args.get<string>("best_animal") << ", and your least-favourite animal a " << args.get<string>("worst_animal") << ". Your favourite colour is " << args.get<string>("colour") << "." << endl << endl;

    cout << "The application will go ";
    if (args.contains("one_way")) {
        cout << "one";
    } else if (args.contains("other_way")) {
        cout << "other";
    } else {
        cout << "no";
    }
    cout << " way." << endl << endl;
}

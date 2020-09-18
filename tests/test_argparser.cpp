/* TEST ARGPARSER.cpp
 *   by Lut99
 *
 * Created:
 *   12/04/2020, 23:49:08
 * Last edited:
 *   14/08/2020, 17:27:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file provides both a test and a showcase for the ArgumentParser
 *   library.
**/

#include <iostream>
#include <string>
#include <cmath>

#include "ArgumentParser.hpp"

using namespace std;
using namespace Lut99;


#define ASSERT_PARSED(ARGS, UID, TYPE, VALUE) \
    if (!ARGS.contains(UID)) { \
        std::cout << "[FAIL]" << std::endl << std::endl; \
        std::cerr << "ERROR: Parser did not parse '" UID "'." << std::endl << std::endl; \
        return false; \
    } else if (ARGS.get<TYPE>(UID) != VALUE) { \
        std::cout << "[FAIL]" << std::endl << std::endl; \
        std::cerr << "ERROR: Parser did not parse '" UID "' correctly: got " << args.get<TYPE>(UID) << ", expected " << VALUE << "." << std::endl << std::endl; \
        return false; \
    }

#define ASSERT_PARSED_F(ARGS, UID, TYPE, VALUE) \
    if (!ARGS.contains(UID)) { \
        std::cout << "[FAIL]" << std::endl << std::endl; \
        std::cerr << "ERROR: Parser did not parse '" UID "'." << std::endl << std::endl; \
        return false; \
    } else if (fabs(ARGS.get<TYPE>(UID) - VALUE) >= 0.00000000001) { \
        std::cout << "[FAIL]" << std::endl << std::endl; \
        std::cerr << "ERROR: Parser did not parse '" UID "' correctly: got " << args.get<TYPE>(UID) << ", expected " << VALUE << "." << std::endl << std::endl; \
        return false; \
    }

#define ASSERT_FAILURE(ARGS, UID, TYPE, ERR) \
    try { \
        ARGS.get<TYPE>(UID); \
        std::cout << "[FAIL]" << std::endl << std::endl; \
        std::cerr << "ERROR: Parser did not throw '" #ERR "' for argument with uid \"" UID "\" while we did expect so." << std::endl << std::endl; \
        return false; \
    } catch(ERR& e) { \
        /* All OK */ \
    }


// Simple test of just positionals
bool test_positional_int() {
    int argc = 5;
    const char* argv[] = {"bin/test.out", "5", "-80", "5000000000000", "-----400000000000"};

    // Create the argumentparser
    ArgumentParser parser;

    parser.add_positional<Int>("test1");
    parser.add_positional<Int>("test2");
    parser.add_positional<Long>("test3");
    parser.add_positional<Long>("test4");

    // Parse an argument that way
    Arguments args = parser.parse(argc, argv);

    ASSERT_PARSED(args, "test1", Int, 5);
    ASSERT_PARSED(args, "test2", Int, -80);
    ASSERT_PARSED(args, "test3", Long, 5000000000000);
    ASSERT_PARSED(args, "test4", Long, -400000000000);

    // Now check for an exception
    parser.add_positional<Int>("test5");
    
    parser.handle_errors = false;
    try {
        args = parser.parse(argc, argv);
        // Not OK
        std::cout << "[FAIL]" << std::endl << std::endl;
        std::cerr << "ERROR: Parser did not error while parsing 'test5'." << std::endl << std::endl;
        return false;
    } catch(MissingMandatoryException& e) {
        // OK
    }

    std::cout << "[ OK ]" << endl;
    return true;
}


template <class T, typename = enable_if_t<is_base_of<Type, T>::value> >
bool test_int() {
    typename T::type max_value = numeric_limits<typename T::type>::max();
    typename T::type min_value = numeric_limits<typename T::type>::min();
    std::string overmax_value = to_string(max_value);
    std::string overmin_value = to_string(min_value);
    ++(overmax_value[overmax_value.size() - 1]);
    ++(overmin_value[overmin_value.size() - 1]);

    int argc = 9;
    string sargv[] = {"test_int.out", "  " + to_string(max_value) + "  ", "  0  ", "--" + to_string(min_value), "  77a4  ", overmax_value, overmin_value, to_string(max_value) + "000", to_string(min_value) + "000"};
    const char* argv[argc];
    for (int i = 0; i < argc; i++) {
        argv[i] = sargv[i].c_str();
    }

    // Create the ArgumentParser
    ArgumentParser parser(false, true);
    parser.add_positional<T>("positive");
    parser.add_positional<T>("naught");
    parser.add_positional<T>("negative");
    parser.add_positional<T>("illegalchar");
    parser.add_positional<T>("outofbounds_pos");
    parser.add_positional<T>("outofbounds_neg");
    parser.add_positional<T>("outofbounds_extreme_pos");
    parser.add_positional<T>("outofbounds_extreme_neg");

    // Try to parse them
    Arguments args = parser.parse(argc, argv);

    // See if positive worked
    ASSERT_PARSED(args, "positive", T, max_value);
    // See if naught worked
    ASSERT_PARSED(args, "naught", T, 0);
    // See if negative worked
    ASSERT_PARSED(args, "negative", T, min_value);
    // See if illegalchar worked
    ASSERT_FAILURE(args, "illegalchar", T, IllegalCharException);
    // See if outofbounds_pos worked
    ASSERT_FAILURE(args, "outofbounds_pos", T, OutOfBoundsException);
    // See if outofbounds_neg worked
    ASSERT_FAILURE(args, "outofbounds_neg", T, OutOfBoundsException);
    // See if outofbounds_extreme_pos worked
    ASSERT_FAILURE(args, "outofbounds_extreme_pos", T, OutOfBoundsException);
    // See if outofbounds_extreme_neg worked
    ASSERT_FAILURE(args, "outofbounds_extreme_neg", T, OutOfBoundsException);

    // Done!
    cout << "[ OK ]" << endl;
    return true;
}


template <class T, typename = enable_if_t<is_base_of<Type, T>::value> >
bool test_uint() {
    typename T::type max_value = numeric_limits<typename T::type>::max();
    std::string overmax_value = to_string(max_value);
    ++(overmax_value[overmax_value.size() - 1]);

    int argc = 6;
    string sargv[] = {"test_uint.out", "  " + to_string(max_value) + "  ", "  0  ", "  77a4  ", overmax_value, to_string(max_value) + "000"};
    const char* argv[argc];
    for (int i = 0; i < argc; i++) {
        argv[i] = sargv[i].c_str();
    }

    // Create the ArgumentParser
    ArgumentParser parser(false, true);
    parser.add_positional<T>("positive");
    parser.add_positional<T>("naught");
    parser.add_positional<T>("illegalchar");
    parser.add_positional<T>("outofbounds_pos");
    parser.add_positional<T>("outofbounds_extreme_pos");

    // Try to parse them
    Arguments args = parser.parse(argc, argv);

    // See if positive worked
    ASSERT_PARSED(args, "positive", T, max_value);
    // See if naught worked
    ASSERT_PARSED(args, "naught", T, 0);
    // See if illegalchar worked
    ASSERT_FAILURE(args, "illegalchar", T, IllegalCharException);
    // See if outofbounds_pos worked
    ASSERT_FAILURE(args, "outofbounds_pos", T, OutOfBoundsException);
    // See if outofbounds_extreme_pos worked
    ASSERT_FAILURE(args, "outofbounds_extreme_pos", T, OutOfBoundsException);

    // Done!
    cout << "[ OK ]" << endl;
    return true;
}


bool test_float() {
    float max_value = numeric_limits<float>::max();

    int argc = 9;
    string sargv[] = {"test_float.out", "  " + to_string(max_value) + "  ", "  0.0  ", "-0.00000000234287648234", "  77a4  ",};
    const char* argv[argc];
    for (int i = 0; i < argc; i++) {
        argv[i] = sargv[i].c_str();
    }

    // Create the ArgumentParser
    ArgumentParser parser(false, true);
    parser.add_positional<Float>("positive");
    parser.add_positional<Float>("naught");
    parser.add_positional<Float>("negative");
    parser.add_positional<Float>("illegalchar");

    // Try to parse them
    Arguments args = parser.parse(argc, argv);

    // See if positive worked
    ASSERT_PARSED_F(args, "positive", Float, max_value);
    // See if naught worked
    ASSERT_PARSED_F(args, "naught", Float, 0);
    // See if negative worked
    ASSERT_PARSED_F(args, "negative", Float, stof("-0.00000000234287648234"));
    // See if illegalchar worked
    ASSERT_FAILURE(args, "illegalchar", Float, IllegalCharException);

    // Done!
    cout << "[ OK ]" << endl;
    return true;
}


bool test_double() {
    double max_value = numeric_limits<double>::max();

    int argc = 9;
    string sargv[] = {"test_double.out", "  " + to_string(max_value) + "  ", "  0.0  ", "-0.00000000234287648234", "  77a4  ",};
    const char* argv[argc];
    for (int i = 0; i < argc; i++) {
        argv[i] = sargv[i].c_str();
    }

    // Create the ArgumentParser
    ArgumentParser parser(false, true);
    parser.add_positional<Double>("positive");
    parser.add_positional<Double>("naught");
    parser.add_positional<Double>("negative");
    parser.add_positional<Double>("illegalchar");

    // Try to parse them
    Arguments args = parser.parse(argc, argv);

    // See if positive worked
    ASSERT_PARSED_F(args, "positive", Double, max_value);
    // See if naught worked
    ASSERT_PARSED_F(args, "naught", Double, 0);
    // See if negative worked
    ASSERT_PARSED_F(args, "negative", Double, stof("-0.00000000234287648234"));
    // See if illegalchar worked
    ASSERT_FAILURE(args, "illegalchar", Double, IllegalCharException);

    // Done!
    cout << "[ OK ]" << endl;
    return true;
}


bool test_bool() {
    int argc =13;
    const char* argv[] = { "test_bool.out", "true", "     yes  ", "  y", "1     ", "     false   ", "  no", "n", "0   ", "true true", "tr ue", "tryu", "kahs" };

    // Create the ArgumentParser
    ArgumentParser parser(false, true);
    parser.add_positional<Bool>("true1");
    parser.add_positional<Bool>("true2");
    parser.add_positional<Bool>("true3");
    parser.add_positional<Bool>("true4");
    parser.add_positional<Bool>("false1");
    parser.add_positional<Bool>("false2");
    parser.add_positional<Bool>("false3");
    parser.add_positional<Bool>("false4");
    parser.add_positional<Bool>("error1");
    parser.add_positional<Bool>("error2");
    parser.add_positional<Bool>("error3");
    parser.add_positional<Bool>("error4");

    // Try to parse them
    Arguments args = parser.parse(argc, argv);

    // See if positive worked
    ASSERT_PARSED(args, "true1", Bool, true);
    ASSERT_PARSED(args, "true2", Bool, true);
    ASSERT_PARSED(args, "true3", Bool, true);
    ASSERT_PARSED(args, "true4", Bool, true);
    ASSERT_PARSED(args, "false1", Bool, false);
    ASSERT_PARSED(args, "false2", Bool, false);
    ASSERT_PARSED(args, "false3", Bool, false);
    ASSERT_PARSED(args, "false4", Bool, false);
    ASSERT_FAILURE(args, "error1", Bool, IllegalValueException);
    ASSERT_FAILURE(args, "error2", Bool, IllegalValueException);
    ASSERT_FAILURE(args, "error3", Bool, IllegalValueException);
    ASSERT_FAILURE(args, "error4", Bool, IllegalValueException);

    // Done!
    cout << "[ OK ]" << endl;
    return true;
}


bool test_char() {
    int argc = 6;
    const char* argv[] = { "test_char.out", "a", "b", "c", "aa", "" };

    // Create the ArgumentParser
    ArgumentParser parser(false, true);
    parser.add_positional<Char>("a");
    parser.add_positional<Char>("b");
    parser.add_positional<Char>("c");
    parser.add_positional<Char>("toomany");
    parser.add_positional<Char>("toofew");

    // Try to parse them
    Arguments args = parser.parse(argc, argv);

    // See if positive worked
    ASSERT_PARSED(args, "a", Char, 'a');
    ASSERT_PARSED(args, "b", Char, 'b');
    ASSERT_PARSED(args, "c", Char, 'c');
    ASSERT_FAILURE(args, "toomany", Char, IllegalValueException);
    ASSERT_FAILURE(args, "toofew", Char, IllegalValueException);

    // Done!
    cout << "[ OK ]" << endl;
    return true;
}


bool test_string() {
    int argc = 6;
    const char* argv[] = { "test_string.out", "aaa", "bbb", "ccc", "Hello there!\nGeneral Kenobi!", "" };

    // Create the ArgumentParser
    ArgumentParser parser(false, true);
    parser.add_positional<String>("aaa");
    parser.add_positional<String>("bbb");
    parser.add_positional<String>("ccc");
    parser.add_positional<String>("hellothere");
    parser.add_positional<String>("toofew");

    // Try to parse them
    Arguments args = parser.parse(argc, argv);

    // See if positive worked
    ASSERT_PARSED(args, "aaa", String, "aaa");
    ASSERT_PARSED(args, "bbb", String, "bbb");
    ASSERT_PARSED(args, "ccc", String, "ccc");
    ASSERT_PARSED(args, "hellothere", String, "Hello there!\nGeneral Kenobi!");
    ASSERT_FAILURE(args, "toofew", String, IllegalValueException);

    // Done!
    cout << "[ OK ]" << endl;
    return true;
}


int main() {
    cout << "   Testing positionals...    " << flush;
    test_positional_int();
    cout << endl;

    cout << "   Testing Byte-type...      " << flush;
    test_int<Byte>();
    cout << "   Testing Short-type...     " << flush;
    test_int<Short>();
    cout << "   Testing Int-type...       " << flush;
    test_int<Int>();
    cout << "   Testing Long-type...      " << flush;
    test_int<Long>();
    cout << "   Testing LongLong-type...  " << flush;
    test_int<LongLong>();
    cout << endl;

    cout << "   Testing UByte-type...     " << flush;
    test_uint<UByte>();
    cout << "   Testing UShort-type...    " << flush;
    test_uint<UShort>();
    cout << "   Testing UInt-type...      " << flush;
    test_uint<UInt>();
    cout << "   Testing ULong-type...     " << flush;
    test_uint<ULong>();
    cout << "   Testing ULongLong-type... " << flush;
    test_uint<ULongLong>();
    cout << endl;

    cout << "   Testing Float-type...     " << flush;
    test_float();
    cout << "   Testing Double-type...    " << flush;
    test_double();

    cout << "   Testing Bool-type...      " << flush;
    test_bool();
    cout << "   Testing Char-type...      " << flush;
    test_char();
    cout << "   Testing String-type...    " << flush;
    test_string();
    cout << endl;

    return 0;
}

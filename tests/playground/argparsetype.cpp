/* ARGPARSETYPE.cpp
 *   by Lut99
 *
 * Created:
 *   19/07/2020, 16:19:45
 * Last edited:
 *   20/07/2020, 13:49:16
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File to test the validity of our DerivedType idea
**/


#include <iostream>
#include <vector>
#include <string>
#include <any>
#include <type_traits>


#define _T(STRING) decltype(STRING ## _tstr)


/* String placeholder type. */
template <char... chars>
using tstring = std::integer_sequence<char, chars...>;

template <typename T, T... chars>
constexpr tstring<chars...> operator""_tstr() { return { }; }


/* Type marks the base of all ArgumentParser-supported types. */
struct Type {};

/* Primary template for the DerivedType class. */
template <typename, size_t, std::any (*)(std::vector<std::string>), typename...>
struct DerivedType : public Type {};

/* Template for the derived type of Type. Can be customized to reflect the target type. */
template <typename T, size_t N_VALUES, std::any (*PARSE_FUNC)(std::vector<std::string>), char... PLACEHOLDER>
struct DerivedType<T, N_VALUES, PARSE_FUNC, tstring<PLACEHOLDER...>> : public Type {
    /* The type that this DerivedType returns. */
    static constexpr size_t t_info = typeid(T).hash_code();
    /* The parser for this type. */
    static constexpr std::any (*parse_func)(std::vector<std::string>) = PARSE_FUNC;
    /* The number of values that this type uses. */
    static constexpr size_t n_values = N_VALUES;
    /* Placeholder text that names the type in the help function. */
    static constexpr char placeholder[] = { PLACEHOLDER..., '\0' };
};


// Function which uses the given type to print a neat lil' message
template <class T, typename = std::enable_if_t<std::is_base_of<Type, T>::value> >
void print() {
    // Call the type to print something, dependend on the type
    T::parse_func(std::vector<std::string>({"Hello there, Mrs " + std::string(T::placeholder) + "!"}));
}


// Normal print function
std::any normal(std::vector<std::string> text) {
    std::cout << text[0] << std::endl;
    return std::any(nullptr);
}

// Print function with an addition
std::any extra(std::vector<std::string> text) {
    std::cout << text[0] << " Hello, Mrs. Whitfield!" << std::endl;
    return std::any(nullptr);
}


int main() {
    // Call the normal print function
    print<DerivedType<void, 1, normal, _T("Morris")>>();
    print<DerivedType<void, 1, extra, _T("Harrington")>>();

    return 0;
}

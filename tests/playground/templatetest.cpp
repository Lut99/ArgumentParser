/* TEMPLATE TEST.cpp
 *   by Anonymous
 *
 * Created:
 *   04/11/2020, 16:48:02
 * Last edited:
 *   04/11/2020, 17:06:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Allows me to test various template-related tests.
**/

#include <iostream>
#include <cstdlib>
#include <utility>
#include <tuple>

using namespace std;


template <size_t N, typename = std::make_index_sequence<N>>
struct Tokens;

template <size_t N, size_t... S>
struct Tokens<N, std::index_sequence<S...>> {
    std::tuple<std::size_t(decltype(S)...)>> values;

    
};


// Let's see if we can make a recursive extender, that return a sequence of 0, ..., N as tuple
template <size_t N>
Tokens<N> sequence() {
    sequence<N - 1>();
    cout << "Hello, " << N << "!" << endl;
}
template <>
Tokens<0> sequence<0>() {
    return Tokens<0>();
}


int main() {
    Tokens<1>().values;
    Tokens<5> result = sequence<5>();
}

#include <iostream>
#include "../include/Vector.h"

int main()
{
    Vector<int> vec = {1, 3, 5, 7};
    for (auto citr = vec.cbegin(); citr != vec.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    for (auto critr = vec.crbegin(); critr != vec.crend(); ++critr) {
        std::cout << *critr << ' ';
    }
    std::cout << '\n';
    std::cout << "Hello World!\n";

    Vector<int> empty;

    std::cout << (empty.begin() == empty.end()) << '\n';
    std::cout << (empty.rbegin() == empty.rend()) << '\n';

    Vector<int> v{1,2,3,4};

    v.insert(v.begin(), 3, 9);
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    v.insert(v.begin()+2, 3, 9);
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    v.insert(v.end(), 3, 9);
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    return 0;
}

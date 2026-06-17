#include <iostream>
#include "../include/Vector.h"
#include <forward_list>
#include <list>
#include <sstream>

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

    Vector<int> v1{1, 4};

    std::forward_list<int> fl{2, 3};

    v1.insert(v1.begin() + 1, fl.begin(), fl.end());
    for (auto citr = v1.cbegin(); citr != v1.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';

    v.clear();
    v = {1,4};
    std::list<int> lst{2,3};

    v.insert(v.begin() + 1, lst.begin(), lst.end());
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';

    v.clear();
    v = {5,4};
    std::istringstream iss("2 3");

    std::istream_iterator<int> first(iss);
    std::istream_iterator<int> last;
    v.insert(v.begin() + 1, first, last);
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';

    v.clear();
    v = {3,4};
    fl.clear();
    fl = {1,2};
    v.insert(v.begin(), fl.begin(), fl.end());
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    v.clear();
    v = {7,9};
    int arr[]{2,3};

    v.insert(v.begin()+1,
             std::begin(arr),
             std::end(arr));
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';

    v.clear();
    v = {173,29};
    fl.clear();
    fl = {17,37};
    v.insert(v.end(), fl.begin(), fl.end());
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';

    v.clear();
    v.reserve(4);

    v.push_back(11);
    v.push_back(43);

    fl = {2,3,5,6};

    v.insert(v.begin() + 1,
             fl.begin(),
             fl.end());
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';

    v = {1,2,3};
    v.insert(v.begin() + 1,
             v.begin(),
             v.end());

    for (auto x : v)
    {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    return 0;
}

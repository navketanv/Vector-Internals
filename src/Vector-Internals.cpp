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
    v.insert(v.begin() + 1, v.begin(), v.end());
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    v = {1,2,3,4,5};
    v.erase(v.begin() + 1, v.begin() + 3);
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    v = {1,2,3,4};

    auto it = v.erase(v.begin(), v.end());

    std::cout << v.size() << '\n';
    std::cout << (it == v.end()) << '\n';
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    // erase first element
    v = {1,2,3,4};
    v.erase(v.begin());
    // expected: 2 3 4
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    // erase last element
    v = {1,2,3,4};
    v.erase(v.end() - 1);
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    // expected: 1 2 3
    // erase middle element
    v = {1,2,3,4};
    v.erase(v.begin() + 2);
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    // expected: 1 2 4
    // erase empty range
    v = {1,2,3,4};
    it = v.erase(v.begin() + 1, v.begin() + 1);
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    std::cout << *it << '\n';
    // vector unchanged
    // it should point to element 2
    // erase suffix
    v = {1,2,3,4,5};
    v.erase(v.begin() + 2, v.end());
    // expected: 1 2
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    // erase prefix
    v = {1,2,3,4,5};
    v.erase(v.begin(), v.begin() + 3);
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    // expected: 4 5
    v = {1,2,3};
    it = v.erase(v.begin()+1, v.begin()+1);
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    std::cout << *it << '\n';
    return 0;
}

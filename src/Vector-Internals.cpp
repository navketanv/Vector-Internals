#include <iostream>
#include "../include/Vector.h"
#include <forward_list>
#include <list>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <random>

struct CopyPreferred
{
    static inline std::size_t copies{};
    static inline std::size_t moves{};

    CopyPreferred() = default;

    CopyPreferred(const CopyPreferred&)
    {
        ++copies;
    }

    CopyPreferred(CopyPreferred&&)
    {
        ++moves;
    }
};

struct MovePreferred
{
    static inline std::size_t copies{};
    static inline std::size_t moves{};

    MovePreferred() = default;

    MovePreferred(const MovePreferred&)
    {
        ++copies;
    }

    MovePreferred(MovePreferred&&) noexcept
    {
        ++moves;
    }
};

struct MoveOnly
{
    static inline std::size_t copies{};
    static inline std::size_t moves{};

    MoveOnly() = default;
    MoveOnly(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&&)
    {
        ++moves;
    }
};

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
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    v.insert(v.begin(), 3, 9);
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    v.insert(v.begin()+2, 3, 88);
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    v.insert(v.end(), 3, 10);
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    std::cout << "sorting\n";
    std::sort(v.begin(), v.end());
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    std::cout << "reverse\n";
    std::reverse(v.begin(), v.end());
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
//    std::distance(v.begin(), v.end());
//    std::advance(it, 3);
//    std::find(v.begin(), v.end(), 88);
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

    std::cout << "test\n";
    Vector<int> test{1,2,3,4};

    auto testit = test.begin();

    auto testit2 = testit + 2;

    std::cout << *(testit2) << '\n';
    std::cout << (testit2 - testit) << '\n';

    std::cout << "Rotate : \n";
    v = {1,2,3,4,5};
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    std::rotate(v.begin(), v.begin() + 2, v.end());
    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    it = std::lower_bound(v.begin(), v.end(), 10);
    if (it != v.cend()) {
        std::cout << "found 10\n";
    } else {
        std::cout << "not found 10\n";
    }
    v.insert(v.end(), 3, 10);
    for (auto citr = v.cbegin(); citr != v.cend(); ++citr) {
        std::cout << *citr << ' ';
    }
    std::cout << '\n';
    it = std::lower_bound(v.begin(), v.end(), 10);
    if (it != v.cend()) {
        std::cout << "found 10 at position = " << std::distance(v.begin(), it) <<'\n';
    } else {
        std::cout << "not found 10\n";
    }
    std::cout << "distance\n";
    v = {1,2,3,4,5};

    std::cout << std::distance(v.begin(), v.end()) << '\n';
    std::cout << std::distance(v.cbegin(), v.cend()) << '\n';
    std::cout << "advance\n";

    auto adv = v.begin();

    std::advance(adv, 3);

    std::cout << *adv << '\n';
    std::cout << "find\n";

    auto fit = std::find(v.begin(), v.end(), 4);

    if (fit != v.end()) {
        std::cout << "found " << *fit << '\n';
    }
    std::cout << "binary search\n";

    std::sort(v.begin(), v.end());

    std::cout << std::binary_search(v.begin(), v.end(), 3) << '\n';
    std::cout << std::binary_search(v.begin(), v.end(), 10) << '\n';
    std::cout << "copy\n";

    Vector<int> dst(5);

    std::copy(v.begin(), v.end(), dst.begin());

    for (auto x : dst) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    std::cout << "equal\n";

    std::cout << std::equal(v.begin(), v.end(), dst.begin()) << '\n';
    std::cout << "min max\n";

    auto minit = std::min_element(v.begin(), v.end());
    auto maxit = std::max_element(v.begin(), v.end());

    std::cout << *minit << ' ' << *maxit << '\n';
    std::cout << "accumulate\n";

    std::cout << std::accumulate(v.begin(), v.end(), 0) << '\n';

    std::cout << "nth element\n";

    v = {7,4,9,1,5,3};
    Vector<int> v2 = v;

    v2 = v;

    std::nth_element(v.begin(),
                     v.begin() + 2,
                     v.end());

    std::cout << v[2] << '\n';

    std::cout << "iter swap\n";

    v = {1,2,3,4};

    std::iter_swap(v.begin(),
                   v.begin() + 3);

    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    static_assert(std::random_access_iterator<Vector<int>::iterator>);
    static_assert(std::random_access_iterator<Vector<int>::const_iterator>);
    static_assert(std::bidirectional_iterator<Vector<int>::iterator>);
    static_assert(std::forward_iterator<Vector<int>::iterator>);

    v = {1,2,3};

    Vector<int>::iterator itr = v.begin();
    Vector<int>::const_iterator citr = v.cbegin();

    std::cout << (itr == citr) << '\n';
    std::cout << (citr == itr) << '\n';
    std::cout << (citr - itr) << '\n';
    std::cout << (itr - citr) << '\n';

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> sizeDist(1, 2000);
    std::uniform_int_distribution<std::size_t> contentDist(1, 1000);

    auto randomVector = [&](std::size_t size) -> std::vector<int> {
        std::vector<int> vec(size, 0);
        for (std::size_t index = 0; index < size; ++index) {
            vec[index] = contentDist(gen);
        }
        return vec;
    };
/*
    for (int trial = 0; trial < 10000; ++trial)
    {
        std::size_t initialSize = sizeDist(gen);
        std::size_t rangeSize = sizeDist(gen);
        std::vector<int> sv = randomVector(initialSize);
        Vector<int> mv(sv.begin(), sv.end());
        assert(sv.size() == mv.size());
        auto range = randomVector(rangeSize);
        std::uniform_int_distribution<std::size_t> posDist(0, sv.size());

        auto pos = posDist(gen);

        sv.insert(sv.begin() + pos,
                  range.begin(),
                  range.end());

        mv.insert(mv.begin() + pos,
                  range.begin(),
                  range.end());
        assert(sv.size() == mv.size());
        mv.insert(
            mv.begin() + pos,
            mv.begin(),
            mv.end());
        sv.insert(
            sv.begin() + pos,
            sv.begin(),
            sv.end());
        std::vector<int> test1(sv.begin(), sv.end());
        std::vector<int> test2(mv.begin(), mv.end());
        assert(test1==test2);
        assert(
            std::equal(
                sv.begin(),
                sv.end(),
                mv.begin()));
    }
*/
    Vector<CopyPreferred> vcp;
    for (int i = 0; i < 100000; ++i)
    {
        vcp.push_back(CopyPreferred{});
    }

    std::cout << "CopyPreferred::copies = " << CopyPreferred::copies << '\n';
    std::cout << "CopyPreferred::moves = " << CopyPreferred::moves << '\n';

    Vector<MovePreferred> vmp;
    for (int i = 0; i < 100000; ++i)
    {
        vmp.push_back(MovePreferred{});
    }

    std::cout << "MovePreferred::copies = " << MovePreferred::copies << '\n';
    std::cout << "MovePreferred::moves = " << MovePreferred::moves << '\n';

    Vector<MoveOnly> vmo;
    for (int i = 0; i < 100000; ++i)
    {
        vmo.push_back(MoveOnly{});
    }

    std::cout << "MoveOnly::copies = " << MoveOnly::copies << '\n';
    std::cout << "MoveOnly::moves = " << MoveOnly::moves << '\n';

    return 0;
}

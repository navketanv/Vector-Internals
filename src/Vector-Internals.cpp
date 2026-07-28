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

void testConstructionAndIterators() {
    Vector<int> vec{1, 3, 5, 7};
    Vector<int> expected{1, 3, 5, 7};

    assert(std::equal(vec.cbegin(), vec.cend(), expected.cbegin(), expected.cend()));
    expected = {7, 5, 3, 1};
    assert(std::equal(vec.crbegin(), vec.crend(), expected.cbegin(), expected.cend()));

    vec.clear();

    assert(vec.begin() == vec.end());
    assert(vec.cbegin() == vec.cend());
    assert(vec.rbegin() == vec.rend());
    assert(vec.crbegin() == vec.crend());

    Vector<int> v{1, 2, 3, 4};
    expected = {1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));
}

void testInsert() {
    Vector<int> vec = {1, 3, 5, 7};
    Vector<int> expected{1, 3, 5, 7};

    Vector<int> v{1, 2, 3, 4};

    v.insert(v.begin(), 3, 9);
    expected = {9, 9, 9, 1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.insert(v.begin()+2, 3, 88);
    expected = {9, 9, 88, 88, 88, 9, 1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.insert(v.end(), 3, 10);
    expected = {9, 9, 88, 88, 88, 9, 1, 2, 3, 4, 10, 10, 10};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));
    v = {1, 4};
    std::forward_list<int> fl{2, 3};
    v.insert(v.begin() + 1, fl.begin(), fl.end());
    expected = {1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1, 4};
    std::list<int> lst{2, 3};

    v.insert(v.begin() + 1, lst.begin(), lst.end());
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {5,4};
    std::istringstream iss("2 3");

    std::istream_iterator<int> first(iss);
    std::istream_iterator<int> last;
    v.insert(v.begin() + 1, first, last);
    expected = {5, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {3,4};
    fl = {1,2};
    v.insert(v.begin(), fl.begin(), fl.end());
    expected = {1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.clear();
    v = {7,9};
    int arr[]{2,3};

    v.insert(v.begin()+1, std::begin(arr), std::end(arr));
    expected = {7, 2, 3, 9};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.clear();
    v = {173,29};
    fl.clear();
    fl = {17,37};
    v.insert(v.end(), fl.begin(), fl.end());
    expected = {173, 29, 17, 37};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.clear();
    v.reserve(4);

    v.push_back(11);
    v.push_back(43);
    v.emplace_back(27);
    v.emplace_back(28);

    fl = {2,3,5,6};

    v.insert(v.begin() + 1, fl.begin(), fl.end());
    expected = {11, 2, 3, 5, 6, 43, 27, 28};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1,2,3};
    v.insert(v.begin() + 1, v.begin(), v.end());
    expected = {1, 1, 2, 3, 2, 3};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1, 2, 3, 4};
    v.insert(v.begin(), 100);
    expected = {100, 1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1, 2, 3, 4};
    v.insert(v.begin(), 24, 100);
    expected = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1, 2, 3, 4};
    v.insert(v.begin() + 2, 7, 100);
    expected = {1, 2, 100, 100, 100, 100, 100, 100, 100, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1, 2, 3, 4};
    v.insert(v.end(), 100);
    expected = {1, 2, 3, 4, 100};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1, 2, 3, 4};
    v.insert(v.end(), 17, 100);
    expected = {1, 2, 3, 4, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));


    v.clear();
    v.insert(v.begin(), 42);
    expected = {42};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.clear();
    v.insert(v.begin(), 7, 42);
    expected = {42, 42, 42, 42, 42, 42, 42};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.resize(10, 91);
    expected = {42, 42, 42, 42, 42, 42, 42, 91, 91, 91};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.clear();
    v.reserve(1000);
    v.insert(v.end(), 19);
    expected = {19};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));
    v.insert(v.begin(), 19);
    expected = {19, 19};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));
    v.insert(v.begin() + 1, 4, 19);
    expected = {19, 19, 19, 19, 19, 19};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.insert(v.begin() + 2, 12, 4119);
    expected = {19, 19, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 19, 19, 19, 19};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.insert(v.end(), fl.begin(), fl.end());
    expected = {19, 19, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 19, 19, 19, 19, 2, 3, 5, 6};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));


    v.insert(v.begin() + 2, fl.begin(), fl.end());
    expected = {19, 19, 2, 3, 5, 6, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 19, 19, 19, 19, 2, 3, 5, 6};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.insert(v.begin() + 2, v.begin(), v.end());
    expected = {19, 19, 19, 19, 2, 3, 5, 6, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 19, 19, 19, 19, 2, 3,
                5, 6, 2, 3, 5, 6, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 19, 19, 19, 19, 2, 3, 5, 6};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    Vector<int> v2 = v;
    expected = {19, 19, 19, 19, 2, 3, 5, 6, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 19, 19, 19, 19, 2, 3,
                5, 6, 2, 3, 5, 6, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 4119, 19, 19, 19, 19, 2, 3, 5, 6};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));
}

void testAlgorithm() {
    Vector<int> expected;
    Vector<int> v{1, 2, 3, 4};

    v.insert(v.begin(), 3, 9);
    expected = {9, 9, 9, 1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.insert(v.begin()+2, 3, 88);
    expected = {9, 9, 88, 88, 88, 9, 1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v.insert(v.end(), 3, 10);
    expected = {9, 9, 88, 88, 88, 9, 1, 2, 3, 4, 10, 10, 10};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    std::sort(v.begin(), v.end());
    expected = {1, 2, 3, 4, 9, 9, 9, 10, 10, 10, 88, 88, 88};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    std::reverse(v.begin(), v.end());
    expected = {88, 88, 88, 10, 10, 10, 9, 9, 9, 4, 3, 2, 1};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    Vector<int> test{1, 2, 3, 4};
    auto testit = test.begin();
    auto testit2 = testit + 2;
    assert(*(testit2) == test[2]);
    assert((testit2 - testit) == 2);

    v = {1, 2, 3, 4, 5};
    expected = {1, 2, 3, 4, 5};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));
    std::rotate(v.begin(), v.begin() + 2, v.end());

    expected = {3, 4, 5, 1, 2};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    auto it = std::lower_bound(v.begin(), v.end(), 10);
    assert(it == v.cend());

    v.insert(v.end(), 3, 10);
    expected = {3, 4, 5, 1, 2, 10, 10, 10};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    it = std::lower_bound(v.begin(), v.end(), 10);
    assert(std::distance(v.begin(), it) == 5);


    v = {1, 2, 3, 4, 5};
    assert(std::distance(v.begin(), v.end()) == v.size());
    assert(std::distance(v.cbegin(), v.cend()) == v.size());
    assert(std::distance(v.crbegin(), v.crend()) == v.size());

    auto adv = v.begin();
    std::advance(adv, 3);
    assert((*adv) == v[3]);

    auto fit = std::find(v.begin(), v.end(), 4);

    if (fit != v.end()) {
        std::size_t index = std::distance(v.begin(), fit);
        assert((*fit) == v[index]);
    } else {
        assert(fit == v.end());
    }

    std::sort(v.begin(), v.end());
    auto cit = std::find(v.cbegin(), v.cend(), 3);
    if (cit != v.cend()) {
        assert(std::binary_search(v.begin(), v.end(), 3) == std::true_type{});
    } else {
        assert(std::binary_search(v.begin(), v.end(), 3) == std::false_type{});
    }
    cit = std::find(v.cbegin(), v.cend(), 10);
    if (cit != v.cend()) {
        assert(std::binary_search(v.begin(), v.end(), 10) == std::true_type{});
    } else {
        assert(std::binary_search(v.begin(), v.end(), 10) == std::false_type{});
    }

    Vector<int> dst(v.size());
    std::copy(v.cbegin(), v.cend(), dst.begin());
    assert(std::equal(v.cbegin(), v.cend(), dst.cbegin(), dst.cend()));

    std::sort(v.begin(), v.end());
    auto minit = std::min_element(v.cbegin(), v.cend());
    auto maxit = std::max_element(v.cbegin(), v.cend());
    assert((*minit) == v[0]);
    assert((*maxit) == v[v.size() - 1]);

    assert(std::accumulate(v.cbegin(), v.cend(), 0) == 15);

    v = {27, 9, 33, 2, 8, 5, 1, 6, 4};
    Vector<int> v2 = v;

    std::sort(v2.begin(), v2.end());
    std::nth_element(v.begin(), v.begin() + 4, v.end());
    assert(v2[4] == v[4]);
    assert(v[0] <= v[4]);
    assert(v[5] >= v[4]);
    assert(v[v.size() - 1] >= v[4]);

    v = {27, 9, 33, 2, 8, 5, 1, 6, 4};
    std::nth_element(v.begin(), v.begin() + 2, v.end());
    assert(v2[2] == v[2]);
    assert(v[0] <= v[2]);
    assert(v[v.size() - 1] >= v[2]);

    v = {27, 9, 33, 2, 8, 5, 1, 6, 4};
    v2 = v;
    std::iter_swap(v.begin(),
                   v.begin() + 3);
    assert((*v.begin()) == (*(v2.begin() + 3)));
    assert((*(v.begin() + 3)) == (*v2.begin()));

    static_assert(std::random_access_iterator<Vector<int>::iterator>);
    static_assert(std::random_access_iterator<Vector<int>::const_iterator>);
    static_assert(std::bidirectional_iterator<Vector<int>::iterator>);
    static_assert(std::forward_iterator<Vector<int>::iterator>);

    v = {1,2,3};

    Vector<int>::iterator itr = v.begin();
    Vector<int>::const_iterator citr = v.cbegin();

    assert(itr == citr);
    assert(citr == itr);
    assert((citr - itr) == 0);
    assert((itr - citr) == 0);
}

void testErase() {
    Vector<int> v{1, 2, 3, 4, 5};
    Vector<int> expected;

    v = {1, 2, 3, 4, 5};
    v.erase(v.begin() + 1, v.begin() + 3);
    expected = {1, 4, 5};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1, 2, 3, 4};
    auto it = v.erase(v.begin(), v.end());

    assert(v.size() == 0);
    assert(it == v.end());

    // erase first element
    v = {1,2,3,4};
    v.erase(v.begin());
    expected = {2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    // erase last element
    v = {1, 2, 3, 4};
    v.erase(v.end() - 1);
    expected = {1, 2, 3};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    // erase middle element
    v = {1, 2, 3, 4};
    v.erase(v.begin() + 2);
    expected = {1, 2, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    // erase empty range
    v = {1, 2, 3, 4};
    it = v.erase(v.begin() + 1, v.begin() + 1);
    expected = {1, 2, 3, 4};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    // it should point to element 2
    // erase suffix
    v = {1, 2, 3, 4, 5};
    v.erase(v.begin() + 2, v.end());
    expected = {1, 2};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    // erase prefix
    v = {1, 2, 3, 4, 5};
    v.erase(v.begin(), v.begin() + 3);
    expected = {4, 5};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));

    v = {1, 2, 3};
    it = v.erase(v.begin() + 1, v.begin() + 1);
    expected = {1, 2, 3};
    assert(std::equal(v.cbegin(), v.cend(), expected.cbegin(), expected.cend()));
}

void loadTest() {

    Vector<int> vec;
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
}

int main()
{
    testConstructionAndIterators();
    testInsert();
    testAlgorithm();
    testErase();
    loadTest();
    return 0;
}

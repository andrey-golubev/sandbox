#include "test.hpp"

#include "random_utils.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <cassert>

namespace detail {
template<typename I, typename Compare>
void merge(I first1, I last1, I first2, I last2, I d_first, Compare cmp) {
    // [first1, last1) and [first2, last2) are sorted
    while (first1 != last1 && first2 != last2) {
        if (cmp(*first1, *first2)) {
            *d_first = *first1;
            ++first1;
        } else {
            *d_first = *first2;
            ++first2;
        }
        ++d_first;
    }

    for (; first1 != last1; ++first1, ++d_first) {
        *d_first = *first1;
    }
    for (; first2 != last2; ++first2, ++d_first) {
        *d_first = *last2;
    }
}
}  // namespace detail

template<typename I, typename Compare> void mergesort(I first, I last, Compare cmp) {
    if (first == last || std::next(first) == last) {  // 0 or 1 elements are already sorted
        return;
    }

    auto midpoint = first + (std::distance(first, last) / 2);
    mergesort(first, midpoint, cmp);
    mergesort(midpoint, last, cmp);

    // create temporary buffer
    using T = typename std::iterator_traits<I>::value_type;
    std::vector<T> storage(first, midpoint);
    // merge inplace
    merge(storage.begin(), storage.end(), midpoint, last, first, cmp);
}

template<typename I> void mergesort(I first, I last) {
    using T = typename std::iterator_traits<I>::value_type;
    mergesort(first, last, std::less<T>{});
}

namespace std {
template<typename T> ostream& operator<<(ostream& os, const std::vector<T>& v) {
    os << "{ ";
    for (const auto& e : v) {
        os << e << " ";
    }
    os << "}";
    return os;
}
}  // namespace std

struct CustomType {
    bool flag = false;
    std::string s;
    int id = -1;

    CustomType(bool f, const std::string& s) : flag(f), s(s) {
        static int i = 0;
        id = i++;
    }

    friend bool operator<(const CustomType& x, const CustomType& y) noexcept { return x.s < y.s; }
    friend bool operator==(const CustomType& x, const CustomType& y) noexcept {
        return x.flag == y.flag && x.s == y.s;
    }
    friend std::ostream& operator<<(std::ostream& os, const CustomType& x) {
        os << "(" << (x.flag ? "t" : "f") << ", " << x.s << ", " << x.id << ")";
        return os;
    }
};

int main() {
    TEST_REPEAT_BEGIN("mergesort_small_sizes", 1) {
        std::vector<int> x;
        mergesort(x.begin(), x.end());

        x.push_back(1);
        mergesort(x.begin(), x.end());

        x.push_back(-1);
        mergesort(x.begin(), x.end());
        return x == std::vector<int>({-1, 1});
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("mergesort_sorted", 1) {
        std::vector<int> x(20);
        std::iota(x.begin(), x.end(), 0);
        std::vector<int> y = x;
        mergesort(x.begin(), x.end());
        return x == y;
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("mergesort_sorted_descending", 1) {
        std::vector<int> x(20);
        std::iota(x.begin(), x.end(), 0);
        std::vector<int> y = x;
        std::reverse(x.begin(), x.end());
        mergesort(x.begin(), x.end());
        return x == y;
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("mergesort_random_input", 50) {
        std::vector<int> x(1000);
        static std::mt19937 g(std::random_device{}());
        iota_utils::random_iota(x.begin(), x.end(), 0, g);
        std::vector<int> y = x;

        mergesort(x.begin(), x.end());
        std::sort(y.begin(), y.end());

        return x == y;
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("mergesort_random_input2", 50) {
        std::vector<int> x(1871);
        static std::mt19937 g(std::random_device{}());
        iota_utils::random_iota(x.begin(), x.end(), 0, g);
        std::vector<int> y = x;

        mergesort(x.begin(), x.end());
        std::sort(y.begin(), y.end());

        return x == y;
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("mergesort_stability", 1) {
        std::vector<CustomType> x = {
            {true, "EAC"},  {false, "EAC"}, {false, "EAC"}, {true, "CDA"},
            {true, "AE"},   {false, "AA"},  {false, "BB"},  {true, "BAC"},
            {false, "BAC"}, {true, "BAC"},  {false, "BF"},  {true, "BR"},
        };
        std::sort(x.begin(), x.end());

        std::vector<CustomType> y = x;

        struct CompareByFlag {
            bool operator()(const CustomType& x, const CustomType& y) const noexcept {
                return x.flag < y.flag;
            }
        };
        mergesort(x.begin(), x.end(), CompareByFlag{});
        std::stable_sort(y.begin(), y.end(), CompareByFlag{});

        return x == y;
    }
    TEST_REPEAT_END()
    return 0;
}
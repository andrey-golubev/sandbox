#include "test.hpp"

#include "random_utils.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

namespace detail {
template<typename I> I partition_point_hoare(I first, I last) {
    // do -1 to compensate for half-open range [first, last)
    auto pivot = *(first + (std::distance(first, last) - 1) / 2);
    I i = std::prev(first);
    I j = last;
    while (true) {
        do {
            ++i;
        } while (*i < pivot);
        do {
            --j;
        } while (pivot < *j);
        if (!(i < j)) {
            return std::next(j);  // include j into the half-open range
        }
        using std::swap;
        swap(*i, *j);
    }
    return last;
}
}  // namespace detail

template<typename I> void quicksort(I first, I last) {
    if (first == last)
        return;
    if (first < std::prev(last)) {
        I p = detail::partition_point_hoare(first, last);
        quicksort(first, p);
        quicksort(p, last);
    }
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

int main() {
    TEST_REPEAT_BEGIN("quicksort_small_sizes", 1) {
        std::vector<int> x;
        quicksort(x.begin(), x.end());

        x.push_back(1);
        quicksort(x.begin(), x.end());

        x.push_back(-1);
        quicksort(x.begin(), x.end());
        return x == std::vector<int>({-1, 1});
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("quicksort_sorted", 1) {
        std::vector<int> x(20);
        std::iota(x.begin(), x.end(), 0);
        std::vector<int> y = x;
        quicksort(x.begin(), x.end());
        return x == y;
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("quicksort_sorted_descending", 1) {
        std::vector<int> x(20);
        std::iota(x.begin(), x.end(), 0);
        std::vector<int> y = x;
        std::reverse(x.begin(), x.end());
        quicksort(x.begin(), x.end());
        return x == y;
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("quicksort_random_input", 50) {
        std::vector<int> x(1000);
        static std::mt19937 g(std::random_device{}());
        iota_utils::random_iota(x.begin(), x.end(), 0, g);
        std::vector<int> y = x;

        quicksort(x.begin(), x.end());
        std::sort(y.begin(), y.end());

        return x == y;
    }
    TEST_REPEAT_END()

    TEST_REPEAT_BEGIN("quicksort_random_input2", 50) {
        std::vector<int> x(1871);
        static std::mt19937 g(std::random_device{}());
        iota_utils::random_iota(x.begin(), x.end(), 0, g);
        std::vector<int> y = x;

        quicksort(x.begin(), x.end());
        std::sort(y.begin(), y.end());

        return x == y;
    }
    TEST_REPEAT_END()
    return 0;
}
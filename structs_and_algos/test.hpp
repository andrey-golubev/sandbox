#pragma once

#include <iostream>

namespace detail {
template<typename... BoolExpressions> bool logical_and(BoolExpressions... exprs) {
    return (... && exprs);
}

template<typename Workload> bool repeat(size_t times, Workload w) {
    for (size_t i = 0; i < times; ++i) {
        bool result = w();
        if (!result) {
            std::cout << "Workload #" << i << " failed\n";
            return false;
        }
    }
    return true;
}
}  // namespace detail

#define TEST_RET_ON_ERROR(name, ...)                                                               \
    std::cout << "[" << name << "]: RUN" << std::endl;                                             \
    if (!detail::logical_and(__VA_ARGS__)) {                                                       \
        std::cerr << "[" << name << "]: FAIL" << std::endl;                                        \
        return EXIT_FAILURE;                                                                       \
    } else {                                                                                       \
        std::cout << "[" << name << "]: PASS" << std::endl;                                        \
    }

#define TEST_REPEAT_BEGIN(name, times)                                                             \
    {                                                                                              \
        std::string test_name = (name);                                                            \
    bool ret = detail::repeat((times), [&] ()

#define TEST_REPEAT_END() );                                                                       \
    TEST_RET_ON_ERROR(test_name, ret);                                                             \
    }

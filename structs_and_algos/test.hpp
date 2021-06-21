#pragma once

#include <iostream>

namespace detail {
template<typename... BoolExpressions> bool logical_and(BoolExpressions... exprs) {
    return (... && exprs);
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

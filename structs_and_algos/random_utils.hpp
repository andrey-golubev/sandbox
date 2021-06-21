#pragma once

// some random stuff from the internet and not only

#include <algorithm>
#include <string>

namespace string_utils {
static inline void ltrim(std::string& s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}
}  // namespace string_utils

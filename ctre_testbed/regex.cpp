#include "ctre.hpp"

#include <iostream>
#include <vector>
#include <utility>
#include <string_view>
#include <sstream>

namespace {
const std::vector<std::pair<std::string, int>> TEST_STRINGS = {
    {"2005-06-28T07:57:30.001UTC+0", 0},
    {"2005-06-28T07:57:30.001UTC-0", 0},
    {"2001-09-13T07:33:01.001 UTC+1", 3600},
    {"2008-09-13T07:33:01.001 UTC-11:01", -39660},
    {"2001-09-15T09:33:01.001UTC+02:57", 10620},
    {"2001-09-15T09:33:01.001 UTC+o8:30", -1},
    {"2001-09-15T09:33:01.001 UTC+08:3i", -1},
    {"2001-09-15T09:33:01.001 UTC+123", -1},
    {"2001-09-15T09:33:01.001 UTC+00005", 0},
    {"2008-10-13 UTC+12::11.50", 43200},
    {"2008-10-13 UTC+:59 11.50", -1},
    {"2008-10-13 UTC+ 11.50", -1},
    {"2008-10-13 UTC+11.50", 42600},
    {"2008-10-13 UTC+05:1 11.50", 18000},
    {"UTC+10:00 2008-10-13T07:33", 36000},
    {"2008-10-13 UTC-03:30 11.50", -12600},
    {"2008-10-13 UTC-2Z11.50", -7200},
    {"2008-10-13 UTC-0100:11.50", -3600},
    {"2008-10-13 UTC+05T:11.50", 18000},
    {"2008-10-13 UTC+010011.50", 3600},
    {"2008-10-13 UTC+0111.50", 4260},
    {"2008-10-13 UTC+01:0011.50", 3600},
    {"2008-10-13 UTC+01:011.50", 3660},
};
}

constexpr char32_t pattern_string[] = U".*UTC([\\+\\-])(\\d\\d?)(:?(\\d\\d))?.*";
constexpr size_t size = sizeof(pattern_string) / sizeof(pattern_string[0]);
static constexpr auto pattern = ctll::fixed_string<size>{ pattern_string };

int to_int(std::string_view s) {
    std::string valid(s.cbegin(), s.cend());
    if (s[0] == ':') {
        valid = std::string(s.cbegin() + 1, s.cend());
    }
    return std::stoi(valid);
}

int getUtcOffset(std::string_view input) {
    auto result = ctre::match<pattern>(input);
    if (!result) {
        // std::cout << "No match" << std::endl;
        return -1;
    }
    // std::cout << "Match: " << result.get<0>().to_view();

    const auto sign = result.get<1>().to_view();
    const auto hours_str = result.get<2>().to_view();
    const auto minutes_str = result.get<3>().to_view();
    // std::cout << " | " << sign << " | " << hours_str << " | " << minutes_str << std::endl;

    const int hours = to_int(hours_str);
    const int minutes = minutes_str.empty() ? 0 : to_int(minutes_str);
    int offset = 3600 * hours + 60 * minutes;
    if (sign == "-") {
        offset = -offset;
    }
    return offset;
}

int main()
{
    for (const auto& s : TEST_STRINGS) {
        const auto& input = s.first;

        const int expected = s.second;
        const int offset = getUtcOffset(input);

        {
            std::stringstream ss;
            if (expected == offset) {
                ss << "OK.";
            } else {
                ss << "FAIL. Expected " << expected << " but got " << offset;
            }

            std::cout << "REGEX_TEST[" << input << "]: " << ss.str() << std::endl;
        }
    }
    return 0;
}

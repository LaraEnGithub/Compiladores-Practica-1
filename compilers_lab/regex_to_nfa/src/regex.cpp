#include "regex.hpp"

#include <iostream>

Regex parse_regex(const std::string &pattern)
{
    (void)pattern;
    std::cout << "parse_regex\n";

    return Regex{};
}

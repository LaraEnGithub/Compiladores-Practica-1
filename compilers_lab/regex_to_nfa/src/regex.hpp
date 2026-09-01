#ifndef REGEX_HPP
#define REGEX_HPP

#include <string>
#include <vector>

struct Token
{
    char value;
};

struct Regex
{
    std::vector<Token> items;
};

Regex parse_regex(const std::string &pattern);

#endif

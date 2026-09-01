#ifndef NFA_HPP
#define NFA_HPP

#include <string>

#include "regex.hpp"

struct Nfa
{
    int placeholder;
};

Nfa regex_to_nfa(const Regex &r);
bool match_nfa(const Nfa &n, const std::string &input);
bool save_nfa(const Nfa &n, const std::string &path);
void free_nfa(Nfa &n);

#endif

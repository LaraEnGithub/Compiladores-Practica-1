#include "nfa.hpp"

#include <iostream>

Nfa regex_to_nfa(const Regex &r)
{
    (void)r;
    std::cout << "regex_to_nfa\n";

    return Nfa{};
}

bool match_nfa(const Nfa &n, const std::string &input)
{
    (void)n;
    (void)input;
    std::cout << "match_nfa\n";
    return false;
}

bool save_nfa(const Nfa &n, const std::string &path)
{
    (void)n;
    (void)path;
    std::cout << "save_nfa\n";
    return true;
}

void free_nfa(Nfa &n)
{
    (void)n;
    std::cout << "free_nfa\n";
}

int Nfa::add_state()
{
    return num_states++; 
}

namespace
{
struct Fragment
{
    int start;
    int accept;
};

Fragment build_literal(Nfa &n, char c)
{
    int s = n.add_state();
    int a = n.add_state();
    n.transitions.push_back({s, a, c});
    return {s, a};
}
} 
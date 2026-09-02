#include "nfa.hpp"
#include <stack>
#include <iostream>



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

Fragment build_concat(Nfa &n, Fragment a, Fragment b)
{
    n.transitions.push_back({a.accept, b.start, EPSILON});
    return {a.start, b.accept};
}

}


Nfa regex_to_nfa(const Regex &r)
{
    Nfa n;
    std::stack<Fragment> stack;

    for (const auto &token : r.items)
    {
        char c = token.value;
        if (c == '.')
        {
            Fragment b = stack.top(); stack.pop();
            Fragment a = stack.top(); stack.pop();
            stack.push(build_concat(n, a, b));
        }
        else
        {
            stack.push(build_literal(n, c));
        }
    }

    Fragment result = stack.top();
    n.start_state = result.start;
    n.accept_state = result.accept;
    return n;
}

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

void Nfa::add_transition(int from, int to, char c){
    states[from].transitions.push_back({to,c});
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
    n.add_transition(s, a, c);
    return {s, a};
}

Fragment build_concat(Nfa &n, Fragment a, Fragment b)
{
    n.add_transition(a.accept, b.start, EPSILON);
    return {a.start, b.accept};
}

Fragment build_union(Nfa &n, Fragment a, Fragment b)
{
    int s = n.add_state();
    int acc = n.add_state();
    n.add_transition(s, a.start, EPSILON);
    n.add_transition(s, b.start, EPSILON);
    n.add_transition(a.accept, acc, EPSILON);
    n.add_transition(b.accept, acc, EPSILON);
    return {s, acc};
}

Fragment build_stark(Nfa &n, Fragment a)
{
    int s = n.add_state();
    int acc = n.add_state();
    n.add_transition(s, a.start, EPSILON);
    n.add_transition(s, acc, EPSILON);
    n.add_transition(a.accept, a.start, EPSILON);
    n.add_transition(a.accept, acc, EPSILON);
    return {s, acc};
}

Fragment build_plus(Nfa &n, Fragment a)
{
    int acc = n.add_state();
    n.add_transition(a.accept, a.start, EPSILON);
    n.add_transition(a.accept, acc, EPSILON);
    return {a.start, acc};
}

Fragment build_optional(Nfa &n, Fragment a)
{
    int s = n.add_state();
    int acc = n.add_state();
    n.add_transition(s, a.start, EPSILON);
    n.add_transition(s, acc, EPSILON);
    n.add_transition(a.accept, acc, EPSILON);
    return {s, acc};
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
        else if (c == '|'){
            Fragment b = stack.top(); stack.pop();
            Fragment a = stack.top(); stack.pop();
            stack.push(build_union(n, a, b));
        }
        else if (c == '*')
        {
            Fragment a = stack.top(); stack.pop();
            stack.push(build_stark(n, a));
        }
        else if (c == '+')
        {
            Fragment a = stack.top(); stack.pop();
            stack.push(build_plus(n, a));
        }
        else if (c == '?')
        {
            Fragment a = stack.top(); stack.pop();
            stack.push(build_optional(n, a));
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
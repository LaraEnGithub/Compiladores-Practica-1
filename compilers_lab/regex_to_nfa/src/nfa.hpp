#ifndef NFA_HPP
#define NFA_HPP

#include <string>
#include <vector>
#include <set>
#include "regex.hpp"

constexpr char EPSILON = '\0';

struct Transition
{
    int to;
    char symbol; // might be epsilon
};

struct State
{
    std::vector<Transition> transitions;
};

struct Nfa
{
    int num_states = 0;
    std::vector<State> states;
    int start_state = -1;
    int accept_state = -1;

    int add_state();
    void add_transition(int from, int to, char c);
};

Nfa regex_to_nfa(const Regex &r);
bool match_nfa(const Nfa &n, const std::string &input);
bool save_nfa(const Nfa &n, const std::string &path);
bool load_nfa(const std::string &path, Nfa &out);
void free_nfa(Nfa &n);
void epsilon_closure(const Nfa &n, std::set<int> &states);

#endif

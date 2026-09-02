#ifndef NFA_HPP
#define NFA_HPP

#include <string>

#include <string>
#include <vector>
#include "regex.hpp"

constexpr char EPSILON = '\0';

struct Transition
{
    int from;
    int to;
    char symbol; //might be epsilon
};


struct Nfa
{
    int num_states = 0; 
    std::vector<Transition> transitions; 
    int start_sate = -1; 
    int accept_state = -1; 

    int add_state(); 
};



Nfa regex_to_nfa(const Regex &r);
bool match_nfa(const Nfa &n, const std::string &input);
bool save_nfa(const Nfa &n, const std::string &path);
void free_nfa(Nfa &n);

#endif

#include "nfa.hpp"
#include <stack>
#include <iostream>
#include <fstream>
#include <sstream>

bool match_nfa(const Nfa &n, const std::string &input)
{
    std::set<int> visited_states;

    visited_states.insert(n.start_state);
    epsilon_closure(n, visited_states);

    for (char c : input)
    {
        std::set<int> new_states;

        for (int s : visited_states)
        {
            for (const Transition &t : n.states[s].transitions)
            {
                if (t.symbol == c)
                    new_states.insert(t.to);
            }
        }

        if (new_states.empty())
            return false;

        visited_states = new_states;
        epsilon_closure(n, visited_states);
    }

    return visited_states.find(n.accept_state) != visited_states.end();
}

// para punto extra de interfaz y JSON
bool save_nfa(const Nfa &n, const std::string &path)
{
    std::ofstream out(path);
    if (!out.is_open())
        return false;

    out << "{\n";
    out << "  \"num_states\": " << n.num_states << ",\n";
    out << "  \"start_state\": " << n.start_state << ",\n";
    out << "  \"accept_state\": " << n.accept_state << ",\n";
    out << "  \"transitions\": [\n";

    bool first = true;
    for (int s = 0; s < n.num_states; ++s)
    {
        for (const Transition &t : n.states[s].transitions)
        {
            if (!first)
                out << ",\n";
            first = false;

            // EPSILON ('\0') lo marcamos como cadena vacía en el JSON
            out << "    {\"from\": " << s
                << ", \"to\": " << t.to
                << ", \"symbol\": ";
            if (t.symbol == EPSILON)
                out << "\"\"";
            else
                out << "\"" << t.symbol << "\"";
            out << "}";
        }
    }

    out << "\n  ]\n";
    out << "}\n";

    return out.good();
}

void free_nfa(Nfa &n)
{
    (void)n;
    n.states.clear();
    n.num_states = 0;
    n.start_state = -1;
    n.accept_state = -1;
}

void epsilon_closure(const Nfa &n, std::set<int> &states)
{
    std::stack<int> s;

    for (int state : states)
        s.push(state);

    while (!s.empty())
    {
        int curr_state = s.top();
        s.pop();

        for (const Transition &t : n.states[curr_state].transitions)
        {
            if (t.symbol == EPSILON && states.find(t.to) == states.end())
            {
                states.insert(t.to);
                s.push(t.to);
            }
        }
    }
}

int Nfa::add_state()
{
    states.push_back(State{});
    return num_states++;
}

void Nfa::add_transition(int from, int to, char c)
{
    states[from].transitions.push_back({to, c});
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
            Fragment b = stack.top();
            stack.pop();
            Fragment a = stack.top();
            stack.pop();
            stack.push(build_concat(n, a, b));
        }
        else if (c == '|')
        {
            Fragment b = stack.top();
            stack.pop();
            Fragment a = stack.top();
            stack.pop();
            stack.push(build_union(n, a, b));
        }
        else if (c == '*')
        {
            Fragment a = stack.top();
            stack.pop();
            stack.push(build_stark(n, a));
        }
        else if (c == '+')
        {
            Fragment a = stack.top();
            stack.pop();
            stack.push(build_plus(n, a));
        }
        else if (c == '?')
        {
            Fragment a = stack.top();
            stack.pop();
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

namespace
{
    // Extrae el primer entero que aparece tras la posición 'from' en 'text'.
    long read_int_after(const std::string &text, std::size_t from, std::size_t &pos)
    {
        std::size_t i = from;
        while (i < text.size() && (text[i] < '0' || text[i] > '9') && text[i] != '-')
            ++i;
        std::size_t start = i;
        if (i < text.size() && text[i] == '-')
            ++i;
        while (i < text.size() && text[i] >= '0' && text[i] <= '9')
            ++i;
        pos = i;
        return std::stol(text.substr(start, i - start));
    }
}

bool load_nfa(const std::string &path, Nfa &out)
{
    std::ifstream in(path);
    if (!in.is_open())
        return false;

    std::stringstream ss;
    ss << in.rdbuf();
    std::string text = ss.str();

    std::size_t pos = 0;

    std::size_t k = text.find("\"num_states\"");
    if (k == std::string::npos)
        return false;
    long num_states = read_int_after(text, k, pos);

    k = text.find("\"start_state\"");
    if (k == std::string::npos)
        return false;
    long start_state = read_int_after(text, k, pos);

    k = text.find("\"accept_state\"");
    if (k == std::string::npos)
        return false;
    long accept_state = read_int_after(text, k, pos);

    // Reconstruimos estados vacíos
    out = Nfa{};
    for (long i = 0; i < num_states; ++i)
        out.add_state();
    out.start_state = static_cast<int>(start_state);
    out.accept_state = static_cast<int>(accept_state);

    // Recorrer las transiciones
    std::size_t tpos = text.find("\"transitions\"");
    if (tpos == std::string::npos)
        return false;

    std::size_t cur = tpos;
    while (true)
    {
        std::size_t f = text.find("\"from\"", cur);
        if (f == std::string::npos)
            break;

        std::size_t p;
        long from = read_int_after(text, f, p);

        std::size_t t = text.find("\"to\"", p);
        if (t == std::string::npos)
            break;
        long to = read_int_after(text, t, p);

        // symbol
        std::size_t s = text.find("\"symbol\"", p);
        if (s == std::string::npos)
            break;
        std::size_t q1 = text.find('"', s + 8);  // abre comillas del valor
        std::size_t q2 = text.find('"', q1 + 1); // cierra
        std::string sym = text.substr(q1 + 1, q2 - q1 - 1);

        char c = sym.empty() ? EPSILON : sym[0];
        out.add_transition(static_cast<int>(from), static_cast<int>(to), c);

        cur = q2 + 1;
    }

    return true;
}

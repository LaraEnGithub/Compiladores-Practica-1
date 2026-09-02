#include "regex.hpp"

#include <queue>
#include <stack>

namespace
{

bool can_end_expression(char c)
{
    return c == ')' || c == '*' || c == '+' || c == '?' ||
           (c != '(' && c != '|' && c != '.');
}

bool can_start_expression(char c)
{
    return c == '(' ||
           (c != ')' && c != '|' && c != '*' && c != '+' && c != '?' && c != '.');
}

std::string add_explicit_concatenation(const std::string &pattern)
{
    static const char CONCAT_OP = '.';

    std::string result;
    for (std::size_t i = 0; i < pattern.size(); ++i)
    {
        char current = pattern[i];
        result.push_back(current);

        if (i + 1 < pattern.size())
        {
            char next = pattern[i + 1];
            if (can_end_expression(current) && can_start_expression(next))
            {
                result.push_back(CONCAT_OP);
            }
        }
    }
    return result;
}

int precedence(char op)
{
    switch (op)
    {
        case '|': return 1;
        case '.': return 2;
        default:  return 0;
    }
}

bool is_postfix_operator(char c)
{
    return c == '*' || c == '+' || c == '?';
}

bool is_binary_operator(char c)
{
    return c == '|' || c == '.';
}

std::vector<Token> shunting_yard(const std::string &pattern)
{
    std::stack<char> operators;
    std::queue<Token> output;

    for (char c : pattern)
    {
        if (c == '(')
        {
            operators.push(c);
        }
        else if (c == ')')
        {
            while (!operators.empty() && operators.top() != '(')
            {
                output.push(Token{operators.top()});
                operators.pop();
            }
            operators.pop();
        }
        else if (is_postfix_operator(c))
        {
            output.push(Token{c});
        }
        else if (is_binary_operator(c))
        {
            while (!operators.empty() && operators.top() != '(' &&
                   precedence(operators.top()) >= precedence(c))
            {
                output.push(Token{operators.top()});
                operators.pop();
            }
            operators.push(c);
        }
        else
        {
            output.push(Token{c});
        }
    }

    while (!operators.empty())
    {
        output.push(Token{operators.top()});
        operators.pop();
    }

    std::vector<Token> result;
    result.reserve(output.size());
    while (!output.empty())
    {
        result.push_back(output.front());
        output.pop();
    }
    return result;
}

} // namespace

Regex parse_regex(const std::string &pattern)
{
    std::string explicit_pattern = add_explicit_concatenation(pattern);
    return Regex{shunting_yard(explicit_pattern)};
}

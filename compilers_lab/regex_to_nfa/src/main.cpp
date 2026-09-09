#include "regex.hpp"
#include "nfa.hpp"

#include <getopt.h>
#include <iostream>
#include <string>

void strip_crlf(std::string &s)
{
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n'))
    {
        s.pop_back();
    }
}

void print_postfix(const Regex &r)
{
    for (const auto &t : r.items)
    {
        std::cout << t.value;
    }
    std::cout << "\n";
}

void test_strings_stdin(const std::string &regex_str)
{
    Regex r = parse_regex(regex_str);
    Nfa n = regex_to_nfa(r);

    std::string line;
    while (std::getline(std::cin, line))
    {
        strip_crlf(line);
        bool result = match_nfa(n, line);
        std::cout << (result ? '1' : '0');
    }
    std::cout << "\n";

    free_nfa(n);
}

int serialize_nfa_from_regex(const std::string &regex_str, const std::string &output_path)
{
    Regex r = parse_regex(regex_str);
    Nfa n = regex_to_nfa(r);

    bool ok = save_nfa(n, output_path);
    free_nfa(n);

    if (!ok)
    {
        std::cerr << "Error: No se pudo serializar el NFA en '" << output_path << "'.\n";
        return 1;
    }

    return 0;
}

// Modo -l (carga): reconstruye un NFA previamente serializado con -o
// leyéndolo desde disco, y valida contra él las cadenas que llegan por
// stdin (una por línea), imprimiendo 1/0 por cada una.
int load_and_test(const std::string &input_path)
{
    Nfa n;
    if (!load_nfa(input_path, n))
    {
        std::cerr << "Error: No se pudo cargar el NFA desde '" << input_path << "'.\n";
        return 1;
    }

    std::string line;
    while (std::getline(std::cin, line))
    {
        strip_crlf(line);
        bool result = match_nfa(n, line);
        std::cout << (result ? '1' : '0');
    }
    std::cout << "\n";

    free_nfa(n);
    return 0;
}

int main(int argc, char *argv[])
{
    int opt;
    std::string output_file;
    std::string input_file;
    int mode = 0;

    while ((opt = getopt(argc, argv, "rto:l:")) != -1)
    {
        switch (opt)
        {
        case 'r':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t o -o.\n";
                return 1;
            }
            mode = 'r';
            break;
        case 't':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t o -o.\n";
                return 1;
            }
            mode = 't';
            break;
        case 'o':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t o -o.\n";
                return 1;
            }
            mode = 'o';
            output_file = optarg;
            break;
        case 'l':
            if (mode != 0)
            {
                std::cerr << "Error: Solo puedes usar una opcion de modo entre -r, -t, -o o -l.\n";
                return 1;
            }
            mode = 'l';
            input_file = optarg;
            break;

        default:
            std::cerr << "Usage: " << argv[0] << " -r | -t | -o <archivo.nfa>\n";
            return 1;
        }
    }

    if (mode == 0)
    {
        std::cerr << "Usage: " << argv[0] << " -r | -t | -o <archivo.nfa>\n";
        return 1;
    }

    // El modo -l carga el NFA desde disco
    if (mode == 'l')
    {
        return load_and_test(input_file);
    }

    std::string regex_str;
    if (!std::getline(std::cin, regex_str))
    {
        return 1;
    }
    strip_crlf(regex_str);

    if (mode == 'r')
    {
        print_postfix(parse_regex(regex_str));
        return 0;
    }

    if (mode == 't')
    {
        test_strings_stdin(regex_str);
        return 0;
    }

    return serialize_nfa_from_regex(regex_str, output_file);
}

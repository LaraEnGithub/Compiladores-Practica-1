## Estructura del proyecto

```
compilers_lab/regex_to_nfa/
├── src/
│   ├── main.cpp
│   ├── regex.hpp/.cpp
│   └── nfa.hpp/.cpp
├── CMakeLists.txt
├── Dockerfile
└── validator
```

## Uso

```
-r              lee una regex de stdin, imprime su forma postfija con concatenación explícita
-t              lee una regex y luego cadenas (una por línea) de stdin, imprime 1/0 por cadena
-o <archivo>    lee una regex de stdin, serializa el NFA resultante en <archivo>
```

Ejemplo:

```bash
echo "(ab)*" | ./regex_to_nfa -r
# ab.*

printf '%s\n' "(ab)*" "ab" "aba" "abab" | ./regex_to_nfa -t
# 1101
```

## Compilar y correr localmente

```bash
cd compilers_lab/regex_to_nfa
cmake -S . -B build
cmake --build build
./build/regex_to_nfa -r
```

## Validar con Docker

```bash
cd compilers_lab/regex_to_nfa
docker build -t regex_to_nfa_validator .
docker run --rm regex_to_nfa_validator
```



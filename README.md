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

...

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



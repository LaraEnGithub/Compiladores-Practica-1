cmake_minimum_required ( VERSION 3.16)
project ( regex_to_nfa C )
set( CMAKE_C_STANDARD 11)
set( CMAKE_C_STANDARD_REQUIRED ON )
add_executable ( regex_to_nfa
./ src / main . c
./ src / nfa . c
./ src / regex . c
)

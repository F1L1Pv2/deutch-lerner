#/bin/sh

set -xe

libs=""
includes="-Ithirdparty -Isrc"
warnings="-Wno-tautological-constant-out-of-range-compare -Wno-microsoft-goto"

clang++ -g --std=c++17 src/main.cpp -o main.exe $warnings $includes $libs
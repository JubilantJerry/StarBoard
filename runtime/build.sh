#!/bin/bash

build() {
    mkdir -p build &&
    cd build &&
    cmake .. &&
    make &&
    cd ..;
}

docs() {
    mkdir -p docs &&
    doxygen;
}

clean() {
    rm -r build
    rm -r docs
}

run_tests() {
    valgrind --leak-check=full --show-reachable=yes --show-leak-kinds=none\
             --suppressions=valgrind_suppressions.txt build/bin/tests;
}

while getopts ":bdcr" opt; do
    case $opt in
        b) build ;;
        d) docs ;;
        c) clean ;;
        r) run_tests ;;
        *) echo "Invalid option: -$OPTARG" >&2
    esac
done

#!/bin/bash

if [ -z "$BOOST_ROOT" ]; then
    export BOOST_ROOT=/usr/local/boost_1_67_0
fi

build() {
    mkdir -p build &&
    cd build &&
    cmake -DCMAKE_BUILD_TYPE=$1 .. &&
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
    valgrind --leak-check=full\
             --suppressions=valgrind_suppressions.txt build/bin/tests;
}

full() {
    clean;
    build RELEASE;
    run_tests;
    docs;
}

while getopts ":bdcrf" opt; do
    case $opt in
        b) build DEBUG ;;
        d) docs ;;
        c) clean ;;
        r) run_tests ;;
        f) full ;;
        *) echo "Invalid option: -$OPTARG" >&2
    esac
done

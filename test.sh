#!/bin/bash

function assert()
{
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -z noexecstack -o tmp tmp.s
    ./tmp
    actual="$?"

    if [[ "$actual" = "$expected" ]]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 41 " 12 + 34 - 5 "
assert 47 '5+6*7'
assert 15 '5*(9-6)'
assert 4 '(3+5)/2'
assert 10 '-10+20'
assert 1 '4+5*2 == 14'
assert 0 '9*(3+ 8) != 9*(11)'
assert 0 '3*3 < 2'
assert 1 '2 <= 3'
assert 1 '8-0 > -2'
assert 0 '2 >= 3'

echo OK
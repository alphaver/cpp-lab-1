#!/bin/bash

cat "$1" > "big_$1"
while [[ $(stat -c%s "big_$1") -lt 8*1024*1024*1024 ]]; do
    echo $(stat -c%s "big_$1")
    cp "big_$1" "/tmp/_big_$1"
    cat "/tmp/_big_$1" >> "big_$1"
    rm "/tmp/_big_$1"
done

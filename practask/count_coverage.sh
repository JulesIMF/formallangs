#!/bin/sh

llvm-profdata merge -sparse default.profraw -o default.profdata

if [ "$show" != "" ]; then
llvm-cov show $1 -instr-profile=default.profdata
fi

llvm-cov report $1 -instr-profile=default.profdata
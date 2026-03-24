#!/bin/bash
usage() {
  echo "./build.sh [-koopa|-riscv] program_name"
}
if [ $# != 2 ]; then
  usage
  return 1
fi
if [ "$1" = "-koopa" ]; then
  ./compiler -koopa $2.c -o $2.S
  koopac $2.S | llc --filetype=obj -o $2.o
  clang $2.o -L$CDE_LIBRARY_PATH/native -lsysy -o $2
  ./$2
  echo $?
elif [ "$1" = "-riscv" ]; then
  ./compiler -riscv $2.c -o $2.S
  clang $2.S -c -o $2.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32 -g
  ld.lld $2.o -L/opt/lib/riscv32 -lsysy -g -O0 -o $2
  qemu-riscv32-static $2
  echo $?
else
  usage
fi


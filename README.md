# build
## koopa
./compiler -koopa ../../compiler-dev-test-cases/testcases/lv1/0_main.c -o 0_main.S
koopac 0_main.S | llc --filetype=obj -o 0_main.o
clang 0_main.o -L$CDE_LIBRARY_PATH/native -lsysy -o 0_main
./0_main

## riscv
./compiler -riscv 6_complex_scopes.c -o 6_complex_scopes.S
clang 6_complex_scopes.S -c -o 6_complex_scopes.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32 -g
ld.lld 6_complex_scopes.o -L/opt/lib/riscv32 -lsysy -g -O0 -o 6_complex_scopes

# debug
## gdb-server
qemu-riscv32-static -g 1234 -singlestep ./your_compiled_program
## gdb-client
gdb-multiarch ./your_compiled_program
target remote localhost:1234
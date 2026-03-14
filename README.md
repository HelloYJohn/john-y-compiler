# build
./compiler -koopa ../../compiler-dev-test-cases/testcases/lv1/0_main.c -o 0_main.S
koopac 0_main.S | llc --filetype=obj -o 0_main.o
clang 0_main.o -L$CDE_LIBRARY_PATH/native -lsysy -o 0_main
./0_main


Team member:
Gage, Artak, Zhen

To compile:

1. First compile suzaku.o with suzaku.c suzaku.h
mpicc -c -o suzaku.o suzaku.c -lm

2. then compile your program with suzaku.o
mpicc yourprogram suzaku.o -lm

To run:
mpiexec -n p ./a.out


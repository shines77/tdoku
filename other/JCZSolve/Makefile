all:	jczsolve

jczsolve:	JCZSolveMain.o JCZSolve.o
	clang JCZSolveMain.o JCZSolve.o -o jczsolve

JCZSolveMain.o:	JCZSolveMain.c JCZSolve.h
	clang -O4 -g -c JCZSolveMain.c -o JCZSolveMain.o

JCZSolve.o:	JCZSolve.c JCZSolve.h
	clang -O4 -g -c JCZSolve.c -o JCZSolve.o

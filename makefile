run:start.o pre_assem.o label.o help.o first_pass.o word.o second_pass.o files.o
	gcc -g -Wall -ansi -pedantic  start.o pre_assem.o label.o help.o first_pass.o word.o second_pass.o files.o -o run
start.o: start.c start.h
	gcc -Wall -ansi -pedantic -c start.c -o start.o
pre_assem.o: pre_assem.c pre_assem.h 
	gcc -Wall -ansi -pedantic -c pre_assem.c -o pre_assem.o
label.o: label.c label.h
	gcc -Wall -ansi -pedantic -c label.c -o label.o
help.o: help.c help.h
	gcc -Wall -ansi -pedantic -c help.c -o help.o
first_pass.o: first_pass.c first_pass.h
	gcc -Wall -ansi -pedantic -c first_pass.c -o first_pass.o
word.o: word.c word.h
	gcc -Wall -ansi -pedantic -c word.c -o word.o
second_pass.o: second_pass.c second_pass.h
	gcc -Wall -ansi -pedantic -c second_pass.c -o second_pass.o
files.o: files.c files.h
	gcc -Wall -ansi -pedantic -c files.c -o files.o

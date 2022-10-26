.PHONY : all
	all : calculate
calculate : calculate.o helper.o helper2.o
	gcc -o calculate calculate.o helper.o helper2.o
calculate.o : helper.h calculate.c
	gcc -c calculate.c 
helper.o : helper.h helper2.h helper.c 
	gcc -c helper.c
helper2.o : helper2.h helper2.c
	gcc -c helper2.c  
clean : 
	rm -f *.o calculate
	

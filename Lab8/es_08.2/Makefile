CC = g++
CFLAGS = -Wall -O3 --std=c++11

my_program.exe  : main_08_2.o random.o
	$(CC) random.o main_08_2.o -o my_program.exe
main_08_2.o : main_08_2.cpp function.h main_08_2.h
	$(CC) -c main_08_2.cpp -o main_08_2.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
run :
	./my_program.exe 
clean :
	rm *.o 






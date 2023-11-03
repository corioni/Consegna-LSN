CC = g++
CFLAGS = -Wall -O3 --std=c++11

my_program.exe : main_05_1.o random.o
	$(CC) random.o main_05_1.o -o my_program.exe
main_05_1.o : main_05_1.cpp function.h main_05_1.h
	$(CC) -c main_05_1.cpp -o main_05_1.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
run : 
	./my_program.exe
clean :
	rm *.o 





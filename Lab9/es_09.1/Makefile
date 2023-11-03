CFLAGS = -Wall -O3 --std=c++11

my_program.exe : main_09_1.o GA.o random.o main_09_1.h GA.h random.h
	g++ random.o GA.o main_09_1.o  -o my_program.exe  $(CFLAGS) -larmadillo
main_09_1.o : main_09_1.cpp main_09_1.h
	g++ -c main_09_1.cpp -o main_09_1.o $(CFLAGS)
random.o : random.cpp random.h
	g++ -c random.cpp -o random.o
GA.o : GA.cpp GA.h random.h
	g++ -c GA.cpp -o GA.o $(CFLAGS)
run :
	./my_program.exe 
clean :
	rm *.o 





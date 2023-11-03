CFLAGS = -Wall -O3 --std=c++17

my_program.exe : main_10_2.o GA.o main_10_2.h GA.h 
	mpicxx  GA.o main_10_2.o  -o my_program.exe  $(CFLAGS) -larmadillo 
main_10_2.o : main_10_2.cpp main_10_2.h
	mpicxx -c main_10_2.cpp -o main_10_2.o $(CFLAGS)
GA.o : GA.cpp GA.h
	mpicxx -c GA.cpp -o GA.o $(CFLAGS)
run :
	mpirun -np 4 ./my_program.exe
clean :
	rm *.o 





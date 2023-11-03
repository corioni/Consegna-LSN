#include "main_09_1.h"
#include "GA.h"
#include "random.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <armadillo>
#include <cstdlib>
#include <cassert>

using namespace std;
using namespace arma;

int main()
{
    Random rnd;
    rnd.Init();
    arma_rng::set_seed(4);

    int N_pop = 250;
    int N_city = 34;
    int N_epoch = 500;

    mat pos_circle(N_city, 2);
    mat pos_square(N_city, 2);

    pos_circle = generate_pos("circle", 1., N_city, rnd);
    pos_square = generate_pos("square", 1., N_city, rnd);

    pos_circle.save("output/circle.dat", raw_ascii);
    pos_square.save("output/square.dat", raw_ascii);
    
    Population pop_c(N_pop, N_city, pos_circle);
    Population pop_s(N_pop, N_city, pos_square);

    pop_c.Check();
    pop_s.Check();
    

    pop_c.Print(0,"output/first_path_c.dat");
    pop_s.Print(0,"output/first_path_s.dat");


    pop_c.Evolution(N_epoch,"circle");
    pop_s.Evolution(N_epoch,"square");

    //(pop_c.Get_chromos()).brief_print();
    //(pop_s.Get_chromos()).brief_print();

    assert(N_pop == pop_c.Get_Dim());
    assert(N_pop == pop_s.Get_Dim());

    pop_c.Print(0,"output/best_path_c.dat");
    pop_s.Print(0,"output/best_path_s.dat");


    return 0;
}
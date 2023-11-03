
#ifndef __main_09_1_h__
#define __main_09_1_h__

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <armadillo>
#include "random.h"

using namespace arma;

mat generate_pos(std::string type, double len, int N_city, Random & rnd) {
    if (type == "circle") {
        mat A(N_city, 2);
        for (int i = 0; i < N_city; i++) {
            double theta = rnd.Rannyu(0, 2 * M_PI);
            A(i, 0) = cos(theta) * len / 2.;
            A(i, 1) = sin(theta) * len / 2.;
        }
        return A;
    } else if (type == "square") {
        mat A(N_city, 2);
        for (int i = 0; i < N_city; i++) {
            A(i, 0) = rnd.Rannyu(0., len);
            A(i, 1) = rnd.Rannyu(0., len);
        }
        return A;
    } else {
        cerr << "Function 'generate_pos' was called incorrectly" << endl;
        // You should add a return statement here or handle the error as needed.
        return mat(); // Return an empty matrix as a placeholder.
    }
}


#endif
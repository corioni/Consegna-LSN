#pragma once

#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>
#include "main_08_2.h"

using namespace std;

class Function
{
public:
    virtual double calculate(double x, double m, double sigma) const = 0;
};

class Wave_Function : public Function
{
public:
    Wave_Function(){};
    double calculate(double x , double mu , double sigma) const override
    {
        return exp(-pow((x-mu), 2)/ (2*sigma*sigma)) + exp(-pow((x+mu), 2)/ (2*sigma*sigma));  
    }
};

class Integrand : public Function
{
public:
    Integrand(){};
    double calculate( double x , double mu , double sigma  ) const override {

        double kinetic_term = -0.5 * ((-2*mu*x)*tanh(x*mu/(sigma*sigma)) + mu*mu + x*x - sigma*sigma)  / pow(sigma , 4.);
        double potential_term =  pow(x ,4) - 5./2. * pow( x , 2 )  ;

        return  ((kinetic_term )  + potential_term ); }
};

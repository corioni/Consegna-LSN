#pragma once

#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <string>
#include <vector>
#include <assert.h>
#include "main_05_1.h"

using namespace std;

class Function
{
public:
    virtual double calculate(vector<double> x) const = 0;
};

class Hydrogen_Atom_100 : public Function
{
private:
    double a0=1.;
public:
    Hydrogen_Atom_100(){};
    double calculate(vector<double> x) const override
    {
        double r= sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
        return pow(a0,-3.)/(M_PI)*pow(M_E,-2*r/a0);
    }
};

class Hydrogen_Atom_210 : public Function
{
private:
    double a0=1.;
public:
    Hydrogen_Atom_210(){};
    double calculate(vector<double> x) const override
    {
        double r= sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
        return pow(a0,-5.)/32.*(2./M_PI)*pow(x[2],2)*pow(M_E,-r/a0);
    }
};

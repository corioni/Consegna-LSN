#pragma once

#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <string>
#include <assert.h>

using namespace std;

class Function
{
public:
    virtual double calculate(double x) const = 0;
};

class Parable : public Function
{
private:
    double a; // Quadratic coefficient
    double b; // Linear coefficient
    double c; // Constant term

public:
    Parable(double a, double b, double c) : a(a), b(b), c(c) {}

    double calculate(double x) const override
    {
        return a * x * x + b * x + c;
    }
};

class Straight_Line : public Function
{
private:
    double m; // Slope
    double q; // Intercept

public:
    Straight_Line(double m, double q) : m(m), q(q) {}

    double calculate(double x) const override
    {
        return m * x + q;
    }
};

class MyFunction : public Function
{
public:
    double calculate(double x) const override
    {
        return M_PI / 2 * cos(M_PI * x / 2);
    }
};

class Exponential : public Function
{
private:
    double lambda;
public:
    Exponential(double lambda) : lambda(lambda) {}

    double calculate(double x) const override
    {
        return lambda * pow(M_E,-lambda*x);
    }
    double get_lambda() const{
         return lambda;}
};


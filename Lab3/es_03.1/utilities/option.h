#pragma once

#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <string>
#include <assert.h>
#include "../random/random.h" // Corrected include statement

using namespace std;

class EuropeanOption
{
protected:
    double S0;    // Current stock price
    double K;     // Strike price
    double r;     // Risk-free interest rate
    double T;     // Time to maturity
    double sigma; // Volatility

public:
    EuropeanOption(double _S, double _K, double _r, double _T, double _sigma)
        : S0(_S), K(_K), r(_r), T(_T), sigma(_sigma) {}

    virtual double payoff(double S0) const = 0;

    // Monte Carlo simulation to estimate option price
    double direct_simulation(Random &rnd) const
    {
        double z = rnd.Gauss(0., 1.);
        double St = S0 * exp((r - (pow(sigma, 2) / 2.)) * T + sigma * z * sqrt(T));
        return exp(-r * T) * payoff(St);
    }

    double discretized_simulation(Random &rnd) const
    {
        double St = S0;
        for (int i = 0; i < 100; i++)
        {
            double z = rnd.Gauss(0., 1.);
            St *= exp((r - pow(sigma, 2) / 2.) * 0.01 + sigma * z * sqrt(0.01));
        }
        return exp(-r * T)*payoff(St);
    }
};

class CallOption : public EuropeanOption
{
public:
    CallOption(double _S, double _K, double _r, double _T, double _sigma)
        : EuropeanOption(_S, _K, _r, _T, _sigma) {}

    double payoff(double S) const override
    {
        return std::max(S - K, 0.0);
    }
};

class PutOption : public EuropeanOption
{
public:
    PutOption(double _S, double _K, double _r, double _T, double _sigma)
        : EuropeanOption(_S, _K, _r, _T, _sigma) {}

    double payoff(double S) const override
    {
        return std::max(K - S, 0.0);
    }
};


#ifndef __main_10_2_h__
#define __main_10_2_h__

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <armadillo>
#include <fstream>

using namespace arma;
using namespace std;

void printProgressBar(double progress)
{
    const int barWidth = 70;
    int pos = barWidth * progress;
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

int file_rows(string filename)
{
    int numLines = 0;
    std::string unused;
    ifstream in(filename);
    if (in.is_open())
    {
        while (std::getline(in, unused))
            ++numLines;
        in.close();
    }
    else
    {
        cout << "Unable to read " + filename + ". Exit." << endl;
        exit(1);
    }

    return numLines;
}

mat load_pos(string filename)
{
    int numLines = file_rows(filename);
    mat cities(numLines, 2);
    ifstream in(filename);
    if (in.is_open())
    {
        for (int i = 0; i < numLines; i++)
        {
            in >> cities(i, 0) >> cities(i, 1);
        }
        in.close();
    }
    else
    {
        cout << "Unable to read " + filename + ". Exit." << endl;
        exit(1);
    }

    return cities;
}

void Check_Out_Directory(std::string dir){
    ofstream out(dir);
    if(!out){
        cerr<<"If the output directory does not exist, before running the code again run : mkdir -p "<<dir<<endl;
        //exit(1);
    }

}

#endif
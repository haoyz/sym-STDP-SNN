#include "file_read.h"
#include "../global.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
float str2float(string s)
{
    float num;
    stringstream ss(s);
    ss >> num;
    return num;
}
void read_gP2E_from_file(float *tmp_gP2E)
{
    string str_gP2E;
    ifstream gP2E_file("./weights/gPE");
    if (gP2E_file)
    {
        cout << "Read from ./weights/gPE..." << endl;
        for (int i = 0; i < _NPoi; i++)
        {
            for (int j = 0; j < _NExc; j++)
            {
                gP2E_file >> str_gP2E;
                tmp_gP2E[i * _NExc + j] = str2float(str_gP2E);
            }
        }
        gP2E_file.close();
    }
    else
    {
        cout << "open ./weights/gPE error!" << endl;
    }
}
void read_thetaPExc_from_file(float *tmp_thetaPExc)
{
    string str_theta;
    ifstream theta_file("./weights/theta");
    if (theta_file)
    {
        cout << "Read from ./weights/theta..." << endl;
        for (int i = 0; i < _NExc; i++)
        {
            theta_file >> str_theta;
            tmp_thetaPExc[i] = str2float(str_theta);
        }
        theta_file.close();
    }
    else
    {
        cout << "open ./weights/theta error!" << endl;
    }
}
void read_gE2C_from_file(float *tmp_gE2C)
{
    string str_gE2C;
    ifstream gE2C_file("./weights/gEC");
    if (gE2C_file)
    {
        cout << "Read from ./weights/gEC..." << endl;
        for (int i = 0; i < _NExc; i++)
        {
            for (int j = 0; j < _NCla; j++)
            {
                gE2C_file >> str_gE2C;
                tmp_gE2C[i * _NCla + j] = str2float(str_gE2C);
            }
        }
        gE2C_file.close();
    }
    else
    {
        cout << "open ./weights/gEC error!" << endl;
    }
}
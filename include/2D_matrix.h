#include<vector>
#include<iostream>


class stiffness_matrix {
    public:
    double DX;
    double DY;
    double NX;
    double NY;
    double A;
    double B;
    double C;
    double D;

    std::vector<std::vector<double>> Km;
    std::vector<std::vector<double>> Cm;
};
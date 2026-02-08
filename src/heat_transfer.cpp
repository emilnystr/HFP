#include "heat_transfer.h"
#include <cmath>

void heat_flow_void(double T1, double T2,
                    double epsilon, double sigma, double h_void,
                    double& Qi, double& Qj)
{
    double T1K = T1 + 273.15;
    double T2K = T2 + 273.15;
    double Tm  = 0.5 * (T1 + T2);

    //strålning
    double q_rad_i = epsilon * sigma * (std::pow(T2K,4) - std::pow(T1K,4));
    double q_rad_j = -q_rad_i;

    //konvektion
    double q_conv_i = h_void * (Tm - T1);
    double q_conv_j = h_void * (Tm - T2);

    Qi = q_rad_i + q_conv_i;
    Qj = q_rad_j + q_conv_j;
}

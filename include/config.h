#ifndef CONFIG
#define CONFIG

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class parameters {
    public:
    int model = 1;
    double initial_temperature = 20;
    double simulation_time = 3600;
    double mm_per_layer = 5;
    double timeStepsPerSecond = 5;
    double time_step = 0.2;
    double h_exposed = 25;
    double h_ambient = 10;
    double stefan_boltzmann = 5.67e-8;
    double emissivity = 0.8;
    double ambient_temperature = 20;
    int fire_curve_exposed = 2;
    double constant_surface_temp = 1000;
};


#endif
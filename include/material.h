#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include<filesystem>

class MaterialProperty {

    public:
    std::string name;
    std::vector<double> temperature;
    std::vector<double> conductivity;
    std::vector<double> specific_heat;
    std::vector<double> density;
};

class FastMaterial {
    public:
    std::vector<double> k;   // Värmeledningsförmåga
    std::vector<double> c;   // Specifik värmekapacitet
    std::vector<double> rho; // Densitet
    
    void precompute(const std::vector<double>& temperature_input,
                   const std::vector<double>& rho_input,
                   const std::vector<double>& cp_input,
                   const std::vector<double>& k_input);
    
    
    void get_props(double X, double& Y_k, double& Y_c, double& Y_rho) const {
        if (X < 0)   X = 0;
        if (X > 2000) X = 2000;

        int X1 = (int)X;     
        int X2 = X1 + 1;                  

        double t = (X - X1) / (X2 - X1);

        Y_k = k[X1]   + t * (k[X2]   - k[X1]);   
        Y_c = c[X1]   + t * (c[X2]   - c[X1]);   
        Y_rho = rho[X1] + t * (rho[X2] - rho[X1]); 
}


};

std::vector<MaterialProperty> load_materials_from_directory(const std::string& directory_path = "material");

#endif
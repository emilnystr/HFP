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

class MaterialTable {
    public:
    std::vector<double> k;  
    std::vector<double> c;   
    std::vector<double> rho; 
    std::vector<double> enthalpy;
    
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

    // Hämta entalpi för en given temperatur samt derivata
    void get_enthalpy(double T, double& e, double& dedT) const {
        if (T < 0) T = 0;
        if (T > 2000) T = 2000;
        
        int T1 = (int)T;
        int T2 = T1 + 1;
        
        if (T2 >= enthalpy.size()) {
            e = enthalpy.back();
            dedT = 0;
            return;
        }
        
        double t = T - T1;
        e = enthalpy[T1] + t * (enthalpy[T2] - enthalpy[T1]);
        dedT = (enthalpy[T2] - enthalpy[T1]); //step = 1 implicit divided by 1 therefore
    }

    // Hämta entalpi utan derivata
    double get_enthalpy_value(double T) const {
        if (T < 0) T = 0;
        if (T > 2000) T = 2000;
        
        int T1 = (int)T;
        int T2 = T1 + 1;
        
        if (T2 >= enthalpy.size()) {
            return enthalpy.back();
        }
        
        double t = T - T1;
        return enthalpy[T1] + t * (enthalpy[T2] - enthalpy[T1]);
}


};

std::vector<MaterialProperty> load_materials_from_directory(const std::string& directory_path = "material");

#endif
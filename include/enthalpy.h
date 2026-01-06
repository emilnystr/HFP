#ifndef ENTHALPY
#define ENTHALPY

#include <vector>
#include <iostream>
#include "material.h"
#include "mesh.h"



std::vector<double> getE(const Mesh& mesh,
    const std::vector<MaterialTable>& fast_materials,
    const std::vector<double>& T);



std::vector<double> getTfromE (const Mesh& mesh,
    const std::vector<MaterialTable>& fast_materials,
    const std::vector<double>& T_prev, std::vector<double> E);

    
#endif
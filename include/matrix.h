#pragma once
#include <vector>
#include "layer.h"
#include "material.h"
#include "mesh.h"

class global_matrices {
public: 
    std::vector<std::vector<double>> K;
    std::vector<double> C;
};

global_matrices compute_global_matrices(
    const Mesh& mesh,
    const std::vector<MaterialTable>& fast_materials,
    const std::vector<double>& T,
    const parameters& cfg);

std::vector<double> matvec(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& x);
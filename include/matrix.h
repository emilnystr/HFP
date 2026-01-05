#pragma once
#include <vector>
#include "layer.h"
#include "material.h"
#include "mesh.h"

// För full matris (bakåtkompatibilitet)
class global_matrices {
public: 
    std::vector<std::vector<double>> K;
    std::vector<double> C;
};


// Original functions (for compatibility)
global_matrices compute_global_matrices(
    const Mesh& mesh,
    const std::vector<MaterialTable>& fast_materials,
    const std::vector<double>& T);

std::vector<double> matvec(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& x);
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "config.h"
#include "material.h"
#include "mesh.h"
#include "fire.h"
#include "matrix.h"

void run_matrix_simulation (const Mesh& mesh, const std::vector<FastMaterial>& fast_materials, 
    const parameters& cfg);
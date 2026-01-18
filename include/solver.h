#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "config.h"
#include "material.h"
#include "mesh.h"
#include "fire.h"
#include "enthalpy.h"
#include "matrix.h"

// Original skalärmetod
void run_simulation(const parameters& cfg,
                    const std::vector<MaterialTable>& fast_materials,
                    const Mesh& mesh);

// Entalpibaserad metod
void run_enthalpy_simulation(const parameters& cfg,
                             const std::vector<MaterialTable>& fast_materials,
                             const Mesh& mesh);

#endif
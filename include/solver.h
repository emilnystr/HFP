#ifndef SOLVER_H
#define SOLVER_H

#include "config.h"
#include "material.h"
#include "mesh.h"
#include "fire.h"
#include "enthalpy.h"
#include "matrix.h"
#include <vector>

void simulation_callback(double time, 
                        const std::vector<double>& positions,
                        const std::vector<double>& temperatures);

void run_simulation(
    const parameters& cfg,
    const std::vector<MaterialTable>& fast_materials,
    const Mesh& mesh,
    void (*callback)(double time, 
                     const std::vector<double>& positions,
                     const std::vector<double>& temperatures),
    int save_interval = 1 
);

void run_enthalpy_simulation(const parameters& cfg,
                             const std::vector<MaterialTable>& fast_materials,
                             const Mesh& mesh,
                             void (*callback)(double time, 
                                              const std::vector<double>& positions,
                                              const std::vector<double>& temperatures),
                             int save_interval = 1);

#endif
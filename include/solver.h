#ifndef SOLVER_H
#define SOLVER_H

#include "config.h"
#include "material.h"
#include "mesh.h"
#include "fire.h"
#include <functional>
#include <vector>

void run_simulation(const parameters& cfg,
                    const std::vector<MaterialTable>& fast_materials,
                    const Mesh& mesh);

// Ny funktion för att samla data för animation
void run_simulation_with_animation(
    const parameters& cfg,
    const std::vector<MaterialTable>& fast_materials,
    const Mesh& mesh,
    std::function<void(double time, 
                       const std::vector<double>& positions,
                       const std::vector<double>& temperatures)> callback,
    int save_interval = 1  // Sparar varje save_interval:e tidssteg
);

#endif
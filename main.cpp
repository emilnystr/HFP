#include <iostream>
#include <iomanip>
#include <chrono>
#include "config.h"
#include "material.h"
#include "layer.h"
#include "mesh.h"
#include "matrix_solver.h" 
#include "solver.h"


int main() {
    std::cout << "Startar HFP-engine" << std::endl;
    
    auto start_total = std::chrono::high_resolution_clock::now();
    
    parameters cfg = load_config("Config.txt");
    
    std::cout << "  Modell: " << cfg.model << std::endl;
    std::cout << "  Simuleringstid: " << cfg.simulation_time << " s" << std::endl;
    std::cout << "  Tidssteg: " << cfg.time_step << " s ";
    std::cout << "  Brandkurva: ";
    if (cfg.fire_curve_exposed == 1)
        std::cout << "ISO 834";
    else if (cfg.fire_curve_exposed == 2)
        std::cout << "HC 180";
    else if (cfg.fire_curve_exposed == 3)
        std::cout << "Parametrisk";
    else if (cfg.fire_curve_exposed == 4)
        std::cout << "Konstant (" << cfg.constant_surface_temp << " °C)";

    std::cout << std::endl;
    
    auto materials = load_materials_from_directory();
    
    std::vector<FastMaterial> fast_materials(materials.size());
    for (size_t i = 0; i < materials.size(); ++i) {
        fast_materials[i].precompute(
            materials[i].temperature,
            materials[i].density,    
            materials[i].specific_heat,  
            materials[i].conductivity    
        );
    }
    
    auto layers = load_layers_from_file(materials);
    
    Mesh mesh = create_mesh(layers, cfg);
    
    run_simulation(cfg, fast_materials, mesh);
    
    auto end_total = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_time = end_total - start_total;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nExekveringstid: " << total_time.count() << " s" << std::endl;
    
    return 0;
}
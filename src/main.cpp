#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include "config.h"
#include "material.h"
#include "layer.h"
#include "mesh.h"
#include "solver.h"
#include "plotter.h"
#include "enthalpy.h"



std::vector<std::vector<double>> all_temperatures;
std::vector<double> all_times;
std::vector<double> positions;

void simulation_callback(double time, 
                           const std::vector<double>& pos, 
                           const std::vector<double>& temps) {
    if (positions.empty()) {
        positions = pos;
    }
    
    all_temperatures.push_back(temps);
    all_times.push_back(time);
}

int main() {
    std::cout << "Startar HFP-engine" << std::endl;
    
    auto start_total = std::chrono::high_resolution_clock::now();
    
    parameters cfg = load_config("Config.txt");
    
    std::cout << "  Model: " << cfg.model << std::endl;
    std::cout << "  simulation time: " << cfg.simulation_time << " s" << std::endl;
    std::cout << "  time step: " << cfg.time_step << " s ";
    std::cout << "  fire curve: ";
    if (cfg.fire_curve_exposed == 1)
        std::cout << "ISO 834";
    else if (cfg.fire_curve_exposed == 2)
        std::cout << "HC 180";
    else if (cfg.fire_curve_exposed == 3)
        std::cout << "Parametrisk";
    else if (cfg.fire_curve_exposed == 4)
        std::cout << "constant temperature (" << cfg.constant_surface_temp << " °C)";

    std::cout << std::endl;
    
    auto materials = load_materials_from_directory();
    
    std::vector<MaterialTable> fast_materials(materials.size());
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

    int save_interval = 1;
    
    all_temperatures.clear();
    all_times.clear();
    positions.clear();
    if (cfg.model == 1) {
        std::cout << "\nScalar method\n";
        run_simulation(cfg, fast_materials, mesh, simulation_callback, save_interval);
    } 
    else if (cfg.model == 2) {
    std::cout << "\nEnthalpy method\n";
    
    all_temperatures.clear();
    all_times.clear();
    positions.clear();
    
    
    run_enthalpy_simulation(cfg, fast_materials, mesh, simulation_callback, save_interval);
}
        
    int total_steps = static_cast<int>(cfg.simulation_time / cfg.time_step);
    
    auto start_sim = std::chrono::high_resolution_clock::now();
    
    auto end_sim = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> sim_time = end_sim - start_sim;

    auto end_total = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_time = end_total - start_total;
    
    std::cout << "\nCalculation time: " << total_time.count() << " s" << std::endl;

    if (!all_temperatures.empty()) {
        
        TemperatureAnimator animator;
        animator.setData(positions, all_temperatures, all_times);
        animator.animate("HFP-engine");
        
        
        const auto& final_temps = all_temperatures.back();
        double final_time = all_times.back();
        
        std::cout << "\nSluttemperaturer vid t = " << final_time << " s (C):\n";
        
        for (size_t i = 0; i < positions.size(); i++) {
            std::cout << positions[i] << " mm: " << final_temps[i] << " C\n";
        }
        
        std::ofstream csv("temperaturoprofil.csv");
        csv << "Position_mm;Temperature_C\n";
        for (size_t i = 0; i < positions.size(); i++) {
            csv << positions[i] << ";" << final_temps[i] << "\n";
        }
                
        double max_temp = *std::max_element(final_temps.begin(), final_temps.end());
        double min_temp = *std::min_element(final_temps.begin(), final_temps.end());
        
    }

    
    std::cout << std::fixed << std::setprecision(2);
    
    return 0;
}
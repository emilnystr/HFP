#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <algorithm>  // LÄGG TILL DENNA!
#include <fstream>    // LÄGG TILL DENNA!
#include <string>     // LÄGG TILL DENNA!
#include "config.h"
#include "material.h"
#include "layer.h"
#include "mesh.h"
#include "solver.h"
#include "plotter.h"

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
    
    // Vektorer för att samla animeringsdata
    std::vector<std::vector<double>> all_temperatures;
    std::vector<double> all_times;
    std::vector<double> positions;
    
    std::cout << "\nKör simulering och samlar data för animation..." << std::endl;
    
    // Beräkna hur många steg vi ska spara (max 1000 frames för att inte bli för många)
    int total_steps = static_cast<int>(cfg.simulation_time / cfg.time_step);
    int save_interval = std::max(1, total_steps / 1000);
    
    auto start_sim = std::chrono::high_resolution_clock::now();
    
    // Kör simuleringen med callback för att samla data
    run_simulation_with_animation(
        cfg,
        fast_materials,
        mesh,
        [&](double time, const std::vector<double>& pos, const std::vector<double>& temps) {
            // Första gången: spara positions
            if (positions.empty()) {
                positions = pos;
            }
            
            // Spara temperaturer och tid
            all_temperatures.push_back(temps);
            all_times.push_back(time);
        },
        save_interval
    );
    
    auto end_sim = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> sim_time = end_sim - start_sim;
    
    std::cout << "\nSimulering klar på " << sim_time.count() << " sekunder." << std::endl;
    std::cout << "Sparade " << all_temperatures.size() << " frames för animation." << std::endl;
    

    if (!all_temperatures.empty()) {
        std::cout << "\nStartar animation av temperaturutveckling..." << std::endl;
        
        TemperatureAnimator animator;
        animator.setData(positions, all_temperatures, all_times);
        animator.animate("HFP-engine - Temperature Development Over Time");
        
        // Efter animationen, visa slutresultatet
        std::cout << "\n\n" << std::string(50, '=') << "\n";
        std::cout << "SIMULERING AVSLUTAD - Slutresultat\n";
        std::cout << std::string(50, '=') << "\n";
        
        // Använd den sparade datan direkt (eller hämta från animatorn)
        const auto& final_temps = all_temperatures.back();
        double final_time = all_times.back();
        
        std::cout << "\nSluttemperaturer vid t = " 
                  << std::fixed << std::setprecision(1) << final_time 
                  << " s (C):\n";
        
        // Visa temperaturer i terminalen
        for (size_t i = 0; i < positions.size(); i++) {
            std::cout << positions[i] << " mm: " << final_temps[i] << " C\n";
        }
        
        // Spara till CSV-fil
        std::ofstream csv("temperature_profile_final.csv");
        csv << "Position_mm;Temperature_C\n";
        for (size_t i = 0; i < positions.size(); i++) {
            csv << positions[i] << ";" << final_temps[i] << "\n";
        }
        csv.close();
        
        std::cout << "\nData sparad till 'temperature_profile_final.csv'\n";
        
        // Beräkna statistik
        double max_temp = *std::max_element(final_temps.begin(), final_temps.end());
        double min_temp = *std::min_element(final_temps.begin(), final_temps.end());
        
        std::cout << "\nStatistik:\n";
        std::cout << "Max temperatur: " << std::fixed << std::setprecision(1) 
                  << max_temp << " C\n";
        std::cout << "Min temperatur: " << min_temp << " C\n";
    }
    
    auto end_total = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_time = end_total - start_total;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nTotal exekveringstid: " << total_time.count() << " s" << std::endl;
    
    return 0;
}  
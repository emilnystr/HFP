#include "solver.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

void run_enthalpy_simulation(const parameters& cfg,
                             const std::vector<MaterialTable>& fast_materials,
                             const Mesh& mesh) {
    
    std::cout << "Kör entalpibaserad simulering (model=2)\n";
    
    // Initial temperatur
    std::vector<double> T(mesh.num_nodes, cfg.initial_temperature);
    
    // Beräkna initial entalpi
    std::vector<double> E = getE(mesh, fast_materials, T);
    
    const int n_nodes = mesh.num_nodes;
    const int n_elems = mesh.num_elements;
    const double dt = cfg.time_step;
    const int time_steps = static_cast<int>(cfg.simulation_time / dt);
    
    
    for (int step = 0; step < time_steps; ++step) {
        double time = step * dt;
        double T_fire = fire_temperature(time, cfg);
        
        // Beräkna globala matriser baserat på aktuell temperatur
        global_matrices matrices = compute_global_matrices(mesh, fast_materials, T);
        std::vector<std::vector<double>>& K = matrices.K;
        std::vector<double>& C = matrices.C;
        
        // Beräkna K*T
        std::vector<double> KT = matvec(K, T);
        
        // Initiera Q-vektor
        std::vector<double> Q(n_nodes, 0.0);
        
        // Värmeflöde på exponerad sida
        double T_front = T[0];
        double q_exposed = cfg.h_exposed * (T_fire - T_front) +
                          cfg.stefan_boltzmann * cfg.emissivity *
                          (std::pow(T_fire + 273.15, 4) - std::pow(T_front + 273.15, 4));
        Q[0] = q_exposed;
        
        // Värmeflöde på oexponerad sida
        int last_node = n_nodes - 1;
        double T_back = T[last_node];
        double q_unexposed = cfg.h_ambient * (cfg.ambient_temperature - T_back) +
                            cfg.stefan_boltzmann * cfg.emissivity *
                            (std::pow(cfg.ambient_temperature + 273.15, 4) - std::pow(T_back + 273.15, 4));
        Q[last_node] = q_unexposed;
        
        // Uppdatera entalpin: E += dt * (Q - K@T)
        for (int i = 0; i < n_nodes; ++i) {
            E[i] += dt * (Q[i] - KT[i]);
        }
        
        // Beräkna nya temperaturer från entalpin
        // Använd föregående temperatur som startgissning
        T = getTfromE(mesh, fast_materials, T, E);
    
    }

    
    // Skriv slutlig temperaturprofil
    std::cout << "\nSluttemperaturer (entalpimodell):\n";
    std::ofstream profile_csv("temperature_profile_enthalpy.csv");
    profile_csv << "Position_mm;Temperature_C\n";
    
    double position = 0.0;
    for (int i = 0; i < n_nodes; ++i) {
        std::cout << position * 1000.0 << " mm: " << T[i] << " °C\n";
        profile_csv << position * 1000.0 << ";" << T[i] << "\n";
        
        if (i < n_elems) {
            position += mesh.element_sizes[i];
        }
    }
    
}
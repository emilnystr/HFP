#include "solver.h"
#include "heat_transfer.h" 
#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

void run_enthalpy_simulation(const parameters& cfg,
                             const std::vector<MaterialTable>& fast_materials,
                             const Mesh& mesh,
                             void (*callback)(double time, 
                                              const std::vector<double>& positions,
                                              const std::vector<double>& temperatures),
                             int save_interval) 
{
    std::cout << "entalpibaserad simulering (model=2)\n";
    
    std::vector<double> T(mesh.num_nodes, cfg.initial_temperature);
    
    std::vector<double> E = getE(mesh, fast_materials, T);
    
    const int n_nodes = mesh.num_nodes;
    const int n_elems = mesh.num_elements;
    const double dt = cfg.time_step;
    const int time_steps = static_cast<int>(cfg.simulation_time / dt);
    
    std::vector<double> positions_mm(n_nodes);
    double length = 0;
    for (int i = 0; i < n_nodes; ++i) {
        positions_mm[i] = length * 1000; 
        if (i < n_elems) {
            length += mesh.element_sizes[i];
        }
    }
    
    callback(0, positions_mm, T);

    for (int step = 0; step < time_steps; ++step) {
        double time = step * dt;
        double T_fire = fire_temperature(time, cfg);
        
        global_matrices matrices = compute_global_matrices(mesh, fast_materials, T, cfg);
        std::vector<std::vector<double>>& K = matrices.K;
        std::vector<double>& C = matrices.C;
        
        std::vector<double> KT = matvec(K, T);
        
        std::vector<double> Q(n_nodes, 0);
        
        // Yttre randvillkor
        double T_front = T[0];
        double q_exposed = cfg.h_exposed * (T_fire - T_front) +
                          cfg.stefan_boltzmann * cfg.emissivity *
                          (std::pow(T_fire + 273.15, 4) - std::pow(T_front + 273.15, 4));
        Q[0] = q_exposed;
        
        int last_node = n_nodes - 1;
        double T_back = T[last_node];
        double q_unexposed = cfg.h_ambient * (cfg.ambient_temperature - T_back) +
                            cfg.stefan_boltzmann * cfg.emissivity *
                            (std::pow(cfg.ambient_temperature + 273.15, 4) - std::pow(T_back + 273.15, 4));
        Q[last_node] = q_unexposed;
        
        for (int e = 0; e < n_elems; ++e) {
            int i = e;
            int j = e + 1;
            
            double T_avg = 0.5 * (T[i] + T[j]);
            double k, c, rho;
            fast_materials[mesh.material_indices[e]].get_props(T_avg, k, c, rho);
            
            if (k == 0.0) {
                double Qi, Qj;
                heat_flow_void(T[i], T[j],
                            cfg.emissivity,
                            cfg.stefan_boltzmann,
                            cfg.h_void,
                            Qi, Qj);

                Q[i] += Qi;
                Q[j] += Qj;
            }
        }

        
        for (int i = 0; i < n_nodes; ++i) {
            E[i] += dt * (Q[i] - KT[i]);
        }
        
        T = getTfromE(mesh, fast_materials, T, E);

        if (save_interval > 0 && step % save_interval == 0) {
            callback(time, positions_mm, T);
        }
    }

    double final_time = time_steps * dt;
    callback(final_time, positions_mm, T);
    
    std::ofstream profile_csv("temperature_profile_entalpi.csv");
    profile_csv << "Position_mm;Temperatur_C\n";
    
    double position = 0;
    for (int i = 0; i < n_nodes; ++i) {
        profile_csv << position * 1000 << ";" << T[i] << "\n";
        
        if (i < n_elems) {
            position += mesh.element_sizes[i];
        }
    }
}
#include "solver.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "heat_transfer.h"


void run_simulation(
    const parameters& cfg,
    const std::vector<MaterialTable>& fast_materials,
    const Mesh& mesh,
    void (*callback)(double time, 
                     const std::vector<double>& positions,
                     const std::vector<double>& temperatures), int save_interval) 
                     {
        
    std::vector<double> T(mesh.num_nodes, cfg.initial_temperature);
    std::vector<double> T_next(mesh.num_nodes, 0);

    std::vector<double> Q_eff(mesh.num_nodes, 0);
    std::vector<double> C(mesh.num_nodes, 0);

    const double* dx_e = mesh.element_sizes.data();
    const int* mat_e   = mesh.material_indices.data();

    const int n_elem  = mesh.num_elements;
    const int n_node  = mesh.num_nodes;
    const double dt   = cfg.time_step;
    const int time_steps = (int)(cfg.simulation_time / dt);

    std::vector<double> positions_mm(n_node);
    double length = 0.0;
    for (int i = 0; i < n_node; ++i) {
        positions_mm[i] = length * 1000.0;
        if (i < n_elem) {
            length += dx_e[i];
        }
    }

    callback(0, positions_mm, T);
    
    

    int last_saved_step = 0;

    for (int step = 0; step < time_steps; ++step) {
        double current_time = (step + 1) * dt;
        double T_fire = fire_temperature(current_time, cfg);
        
        std::fill(Q_eff.begin(), Q_eff.end(), 0.0);
        std::fill(C.begin(), C.end(), 0.0);


        for (int e = 0; e < n_elem; ++e) {
            int i = e;
            int j = e + 1;
            double T_average = 0.5 * (T[i] + T[j]);

            double k, c, rho;
            fast_materials[mat_e[e]].get_props(T_average, k, c, rho);
            double dx = dx_e[e];

            C[i] += rho * c * dx * 0.5;
            C[j] += rho * c * dx * 0.5;

            if (k == 0.0) {
                double Qi, Qj;
                heat_flow_void(T[i], T[j],
                            cfg.emissivity,
                            cfg.stefan_boltzmann,
                            cfg.h_void,
                            Qi, Qj);

                Q_eff[i] += Qi;
                Q_eff[j] += Qj;
            } else {
                double q = (k / dx) * (T[i] - T[j]);
                Q_eff[i] -= q;
                Q_eff[j] += q;
            }

        }

        double T_front = T[0];
        double q_exposed = cfg.stefan_boltzmann * cfg.emissivity * 
                          (std::pow(T_fire + 273.15, 4) - std::pow(T_front + 273.15, 4)) + 
                          cfg.h_exposed * (T_fire - T_front);
        Q_eff[0] += q_exposed;

        int n = n_node - 1;
        double T_back = T[n];
        double q_unexposed = cfg.stefan_boltzmann * cfg.emissivity * 
                            (std::pow(cfg.ambient_temperature + 273.15, 4) - std::pow(T_back + 273.15, 4)) + 
                            cfg.h_ambient * (cfg.ambient_temperature - T_back);
        Q_eff[n] += q_unexposed;

        for (int i = 0; i < n_node; ++i) {
            T_next[i] = T[i] + dt * (Q_eff[i] / C[i]);
        }

        callback(current_time, positions_mm, T_next);
        last_saved_step = step;
        
        std::swap(T, T_next);
    }

    double final_time = time_steps * dt;
    callback(final_time, positions_mm, T);
    
}
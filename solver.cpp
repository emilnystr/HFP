#include "solver.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

/*Denna metod bygger på skalärformen som presenteras i Ulfs bok i kapitel 7,
istället för att utföra matrisberäkningar bygger vi beräkningen genom värmeflödesvektorn och utför 
elementvisa beräkningar baserat på lokala temperaturgradienter
 */
void run_simulation(const parameters& cfg,
                    const std::vector<MaterialTable>& fast_materials,
                    const Mesh& mesh)
{
    std::vector<double> T(mesh.num_nodes, cfg.initial_temperature);
    std::vector<double> T_next(mesh.num_nodes, 0);

    //Q - K*T som en vektor
    std::vector<double> Q_eff(mesh.num_nodes, 0);

    //värmekapacitet
    std::vector<double> C(mesh.num_nodes, 0);

    //pointers till arrayerna
    const double* dx_e = mesh.element_sizes.data();
    const int* mat_e   = mesh.material_indices.data();

    //pointers meningslösa här
    const int n_elem  = mesh.num_elements;
    const int n_node  = mesh.num_nodes;
    const double dt   = cfg.time_step;
    const int time_steps = (int)(cfg.simulation_time / dt);

    for (int step = 0; step < time_steps; ++step) {
        double time = step * dt;
        double T_fire = fire_temperature(time, cfg);
        
        //nollställ värmeflödet efter varje tidssteg
        std::fill(Q_eff.begin(), Q_eff.end(), 0);
        std::fill(C.begin(), C.end(), 0);

        for (int e = 0; e < n_elem; ++e) {
            int i = e;       
            int j = e + 1;   

            double T_average = 0.5 * (T[i] + T[j]);

            double k;
            double c;
            double rho;
            fast_materials[mat_e[e]].get_props(T_average, k, c, rho);

            double dx = dx_e[e];

            C[i] += rho * c * dx * 0.5;
            C[j] += rho * c * dx * 0.5;

            double q = (k / dx) * (T[i] - T[j]);
            
            //negativt flöde in, positivt flöde ut ur elementet
            Q_eff[i] -= q;
            Q_eff[j] += q;
        }

        //värmeflöde på exponerad nod
            double T_front = T[0];
            double q_exposed = cfg.stefan_boltzmann * cfg.emissivity * (std::pow(T_fire + 273.15, 4) - std::pow(T_front + 273.15, 4)) + cfg.h_exposed * (T_fire - T_front);

            Q_eff[0] += q_exposed;

        // Oexponerad sida
        
            int n = n_node - 1;
            double T_back = T[n];

            double q_unexposed = cfg.stefan_boltzmann * cfg.emissivity * (std::pow(cfg.ambient_temperature + 273.15, 4) - std::pow(T_back + 273.15, 4)) + cfg.h_ambient * (cfg.ambient_temperature - T_back);

            Q_eff[n] += q_unexposed;
        

        for (int i = 0; i < n_node; ++i) {

            T_next[i] = T[i] + dt * (Q_eff[i] / C[i]);

        }


        std::swap(T, T_next);
    }

    std::cout << "Sluttemperaturer (C):\n";

    double length = 0;
    for (int i = 0; i < n_node; ++i) {
        std::cout << length * 1000 << "mm: " << T[i] << "\n";

        if (i < mesh.num_elements){
            length += mesh.element_sizes[i];
        }
    }

    std::ofstream csv("temperature_profile.csv");
    csv << "Position_mm;Temperature_C\n";
    
    {double length = 0.0;
    for (int i = 0; i < n_node; ++i) {
        csv << (length * 1000.0) << ";" << T[i] << "\n";

        if (i < mesh.num_elements) {
            length += mesh.element_sizes[i];
        }
    }}
}

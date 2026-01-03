#include "matrix_solver.h"


void run_matrix_simulation (const Mesh& mesh, const std::vector<FastMaterial>& fast_materials, 
    const parameters& cfg) {
    
    
    std::vector<double> Temp(mesh.num_nodes, 20);
    std::vector<double> Q(mesh.num_nodes, 0);

    global_matrices K_and_C = compute_global_matrices(mesh, fast_materials, Temp);

    
    const int n_elems = mesh.num_elements;
    const int n_nodes = mesh.num_nodes;
    const double dt = cfg.time_step;
    const double simulation_time = cfg.simulation_time;
    const int number_of_timesteps = (int)(simulation_time / dt);
    const int fire_curve = cfg.fire_curve_exposed;
    double T_fire;
    double h_exposed = cfg.h_exposed;
    double h_ambient = cfg.h_ambient;
    double ambient_temp = cfg.ambient_temperature;
    double sigma = cfg.stefan_boltzmann;
    double epsilon = cfg.emissivity;
    std::vector<std::vector<double>> K = K_and_C.K;
    std::vector<double> C = K_and_C.C;


    int model = cfg.model;


    for (int i = 0; i < number_of_timesteps; ++i) {

        double current_time = i * dt;

        T_fire = fire_temperature(current_time, cfg);

        // Uppdatera globala matriser baserat på aktuell temperatur
        global_matrices K_and_C = compute_global_matrices(mesh, fast_materials, Temp);
        std::vector<std::vector<double>> K = K_and_C.K;
        std::vector<double> C = K_and_C.C;

        // Randvillkor
        Q[0] = h_exposed * (T_fire - Temp[0]) +
            sigma * epsilon * (pow(T_fire + 273.15, 4) - pow(Temp[0] + 273.15, 4));

        Q.back() = h_ambient * (ambient_temp - Temp.back()) +
                sigma * epsilon * (pow(ambient_temp + 273.15, 4) - pow(Temp.back() + 273.15, 4));

        if (model == 1) {

            std::vector<double> KT = matvec(K, Temp);

            std::vector<double> Q_KT(n_nodes);
            for (int j = 0; j < n_nodes; ++j)
                Q_KT[j] = Q[j] - KT[j];

            std::vector<double> Cinv_QKT(n_nodes);
            for (int j = 0; j < n_nodes; ++j)
                Cinv_QKT[j] = (1.0 / C[j]) * Q_KT[j];   // explicit metod

            std::vector<double> Temp_next(n_nodes);
            for (int j = 0; j < n_nodes; ++j)
                Temp_next[j] = Temp[j] + dt * Cinv_QKT[j];

            Temp = Temp_next;   // ← här uppdateras temperaturen

            // ============================
            // Lägg till utskrift här
            // ============================
            if (i == simulation_time / dt - 1) {
                std::cout << "\nTemperaturer vid t = " << current_time << " s:\n";
                for (int j = 0; j < n_nodes; ++j) {
                    std::cout << "  Nod " << j << ": " << Temp[j] << " °C\n";
                }
            }

        }
    }
}
#include "matrix.h"
#include "mesh.h"
#include "heat_transfer.h" 

global_matrices compute_global_matrices(
    const Mesh& mesh,
    const std::vector<MaterialTable>& fast_materials,
    const std::vector<double>& T,
    const parameters& cfg)  
{
    int n = mesh.num_nodes;

    global_matrices K_and_C;
    K_and_C.K.assign(n, std::vector<double>(n, 0.0));
    K_and_C.C.assign(n, 0);

    auto& K = K_and_C.K;
    auto& C = K_and_C.C;

    int e = 0;
    double dx = mesh.element_sizes[e];
    double T_avg = 0.5 * (T[0] + T[1]);

    int mat = mesh.material_indices[e];
    double k, c, rho;
    fast_materials[mat].get_props(T_avg, k, c, rho);

    C[0] += rho * c * dx * 0.5;

    if (k > 0.0) {
        double ke = k / dx;
        K[0][0] =  ke;
        K[0][1] = -ke;
    }

    // Mellannoder
    for (int i = 1; i < n - 1; ++i)
    {
        // Element till vänster om nod i
        int e_left = i - 1;
        double dx_left = mesh.element_sizes[e_left];
        double T_avg_left = 0.5 * (T[i-1] + T[i]);

        int mat_left = mesh.material_indices[e_left];
        double k_left, c_left, rho_left;
        fast_materials[mat_left].get_props(T_avg_left, k_left, c_left, rho_left);

        C[i] += rho_left * c_left * dx_left * 0.5;

        if (k_left > 0.0) {
            double ke_left = k_left / dx_left;
            K[i][i-1] += -ke_left;
            K[i][i]   +=  ke_left;
        }

        int e_right = i;
        double dx_right = mesh.element_sizes[e_right];
        double T_avg_right = 0.5 * (T[i] + T[i+1]);

        int mat_right = mesh.material_indices[e_right];
        double k_right, c_right, rho_right;
        fast_materials[mat_right].get_props(T_avg_right, k_right, c_right, rho_right);

      
        C[i] += rho_right * c_right * dx_right * 0.5;

        if (k_right > 0.0) {
            double ke_right = k_right / dx_right;
            K[i][i]   +=  ke_right;
            K[i][i+1] += -ke_right;
        }
    }

    // Sista noden
    {
        int i = n - 1;
        int e = n - 2;

        double dx = mesh.element_sizes[e];
        double T_avg = 0.5 * (T[i-1] + T[i]);

        int mat = mesh.material_indices[e];
        double k, c, rho;
        fast_materials[mat].get_props(T_avg, k, c, rho);

        C[i] += rho * c * dx * 0.5;

        if (k > 0.0) {
            double ke = k / dx;
            K[i][i-1] = -ke;
            K[i][i]   =  ke;
        }
    }

    return K_and_C;
}

std::vector<double> matvec(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& x)
{
    std::vector<double> y(A.size(), 0);

    for (int i = 0; i < A.size(); ++i)
        for (int j = 0; j < A.size(); ++j)
            y[i] += A[i][j] * x[j];

    return y;
}

std::vector<double> invert_C(const std::vector<double>& C)
{
    std::vector<double> Cinv(C.size());

    for (size_t i = 0; i < C.size(); ++i) {
        if (C[i] != 0.0) {
            Cinv[i] = 1 / C[i];
        }
    }

    return Cinv;
}
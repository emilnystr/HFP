#include "matrix.h"
#include "mesh.h"

global_matrices compute_global_matrices(
    const Mesh& mesh,
    const std::vector<FastMaterial>& fast_materials,
    const std::vector<double>& T)
{
    int n = mesh.num_nodes;

    global_matrices K_and_C;
    K_and_C.K.assign(n, std::vector<double>(n, 0.0));
    K_and_C.C.assign(n, 0.0);

    auto& K = K_and_C.K;
    auto& C = K_and_C.C;

 
    
        int e = 0;
        double dx = mesh.element_sizes[e];
        double T_avg = 0.5 * (T[0] + T[1]);

        int mat = mesh.material_indices[e];
        double k, c, rho;
        fast_materials[mat].get_props(T_avg, k, c, rho);

        double ke = k / dx;

        K[0][0] =  ke;
        K[0][1] = -ke;

        C[0] += rho * c * dx * 0.5;


    for (int i = 1; i < n - 1; ++i)
    {
        // Vänster element
            int e = i - 1;
            double dx = mesh.element_sizes[e];
            double T_avg = 0.5 * (T[i-1] + T[i]);

            int mat = mesh.material_indices[e];
            double k, c, rho;
            fast_materials[mat].get_props(T_avg, k, c, rho);

            double ke = k / dx;

            K[i][i-1] += -ke;
            K[i][i]   +=  ke;

            C[i] += rho * c * dx * 0.5;

        // Höger element
        {
            int e = i;
            double dx = mesh.element_sizes[e];
            double T_avg = 0.5 * (T[i] + T[i+1]);

            int mat = mesh.material_indices[e];
            double k, c, rho;
            fast_materials[mat].get_props(T_avg, k, c, rho);

            double ke = k / dx;

            K[i][i]   +=  ke;
            K[i][i+1] += -ke;

            C[i] += rho * c * dx * 0.5;
        }
    }

    {
        int i = n - 1;
        int e = n - 2;

        double dx = mesh.element_sizes[e];
        double T_avg = 0.5 * (T[i-1] + T[i]);

        int mat = mesh.material_indices[e];
        double k, c, rho;
        fast_materials[mat].get_props(T_avg, k, c, rho);

        double ke = k / dx;

        K[i][i-1] = -ke;
        K[i][i]   =  ke;

        C[i] += rho * c * dx * 0.5;
    }

    return K_and_C;
}



std::vector<double> matvec(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& x)
{
    std::vector<double> y(A.size(), 0.0);

    for (int i = 0; i < A.size(); ++i)
        for (int j = 0; j < A.size(); ++j)
            y[i] += A[i][j] * x[j];

    return y;
}

std::vector<double> invert_C(const std::vector<double>& C)
{
    std::vector<double> Cinv(C.size());

    for (size_t i = 0; i < C.size(); ++i)
        Cinv[i] = 1.0 / C[i];

    return Cinv;
}
#include "enthalpy.h"
#include <algorithm>
#include <cmath>

/*Vi använder materialfilerna för att beräkna entalpikurvorna för varje material användaren ber om
*/
std::vector<double> getE(const Mesh& mesh, const std::vector<MaterialTable>& fast_materials,
    const std::vector<double>& T) {
    
    std::vector<double> E(mesh.num_nodes, 0.0);
    
    // Första noden - enbart bidrag från hälften av första elementet
    if (mesh.num_elements > 0) {
        double e0;
        double dedT; // Ej använd här, men behövs för funktionsanrop
        fast_materials[mesh.material_indices[0]].get_enthalpy(T[0], e0, dedT);
        E[0] = e0 * mesh.element_sizes[0] * 0.5;
    }
    
    // Inre noder - bidrag från två element
    for (int i = 1; i < mesh.num_nodes - 1; ++i) {
        double ei;
        double dedT;
        
        // Bidrag från vänster element
        fast_materials[mesh.material_indices[i-1]].get_enthalpy(T[i], ei, dedT);
        double left_contrib = ei * mesh.element_sizes[i-1] * 0.5;
        
        // Bidrag från höger element
        fast_materials[mesh.material_indices[i]].get_enthalpy(T[i], ei, dedT);
        double right_contrib = ei * mesh.element_sizes[i] * 0.5;
        
        E[i] = left_contrib + right_contrib;
    }
    
    // Sista noden - enbart bidrag från hälften av sista elementet
    if (mesh.num_elements > 0) {
        double en;
        double dedT;
        int last_elem = mesh.num_elements - 1;
        fast_materials[mesh.material_indices[last_elem]].get_enthalpy(T.back(), en, dedT);
        E.back() = en * mesh.element_sizes[last_elem] * 0.5;
    }
    
    return E;
}



std::vector<double> getTfromE(const Mesh& mesh, const std::vector<MaterialTable>& fast_materials,
    const std::vector<double>& T_prev, std::vector<double>& E) {
    
    int n = E.size();
    std::vector<double> T_out(n, 0);
    std::vector<double> V(n, 0);
    
    if (mesh.num_elements > 0) {
        V[0] = 0.5 * mesh.element_sizes[0];
        V[n-1] = 0.5 * mesh.element_sizes[mesh.num_elements-1];
    }
    
    for (int i = 1; i < n-1; ++i) {
        V[i] = 0.5 * mesh.element_sizes[i-1] + 0.5 * mesh.element_sizes[i];
    }
    
    std::vector<double> T_guess = T_prev;
    
    for (int nod = 0; nod < n; ++nod) {
        // Homogena noder eller gränsnoder med samma material
        bool is_homogeneous = false;
        
        if (nod == 0 || nod == n-1) {
            is_homogeneous = true;
        } else if (mesh.material_indices[nod-1] == mesh.material_indices[nod]) {
            is_homogeneous = true;
        }
        
        if (is_homogeneous) {
            int mat_idx;
            if (nod == 0) {
                mat_idx = mesh.material_indices[0];
            } else if (nod == n-1) {
                mat_idx = mesh.material_indices[mesh.num_elements-1];
            } else {
                mat_idx = mesh.material_indices[nod];
            }
            
            const MaterialTable& mat = fast_materials[mat_idx];
            double e_target = E[nod] / V[nod];
            
            bool found = false;
            for (size_t j = 0; j < mat.enthalpy.size() - 1; ++j) {
                double ej = mat.enthalpy[j];
                double ej1 = mat.enthalpy[j+1];
                
                if (ej <= e_target && e_target < ej1) {
                    double Tj = j;
                    double Tj1 = j + 1;
                    
                    if (ej1 != ej) {
                        T_out[nod] = Tj + (e_target - ej) / (ej1 - ej) * (Tj1 - Tj);
                    } else {
                        T_out[nod] = Tj; // Fasövergång
                    }
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                T_out[nod] = mat.enthalpy.size() - 1;
            }
        } 
        else {
            int cnt = 0;
            double rel_fel = 1.0;
            
            while (rel_fel > 0.01 && cnt < 50) {
                int mat_left = mesh.material_indices[nod-1];
                int mat_right = mesh.material_indices[nod];
                
                double dx_left = 0.5 * mesh.element_sizes[nod-1];
                double dx_right = 0.5 * mesh.element_sizes[nod];
                
                double e_left, dedT_left;
                fast_materials[mat_left].get_enthalpy(T_guess[nod], e_left, dedT_left);
                
                double e_right, dedT_right;
                fast_materials[mat_right].get_enthalpy(T_guess[nod], e_right, dedT_right);
                
                double E_uppskattad = e_left * dx_left + e_right * dx_right;
                double Cm_nod = dedT_left * dx_left + dedT_right * dx_right;
                
                double T_ny = T_guess[nod] + (E[nod] - E_uppskattad) / Cm_nod;
                
                double sum_T = T_guess[nod] + T_ny;
                if (sum_T > 1e-10) {
                    rel_fel = 2.0 * std::abs(T_guess[nod] - T_ny) / sum_T;
                } else {
                    rel_fel = 0.0;
                }
                
                // Justera temperatur om det tar för lång tid
                if (cnt > 40) {
                    double diff = std::abs(T_guess[nod] - T_ny);
                    if (T_guess[nod] > T_ny) {
                        T_guess[nod] -= 0.05 * diff;
                    } else {
                        T_guess[nod] += 0.05 * diff;
                    }
                } else {
                    T_guess[nod] = T_ny;
                }
                
                cnt++;
            }
            
            T_out[nod] = T_guess[nod];
        }
    }
    
    return T_out;
}
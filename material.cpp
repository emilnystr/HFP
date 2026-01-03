#include "material.h"

void FastMaterial::precompute(const std::vector<double>& temperature_input,
                   const std::vector<double>& rho_input,
                   const std::vector<double>& cp_input,
                   const std::vector<double>& k_input) {
    int table_size = 2002;  
    k.resize(table_size);
    c.resize(table_size);
    rho.resize(table_size);
    
    for (int t = 0; t < table_size; ++t) {
        double temp = (double)t;
        
        // Linjär interpolation
        if (temp <= temperature_input.front()) {
            k[t] = k_input.front();       
            c[t] = cp_input.front();      
            rho[t] = rho_input.front();   
        } else if (temp >= temperature_input.back()) {
            k[t] = k_input.back();
            c[t] = cp_input.back();
            rho[t] = rho_input.back();
        } else {
            size_t i = 0;
            for (; i < temperature_input.size() - 1; ++i) {
                if (temperature_input[i] <= temp && temp < temperature_input[i+1]) {
                    break;
                }
            }
            
            double frac = (temp - temperature_input[i]) / (temperature_input[i+1] - temperature_input[i]);
            k[t] = k_input[i] + frac * (k_input[i+1] - k_input[i]);         
            c[t] = cp_input[i] + frac * (cp_input[i+1] - cp_input[i]);     
            rho[t] = rho_input[i] + frac * (rho_input[i+1] - rho_input[i]);
        }
    }
}


std::vector<MaterialProperty> load_materials_from_directory(
    const std::string& directory_path){

    std::vector<MaterialProperty> materials;

    std::filesystem::path material_directory(directory_path);

    //bläddra igenom filer i mappen
    std::filesystem::directory_iterator directory_iterator(material_directory);

    for (const std::filesystem::directory_entry& entry : directory_iterator) {

        if (!entry.is_regular_file()) {
            continue;
        }

        std::filesystem::path file_path = entry.path();
        std::string file_name = file_path.filename().string();

        if (file_name == "Material.txt") {
            continue;  
        }

        std::string file_extension = file_path.extension().string();
        if (file_extension != ".txt") {
            continue;
        }

        std::ifstream file_stream;
        file_stream.open(file_path);

        MaterialProperty material;

        std::string file_name_without_extension =
            file_path.stem().string();

        material.name = file_name_without_extension;

        double temperature_value = 0;
        double conductivity_value = 0;
        double specific_heat_value = 0;
        double density_value = 0;

        while (file_stream >> temperature_value
                   >> conductivity_value
                   >> specific_heat_value
                   >> density_value) {

        material.temperature.push_back(temperature_value);
        material.conductivity.push_back(conductivity_value);
        material.specific_heat.push_back(specific_heat_value);
        material.density.push_back(density_value);
        
        }

        file_stream.close();

        materials.push_back(material);
    }

    return materials;
}
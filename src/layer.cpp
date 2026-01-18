#include "layer.h"

std::vector<Layer> load_layers_from_file(const std::vector<MaterialProperty>& materials) {
    std::vector<Layer> layers;    
    std::ifstream file("material/Material.txt");
    if (!file.is_open()) {
        std::cout << "  cant open 'material/Material.txt'" << std::endl;
    
    }
    
    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        for (int i = line.size() - 1; i >= 0; i--) {
            if (line[i] != ' ') break; 
            line.erase(i); 

        }
        if (line.empty()) continue;
        
        std::string thickness_string;
        std::string material_name;

        int i = 0;

        while (i < line.size() && line[i] != ' ') {
            thickness_string += line[i];
            i++;
        }

        while (i < line.size() && line[i] == ' ') { 
            i++; 
        }

        while (i < line.size()) {
            material_name += line[i];
            i++;
        }

        double thickness = 0;
        thickness = std::stod(thickness_string);

        int material_index = -1;

        for (int i = 0; i < materials.size(); i++) {
            if (materials[i].name == material_name) {
                material_index = i;
                break;
            }
        }

        layers.push_back({thickness, material_name, material_index});
        
        
    }
    
    return layers;
}
#include "mesh.h"

Mesh create_mesh(const std::vector<Layer>& layers, const parameters& cfg) {
    Mesh mesh;
    double dx_target = cfg.mm_per_layer / 1000;
    
    for (const auto& layer : layers) {
        double thickness_mm = layer.thickness_mm;
        int material_index = layer.material_index;
        
        double thickness_m = thickness_mm / 1000;
        int num_elements;
        double actual_dx;
        
        double remainder = fmod(thickness_mm, cfg.mm_per_layer); //beräkna rest
        
        //om tjocklek och elementstorlek är jämnt delbara
        if (remainder < 0.00000001) {
            num_elements = (int)(thickness_mm / cfg.mm_per_layer);
            actual_dx = dx_target;
        
        //annars lägger vi till ett extra element och beräknar den nya elementstorleken
        } else {
            num_elements = (int)(thickness_mm / cfg.mm_per_layer) + 1;
            actual_dx = thickness_m / num_elements;
        }
        
        for (int j = 0; j < num_elements; j++) {
            mesh.element_sizes.push_back(actual_dx);
            mesh.material_indices.push_back(material_index);
        }
    }
    
    mesh.num_elements = (int)mesh.element_sizes.size();
    mesh.num_nodes = mesh.num_elements + 1;
    
    return mesh;
}
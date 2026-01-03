#ifndef MESH_H
#define MESH_H

#include <vector>
#include <cmath>
#include "config.h"
#include "layer.h"

class Mesh {

    public:
    std::vector<double> element_sizes;
    std::vector<int> material_indices;
    int num_nodes;
    int num_elements;
};

Mesh create_mesh(const std::vector<Layer>& layers, const parameters& cfg);

#endif
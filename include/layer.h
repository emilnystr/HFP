#ifndef LAYER_H
#define LAYER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "material.h"

struct Layer {
    double thickness_mm;
    std::string material_name;
    int material_index;
};

std::vector<Layer> load_layers_from_file(const std::vector<MaterialProperty>& materials);

#endif
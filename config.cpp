#include "config.h"

parameters load_config (std::string& filename) {
    parameters config;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "  Kunde inte öppna " << filename << ". Använder standardvärden." << std::endl;
        return config;
    }

    std::string line;
    //vi läser radvis
    while (std::getline(file, line)) {

        //trimma strängen
        for (int i = line.size() - 1; i >= 0; i--) {
            if (line[i] != ' ') break; //hitta bokstaven
            line.erase(i); //annars tar vi bort tecknet

        }
        if (line.empty()) continue;
        int equals_pos = line.find('=');

        std::string key;
        std::string value;

        for (int i = equals_pos + 2; i < line.size(); i++){
            if (line[i] == ' ') break;
            value += line[i];
        }

        for (int i = 0; i < line.size(); i++){
            if (line[i] == '=') break;
            key += line[i];
        }
        key.pop_back(); //tar bort mellanslaget, viktigt här att användaren skriver indata såhär: model = 1

        if (key == "MODEL") {
            config.model = std::stoi(value);

        } else if (key == "SIMULATION_TIME") {
            config.simulation_time = std::stod(value);

        } else if (key == "NUMBER_OF_MM_PER_LAYER") {
            config.mm_per_layer = std::stod(value);

        } else if (key == "TIME_STEPS_PER_SECOND") {
            config.timeStepsPerSecond = std::stod(value);
            config.time_step = 1.0 / config.timeStepsPerSecond;

        } else if (key == "FIRE_CURVE_EXPOSED") {
            config.fire_curve_exposed = std::stoi(value);

        } else if (key == "CONSTANT_TEMP") {
            config.constant_surface_temp = std::stod(value);
            
        } else if (key == "EPSILON") {
            config.emissivity = std::stod(value);

        } else if (key == "H_EXPOSED") {
            config.h_exposed = std::stod(value);

        } else if (key == "H_AMBIENT") {
            config.h_ambient = std::stod(value);

        } else if (key == "INITIAL_TEMPERATURE") {
            config.initial_temperature = std::stod(value);

        } else if (key == "AMBIENT_TEMPERATURE") {
            config.ambient_temperature = std::stod(value);

        } else if (key == "SIGMA") {
            config.stefan_boltzmann = std::stod(value);
        }
    }
    file.close();
    return config;
}
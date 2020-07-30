#include "mmConfig.h"

mmConfig::mmConfig() : json() {
}

bool mmConfig::initialise() {
    if (std::filesystem::exists("config.json")) {
        std::ifstream input("config.json", std::fstream::in);

        input >> *this;

        return contains("starsector_mm");
    } else {
        std::ofstream output("config.json");

        if (output.is_open()) {
            emplace("starsector_mm", "Written by Iain Laird");
            emplace("install_dir", "");

            output << std::setw(2) << *this;
            
            return true;
        } else return false;
    }
}

bool mmConfig::apply() {
    std::ofstream output("config.json");

    if (output.is_open()) {
        output << std::setw(2) << *this;
        return true;
    }
    else return false;
}

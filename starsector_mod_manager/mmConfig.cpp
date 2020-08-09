#include "mmConfig.h"

mmConfig::mmConfig(fs::path config_file_path) : json(), config_file(config_file_path) {
}

bool mmConfig::initialise() {
    if (fs::exists(config_file)) {
        std::ifstream input(config_file);
        std::stringstream buffer;
        buffer << input.rdbuf();
        //std::string temp = buffer.str();
        std::string temp = std::regex_replace(buffer.str(), std::regex("#.*\\\n"), "\n");
        std::string config_string = std::regex_replace(temp, std::regex(",(\\s*[\\}\\]])"), "$1");

        update(json::parse(config_string));

        return true;
    } else return false;
}

bool mmConfig::init_or_create(std::string key_check, json& create_with) {
    if (initialise()) {
        return contains(key_check);
    } else {
        std::ofstream output(config_file);

        if (output.is_open()) {
            update(create_with);

            output << std::setw(2) << *this;
            
            return true;
        } else return false;
    }
}

bool mmConfig::apply() {
    std::ofstream output(config_file);

    if (output.is_open()) {
        output << std::setw(2) << *this;
        return true;
    }
    else return false;
}

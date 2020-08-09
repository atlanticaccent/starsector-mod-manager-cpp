#ifndef MM_CONFIG
#define MM_CONFIG

#include <fstream>
#include <filesystem>
#include <sstream>
#include <regex>

#include "json.hpp"

using namespace nlohmann;
namespace fs = std::filesystem;

class mmConfig : public json {
public:
	mmConfig(fs::path);

	bool initialise();
	bool init_or_create(std::string key_check, json& create_with);

	bool apply();

private:
	fs::path config_file;
};

#endif // !MM_CONFIG

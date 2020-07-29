#ifndef MM_CONFIG
#define MM_CONFIG

#include <fstream>
#include <filesystem>

#include "json.hpp"

using namespace nlohmann;

class mmConfig : public json {
public:
	mmConfig();

	~mmConfig();

	bool initialise();
};

#endif // !MM_CONFIG

#include "vendor/json/json.hpp"
#include <fstream>
#include <iostream>
using namespace nlohmann;
#ifndef LOG
#define LOG(x) std::cout << x << std::endl
#endif


#ifndef CVAIM_GETJSON_H
#define CVAIM_GETJSON_H


nlohmann::json getJson(std::string fileName);




#endif //CVAIM_GETJSON_H

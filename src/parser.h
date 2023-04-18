#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>
#ifndef CVAIM_PARSER_H
#define CVAIM_PARSER_H

class Vars {
    std::ifstream is;
    std::unordered_map<std::string, int> map;
public:
    Vars(std::string file);
    std::istream& toVal(std::string name);
    template<class T>
    T getVal(std::string name);
    template<class T>
    std::vector<T> getValues(std::string name);
};






#endif //CVAIM_PARSER_H

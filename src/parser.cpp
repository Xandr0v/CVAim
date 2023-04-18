#include "parser.h"




Vars::Vars(std::string file) {
    is.open(file);
    if (!is.is_open()) std::cerr << "failed to open " << file << std::endl;

    std::string line, s;
    for (int nl = 0; std::getline(is, line); nl++) {
        std::stringstream ss(line);
        ss >> s;
        if (s[0] != '#' && !line.empty()) map[s] = nl;
    }

}
std::istream& Vars::toVal(std::string name) {
    if (map.count(name) == 0) {
        std::cerr << "you're trying to get non-existing variable" << std::endl;
        exit(2);
    }
    is.clear();
    is.seekg(std::ios::beg);
    int l = map[name];
    for (int i = 0; i < l; i++) {
        is.ignore(256, '\n');
    }
    is.ignore(256, '=');

    return is;
}

template<class T>
T Vars::getVal(std::string name) {
    toVal(name);
    T n; is >> n;
    return n;
}


template<class T>
std::vector<T> Vars::getValues(std::string name) {
    toVal(name);
    std::vector<T> v;
    T n; is >> n;
    v.push_back(n);
    while (is.peek() == ',') {
        is.ignore();
        T n; is >> n;
        v.push_back(n);
    }
    return v;
}



template int Vars::getVal(std::string name);
template float Vars::getVal(std::string name);
template std::string Vars::getVal(std::string name);
template bool Vars::getVal(std::string name);
template std::vector<bool> Vars::getValues(std::string name);
template std::vector<std::string> Vars::getValues(std::string name);
template std::vector<int> Vars::getValues(std::string name);
template std::vector<float> Vars::getValues(std::string name);

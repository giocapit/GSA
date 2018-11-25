#ifndef utils_hpp
#define utils_hpp
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

template<typename Out>
inline void splitToDouble(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = stod(item);
    }
}

inline std::vector<double> splitToDouble(const std::string &s, char delim) {
    std::vector<double> elems;
    splitToDouble(s, delim, std::back_inserter(elems));
    return elems;
}


template<typename Out>
inline void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}
#endif

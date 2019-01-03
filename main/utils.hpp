#ifndef utils_hpp
#define utils_hpp
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

template<typename type>
inline type convert(const std::string &s){
	return s;
}

template<>
inline int convert(const std::string &s){
	return stoi(s);
}

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

template<typename type>
inline std::vector<type> splitString(const std::string &s, char delim){
	std::vector<type> elems;
	std::stringstream ss;
	ss.str(s);
	std::string item;
	std::back_insert_iterator<std::vector<type>> out = std::back_inserter(elems);
	while (std::getline(ss, item, delim)) {
		*(out++) = convert<type>(item);
	}
	return elems;
}



#endif

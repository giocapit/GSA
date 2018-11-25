#include <iostream>
#include <vector>
#include "settings.hpp"
#include "utils.hpp"

Configs::Configs(){load();};

void Configs::load()
{
	std::string line;
	std::ifstream configFile("configs.txt");
	while ( getline(configFile,line)){
		std::vector<std::string> confLine = split(line,' ');
		configs[confLine[0]] = confLine[1];
	}
	configFile.close();
};

Configs & Configs::instance(){
	static Configs  conf = Configs();
	return conf;
};


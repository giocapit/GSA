#include <string>
#include <map>

class Configs{
	private:
		std::map<std::string,std::string> configs;
			
		Configs();
		void load();
	public:
		~Configs(){};
		static Configs & instance();
		std::string  get(std::string tag){return configs[tag];};
};

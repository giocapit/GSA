#include <functional>
#include <random>
#include <vector>
#include <set>
class GlobalSensitivityCalculator{
	private:
		std::function<double(std::vector<double>)> f_;
		int m_;
		int n_;
		std::mt19937 generator;
		//std::set<int> indices;
		//
		double f_0;
		double D_x;
		double D_y;
		double D;

	public:
		GlobalSensitivityCalculator(std::function<double(std::vector<double>)> f, int m, int n);

		~GlobalSensitivityCalculator(){};

		void calculate(const std::set<int> indices);

		double getD_x(){return D_x;};
		double getD_y(){return D_y;};
};

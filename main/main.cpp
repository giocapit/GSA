#include <iostream>
#include <functional>
#include <set>

#include "GSAFunction.hpp"
#include "GSA.hpp"
#include "settings.hpp"

using namespace std;


int main (int argc,char* argv[]){
	
	std::clock_t start = std::clock();
	double duration;
	Configs conf = Configs::instance();
	string AoD = conf.get("AoD");
	GSAFunction gsa_function(AoD);
	std::function<double(std::vector<double>)> f = 
		[&gsa_function] (std::vector<double> v) {return gsa_function.eval(v[0],v[1], v[2], v[3], v[4], v[5], 1);};
	std::cout << "Base price: " << gsa_function.eval(0.0,0.0,0.0,0.0,0.0,0.0,0) << std::endl;
	int sampleNumber = stoi(conf.get("sample_number"));
	PseudoRandomGaussianGenerator rng;
	shared_ptr<GSAResults> gsa_results(new GSAResults());
	GlobalSensitivityCalculator<PseudoRandomGaussianGenerator> gsa_calculator(rng,f,sampleNumber,6, gsa_results);
	std::vector<int> indexes = gsa_function.getIndices();
	gsa_calculator.calculate(std::set<int>(indexes.begin(),indexes.end()));
	duration = (std::clock() - start)/(double) (CLOCKS_PER_SEC/1000);
	cout << "Calculation time: " << duration << endl;
	cout << "D_x: " << gsa_results->getD_x() << endl;
	cout << "D_y: " << gsa_results->getD_y() << endl;
	cout << "D: " << gsa_results->getD() << endl;
	cout << "f_0: " << gsa_results->getf_0() << endl;
	cout << "S_x: " << gsa_results->getS_x() << endl;
	cout << "S_y: " << gsa_results->getS_y() << endl;
}

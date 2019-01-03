#include <random>
#include <ql/math/randomnumbers/sobolrsg.hpp>

class PseudoRandomGaussianGenerator{

	public:
		PseudoRandomGaussianGenerator(){ randNormal = std::normal_distribution<double> (0.0,1.0);};
		~PseudoRandomGaussianGenerator(){};

		double next();
	private:
		std::mt19937 generator;
		std::normal_distribution<double> randNormal;
};



class QuasiRandomGaussianGenerator{

	public:
		QuasiRandomGaussianGenerator(QuantLib::Size dim):
			rsg(QuantLib::SobolRsg(dim))

	{};
		~QuasiRandomGaussianGenerator(){};

		double next();
	private:
		QuantLib::SobolRsg rsg;
};

inline double PseudoRandomGaussianGenerator::next(){
	return randNormal(generator);
}

inline double QuasiRandomGaussianGenerator::next(){
	return (rsg.nextSequence().value)[0];
}

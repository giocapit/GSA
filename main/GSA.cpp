#include "GSA.hpp"
#include <set>

GlobalSensitivityCalculator::GlobalSensitivityCalculator(std::function<double(std::vector<double>)> f, int m, int n):
	f_(f),m_(m),n_(n)
{};


void GlobalSensitivityCalculator::calculate(const std::set<int> indices)
{
	std::normal_distribution<double> randNormal = std::normal_distribution<double> (0.0,1.0);
	std::vector<double> v;
	D_x=0;
	D_y=0;
	f_0=0;
	D=0;
	for (int i=0; i<m_; ++i){
		std::vector<double> v;
		std::vector<double> w;
		for (int j=0; j< n_; ++j){
			v.push_back(randNormal(generator));
			w.push_back(randNormal(generator));
		}
		std::vector<double> v1=v;
		std::vector<double> v2=w;
		std::set<int>::iterator itr;
		for(itr=indices.begin(); itr!=indices.end();++itr){
			v1[*itr]=w[*itr];
			v2[*itr]=v[*itr];
		}
		double f_v = f_(v);
		D_x+=f_v*f_(v1);
		D_y+=f_v*f_(v2);
		f_0+=f_v;
		D+=f_v*f_v;
	}
	D_x=D_x/m_;
	D_y=D_y/m_;
	f_0=f_0/m_;
	D=D/m_;
}


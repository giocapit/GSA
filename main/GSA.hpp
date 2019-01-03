#include <functional>
#include <vector>
#include <set>
#include <memory>
#include "randomnumbersgenerators.hpp"
class GSAResults{
	private:
		double f_0;
		double D_x;
		double D_y;
		double D;
		double S_x;
		double S_y;

	public:
		GSAResults(){
			D_x=0;
			D_y=0;
			f_0=0;
			D=0;
		};
		~GSAResults(){};

		void collect(double f_v,double f_v1,double f_v2);
		void complete(int m);

		double getD_x(){return D_x;};
		double getD_y(){return D_y;};
		double getD(){return D;};
		double getf_0(){return f_0;};
		double getS_x(){return S_x;};
		double getS_y(){return S_y;};
};

template <class GRNG>
class GlobalSensitivityCalculator{
	private:
		GRNG generator_;
		std::function<double(std::vector<double>)> f_;
		int m_;
		int n_;

		std::shared_ptr<GSAResults> results_;

	public:
		GlobalSensitivityCalculator(GRNG generator, std::function<double(std::vector<double>)> f, int m, int n, std::shared_ptr<GSAResults> results);

		~GlobalSensitivityCalculator(){};

		void calculate(const std::set<int> indices);

		std::shared_ptr<GSAResults> getResults(){return results_;};
};


template <class GRNG>
GlobalSensitivityCalculator<GRNG>::GlobalSensitivityCalculator(GRNG generator, std::function<double(std::vector<double>)> f, int m, int n, std::shared_ptr<GSAResults> results):
	generator_(generator),f_(f),m_(m),n_(n),results_(results)
{};

	template <class GRNG>
void GlobalSensitivityCalculator<GRNG>::calculate(const std::set<int> indices)
{
	for (int i=0; i<m_; ++i){
		std::vector<double> v;
		std::vector<double> w;
		for (int j=0; j< n_; ++j){
			v.push_back(generator_.next());
			w.push_back(generator_.next());
		}
		std::vector<double> v1=w;
		std::vector<double> v2=v;
		std::set<int>::iterator itr;
		for(itr=indices.begin(); itr!=indices.end();++itr){
			v1[*itr]=v[*itr];
			v2[*itr]=w[*itr];
		}
		results_->collect(f_(v), f_(v1), f_(v2));

	}
	results_->complete(m_);
}

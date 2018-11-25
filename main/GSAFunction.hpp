#include <memory>
#include <fstream>
#include <ql/models/shortrate/onefactormodels/vasicek.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include "CCSPricer.hpp"

class GSAFunction{

	private:
		QuantLib::Date today;
		QuantLib::Date settlementDate;
		std::shared_ptr<QuantLib::OneFactorAffineModel> rateModel_ccy1;
		std::shared_ptr<QuantLib::OneFactorAffineModel> rateModel_ccy2;
		std::shared_ptr<QuantLib::OneFactorAffineModel> rateModel_assetCurve;
		std::shared_ptr<QuantLib::OneFactorAffineModel> rateModel_numeraireCurve;
		std::shared_ptr<QuantLib::BlackScholesProcess> process_fx;
		std::vector<double> offsets_ccy1;
		std::vector<double> offsets_ccy2;
		std::vector<double> offsets_assetCurve;
		std::vector<double> offsets_numeraireCurve;
		std::vector<QuantLib::Time> periods_ccy1;
		std::vector<QuantLib::Time> periods_ccy2;
		std::vector<QuantLib::Time> periods_assetCurve;
		std::vector<QuantLib::Time> periods_numeraireCurve;
		std::vector<QuantLib::Date> dates_ccy1;
		std::vector<QuantLib::Date> dates_ccy2;
		std::vector<QuantLib::Date> dates_assetCurve;
		std::vector<QuantLib::Date> dates_numeraireCurve;
		double ccy1_r0;
		double ccy2_r0;
		double assetCurve_r0;
		double numeraireCurve_r0;
		double fx_spot;
		QuantLib::Actual365Fixed dayCount_;
		CCSPricer ccsPricer;

		QuantLib::ext::shared_ptr<QuantLib::YieldTermStructure> createTermStructure(double rate, 
				const std::shared_ptr<QuantLib::OneFactorAffineModel> & rateModel, 
				const std::vector<double> & offsets, 
				const std::vector<QuantLib::Time> & curveTimes, 
				const std::vector<QuantLib::Date> & dates) const;

		std::shared_ptr<QuantLib::YieldTermStructure> createTermStructure(std::vector<double> rates, 
				std::vector<double> offsets);
		void fillCurve(std::ifstream & file, std::vector<double> & curveValues, std::vector<double> & curveOffsets,  std::vector<QuantLib::Time> & times, std::vector<QuantLib::Date> & dates);
	public: 
		GSAFunction(std::string todayAsString);
		~GSAFunction(){};
		double eval(double ccy1_r, 
				double ccy2_r, 
				double asset_r, 
				double numeraire_r,
				double fx,
				double t) const;
	

};

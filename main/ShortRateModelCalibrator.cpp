#include <iostream>
#include <iomanip>
#include <ql/math/optimization/levenbergmarquardt.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/time/all.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include "ShortRateModelCalibrator.hpp"


using namespace QuantLib;
/* class Statistics{
	public:
		Statistics(){};
		~Statistics(){};

};

class MyEndCriteria : public EndCriteria {
	public:
	MyEndCriteria(Size maxIterations,
                    Size maxStationaryStateIterations,
                    Real rootEpsilon,
                    Real functionEpsilon,
                    Real gradientNormEpsilon): 
		EndCriteria(maxIterations,
                    maxStationaryStateIterations,
                    rootEpsilon,
                    functionEpsilon,
                    gradientNormEpsilon){};

        bool checkStationaryFunctionAccuracy(const Real f,
                                             const bool positiveOptimization,
                                             EndCriteria::Type& ecType) const{
		std::cout << "calibration accuracy" << f;
		return EndCriteria::checkStationaryFunctionAccuracy(f, positiveOptimization,ecType);
	}
};*/

AffineModelHelper::AffineModelHelper(const Handle<Quote>& volatility,
		const Handle<YieldTermStructure>& termStructure,
		const std::shared_ptr<OneFactorAffineModel> model,
		const DayCounter& dayCount,
		const Date maturity,
		const Date now,
		const double rate): CalibrationHelper(volatility,termStructure, PriceError),
	dayCount_(dayCount),model_(model),maturity_(maturity),now_(now), rate_(rate){
		yearFraction_ = dayCount_.yearFraction(now_,maturity_,now_,maturity_);
	};

Real AffineModelHelper::blackPrice(Volatility volatility) const{
	return 1/(1 + yearFraction_ * volatility);
};

Real AffineModelHelper::modelValue() const{
	Time maturity = dayCount_.yearFraction(now_,maturity_,now_,maturity_);
	return model_->discountBond(0,maturity,rate_);	
};

int ShortRateModelCalibrator::calibrate(std::vector<double> &offsets, std::vector<double> &rates, std::shared_ptr<OneFactorAffineModel> model, Date todaysDate, double r0) {
	try{
		Calendar calendar = TARGET();

		Actual365Fixed dayCount;

		std::vector<ext::shared_ptr<CalibrationHelper>> bondDiscounts1;

		// flat yield term structure impling 1x5 swap at 5%
		ext::shared_ptr<Quote> flatRate(new SimpleQuote(0.04875825));
		Handle<YieldTermStructure> termStructure(
				ext::make_shared<FlatForward>(
					todaysDate, Handle<Quote>(flatRate),
					Actual365Fixed()));
		for (size_t i = 0; i < rates.size(); ++i ){
			Date maturity = calendar.advance(todaysDate,offsets[i],Days);
			ext::shared_ptr<Quote> rate(new SimpleQuote(rates[i]));
			bondDiscounts1.push_back(ext::shared_ptr<CalibrationHelper> (new 
						AffineModelHelper(Handle<Quote>(rate),
							termStructure,
							model,
							dayCount,
							maturity,
							todaysDate,
							r0)));
		}
		LevenbergMarquardt om;

		model->calibrate(bondDiscounts1, om,
				EndCriteria(400, 100, 1.0e-8, 1.0e-8, 1.0e-8));


		EndCriteria::Type ecType = model->endCriteria();	
		std::cout << "end criterium: " << ecType << std::endl;
		for (size_t i = 0; i < rates.size(); ++i ){
			Real bondDiscount = bondDiscounts1[i]->modelValue();
			Real modelRate = (1 - bondDiscount) / (bondDiscount * 
					ext::dynamic_pointer_cast<AffineModelHelper> (bondDiscounts1[i])->getYearFraction());
			std::cout << modelRate << std::endl;
		}

		std::cout << "function evaluations: " << model->functionEvaluation() << std::endl;

		return 0;

	}catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "unknown error" << std::endl;
		return 1;
	}






}

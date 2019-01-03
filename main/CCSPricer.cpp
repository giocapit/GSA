#include "CCSPricer.hpp"
#include <ql/pricingengines/swap/discountingswapengine.hpp>
#include <ql/termstructures/yield/ratehelpers.hpp>

using namespace QuantLib;
CCSPricer::CCSPricer(Date today, Date settlementDate, Date maturity, double notional,std::vector<double> notionalsCCY2, std::vector<double> fixingsCCY1, std::vector<double>fixingsCCY2): notional_(notional), notionalsCCY2_(notionalsCCY2), eurLibor3M(), euribor3M(){
	this->today = today;
	calendar = TARGET();
	settlementDate = calendar.adjust(settlementDate);
	frequencyCCY1 = Quarterly;
	conventionCCY1 = ModifiedFollowing;
	dayCounterCCY1 = Thirty360(Thirty360::European);
	frequencyCCY2 = Quarterly;
	conventionCCY2 = Unadjusted;
	dayCounterCCY2 = Thirty360(Thirty360::USA);
	fixedSchedule = Schedule(settlementDate, maturity,
			Period(frequencyCCY2),
			calendar, conventionCCY2,
			conventionCCY2,
			DateGeneration::Forward, false);
	floatScheduleCCY2 = Schedule(settlementDate, maturity,
			Period(frequencyCCY2),
			calendar, conventionCCY2,
			conventionCCY2,
			DateGeneration::Forward, false);
	floatScheduleCCY1 = Schedule(settlementDate, maturity,
			Period(frequencyCCY1),
			calendar, conventionCCY1,
			conventionCCY1,
			DateGeneration::Forward, false);
	std::vector<Date> ccy2Schedule = floatScheduleCCY2.dates();

	std::copy(ccy2Schedule.begin(), ccy2Schedule.end(), std::ostream_iterator<Date>(std::cout, " ")); std::cout << std::endl;
	for (size_t i=0; i<fixingsCCY1.size();++i){	
		eurLibor3M.addFixing(calendar.advance(floatScheduleCCY1[i],-2,Days),fixingsCCY1[i]);
	}
	for (size_t i=0; i<fixingsCCY2.size();++i){	
		euribor3M.addFixing(calendar.advance(floatScheduleCCY2[i],-2,Days),fixingsCCY2[i]);
	}
};

double CCSPricer::eval(Rate fxSpot,
		const Handle<YieldTermStructure> & yCCY1Curve,
		const Handle<YieldTermStructure> & yCCY2Curve,
		const Handle<YieldTermStructure> & assetCurve,
		const Handle<YieldTermStructure> & numeraireCurve
		) const {

	try {
		Settings::instance().evaluationDate() = today;
		ext::shared_ptr<Quote> fxSpotRate(new SimpleQuote(fxSpot));

		DayCounter termStructureDayCounter =
			ActualActual(ActualActual::ISDA);

		std::vector<Real> nominalCCY1(floatScheduleCCY2.size() - 1,notional_);

		std::vector<Real> nominalCCY2;

		std::vector<Real> nominalDummy(floatScheduleCCY2.size() - 1,0.0);
		// fixed leg
		DayCounter dayCounterDummy = Thirty360(Thirty360::European);
		std::vector<Rate> fixedRate(nominalCCY1.size(),0.0);

		// floating leg
		ext::shared_ptr<IborIndex> euriborIndexCCY1=eurLibor3M.clone(yCCY1Curve);
		ext::shared_ptr<IborIndex> euriborIndexCCY2=euribor3M.clone(yCCY2Curve);
		Spread spreadCCY2 = 0.0;
		Spread spreadCCY1 = 0.0;

		VanillaSwap::Type swapTypeCCY2 = VanillaSwap::Receiver;
		VanillaSwap::Type swapTypeCCY1 = VanillaSwap::Payer;

		for (Size i=0; i<floatScheduleCCY2.size() - 1;++i)
		{
			if (notionalsCCY2_[0] != 0){
				nominalCCY2.push_back(notionalsCCY2_[i]);
			} else {
				if (floatScheduleCCY2[i] <= today){
					nominalCCY2.push_back(nominalCCY1[i] * fxSpot);
				} else {
					Period mat(floatScheduleCCY2[i] - today,Days);
					std::shared_ptr<RateHelper> fwdPoint(new FxSwapRateHelper(
								Handle<Quote>(ext::shared_ptr<SimpleQuote> (new SimpleQuote(0.0))), Handle<Quote>(fxSpotRate),
								mat, 2, calendar,
								conventionCCY2,
								true,
								true,
								numeraireCurve));
					fwdPoint->setTermStructure((*assetCurve).get());

					Real fxPoints = fwdPoint->impliedQuote();
					Real fxFwd = fxSpot + fxPoints;
					nominalCCY2.push_back(nominalCCY1[i]*fxFwd);
				}
			}

		}

		NonstandardSwap dummyCCY1Swap(swapTypeCCY1, nominalDummy, nominalCCY1,
				floatScheduleCCY1, fixedRate,
				dayCounterDummy, floatScheduleCCY1,
				euriborIndexCCY1, 1, spreadCCY1,
				dayCounterCCY1, false, false, conventionCCY1);
		NonstandardSwap dummyCCY2Swap(swapTypeCCY2, nominalDummy, nominalCCY2,
				floatScheduleCCY2, fixedRate,
				dayCounterDummy, floatScheduleCCY2,
				euriborIndexCCY2, 1, spreadCCY2,
				dayCounterCCY2, false, false, conventionCCY2);

		/***************
		 * SWAP PRICING *
		 ****************/

		Real NPVCCY2;
		Real NPVCCY1;
		Real NPVmySwap;
		Handle<YieldTermStructure> dCCY1Curve=assetCurve;
		Handle<YieldTermStructure> dCCY2Curve = numeraireCurve;
		ext::shared_ptr<PricingEngine> swapEngineCCY1(
				new DiscountingSwapEngine(dCCY1Curve));

		dummyCCY1Swap.setPricingEngine(swapEngineCCY1);

		ext::shared_ptr<PricingEngine> swapEngineCCY2(
				new DiscountingSwapEngine(dCCY2Curve));

		dummyCCY2Swap.setPricingEngine(swapEngineCCY2);


		NPVCCY1 = dummyCCY1Swap.NPV();

		NPVCCY2 = dummyCCY2Swap.NPV();

		//	std::cout << "CCY2 leg N/V:" << NPVCCY2/fxSpot << std::endl;
		//	std::cout << "CCY1 leg NPV:" << NPVCCY1 << std::endl;


		NPVmySwap = NPVCCY2 / fxSpot - NPVCCY1 ;

		//		std::cout << "CCS price:" << std::endl;
		//		std::cout << NPVmySwap << std::endl;

		return NPVmySwap;

	}
	catch (std::exception& e) {
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "unknown error" << std::endl;
		return 1;
	}
}

#include <ql/time/all.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/instruments/nonstandardswap.hpp>

#include <iostream>
#include <iomanip>

class CCSPricer{
	private:
		QuantLib::Date today;
		QuantLib::Frequency frequencyCCY1;
		QuantLib::BusinessDayConvention conventionCCY1;
		QuantLib::DayCounter dayCounterCCY1;
		QuantLib::Frequency frequencyCCY2;
		QuantLib::BusinessDayConvention conventionCCY2;
		QuantLib::DayCounter dayCounterCCY2;
		double notional_;
		QuantLib::Calendar calendar ;

		QuantLib::Schedule fixedSchedule;
		QuantLib::Schedule floatScheduleCCY1;
		QuantLib::Schedule floatScheduleCCY2;

	public:
		CCSPricer(){};
		CCSPricer(QuantLib::Date today, QuantLib::Date settlementDate, QuantLib::Date maturity, double notional);
		double eval(QuantLib::Rate fxSpot,
		const QuantLib::Handle<QuantLib::YieldTermStructure> & yCCY1Curve,
		const QuantLib::Handle<QuantLib::YieldTermStructure> & yCCY2Curve,
		const QuantLib::Handle<QuantLib::YieldTermStructure> & assetCurve,
		const QuantLib::Handle<QuantLib::YieldTermStructure> & numeraireCurve) const;
};

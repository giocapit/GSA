#include <vector>
#include <memory>
#include <ql/models/shortrate/onefactormodels/vasicek.hpp>


class ShortRateModelCalibrator{

	public:
	ShortRateModelCalibrator(){};
	~ShortRateModelCalibrator(){};
	int calibrate(std::vector<double> &offsets, std::vector<double> &rates, std::shared_ptr<QuantLib::OneFactorAffineModel> model, QuantLib::Date todaysDate, double r0);

};


class AffineModelHelper : public QuantLib::CalibrationHelper {
	public:
		AffineModelHelper(const QuantLib::Handle<QuantLib::Quote>& volatility,
				const QuantLib::Handle<QuantLib::YieldTermStructure>& termStructure,
				const std::shared_ptr<QuantLib::OneFactorAffineModel> model,
				const QuantLib::DayCounter& dayCount,
				const QuantLib::Date maturity,
				const QuantLib::Date now,
				const double rate);

		virtual QuantLib::Real modelValue() const;

		virtual QuantLib::Real blackPrice(QuantLib::Volatility volatility) const;

		virtual void addTimesTo(std::list<QuantLib::Time>& times) const {};

		QuantLib::Time getYearFraction() const {return yearFraction_;};

	private:
		const QuantLib::DayCounter dayCount_;
		const QuantLib::Date maturity_;
		const QuantLib::Date now_;
		QuantLib::Time yearFraction_;
		const std::shared_ptr<QuantLib::OneFactorAffineModel> model_;
		const double rate_;

};

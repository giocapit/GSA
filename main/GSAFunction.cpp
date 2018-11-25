#include <iostream>
#include <sstream>
#include <string>
#include <ql/quotes/simplequote.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include "GSAFunction.hpp"
#include "utils.hpp"
#include "ShortRateModelCalibrator.hpp"
using namespace QuantLib;

GSAFunction::GSAFunction(std::string todayAsString){
	Date::serial_type dateAsSerial = stoi(todayAsString);

	today = Date(dateAsSerial);
	
	std::ifstream tradeFile;
	tradeFile.open("trades.txt");
	std::cout << "Reading the trade file..." << std::endl;
	//leggo settlement date
	std::string line;
	std::getline(tradeFile,line);
	std::getline(tradeFile,line);
	Date::serial_type settlementDateAsSerial = stoi(line);
	settlementDate = Date(settlementDateAsSerial);
	std::cout << "settlement date: " << settlementDate << std::endl;
	//leggo curva (valori e offset)
	std::cout << "Reading curves from the trade file..." << std::endl;
	std::vector<double> ccy1CurveValues;
	fillCurve(tradeFile, ccy1CurveValues, offsets_ccy1, periods_ccy1, dates_ccy1);
	std::copy(ccy1CurveValues.begin(), ccy1CurveValues.end(), std::ostream_iterator<double>(std::cout, " ")); std::cout << std::endl;
	std::copy(offsets_ccy1.begin(), offsets_ccy1.end(), std::ostream_iterator<double>(std::cout, " ")); std::cout << std::endl;
	std::vector<double> ccy2CurveValues;
	fillCurve(tradeFile, ccy2CurveValues, offsets_ccy2, periods_ccy2, dates_ccy2);
	std::vector<double> assetCurveValues;
	fillCurve(tradeFile, assetCurveValues, offsets_assetCurve, periods_assetCurve, dates_assetCurve);
	std::vector<double> numeraireCurveValues;
	fillCurve(tradeFile, numeraireCurveValues, offsets_numeraireCurve, periods_numeraireCurve, dates_numeraireCurve);
	//leggo fx Spot:
	std::cout << "Reading fx rate from the trade file..." << std::endl;
	std::getline(tradeFile,line);
	std::getline(tradeFile,line);
	std::cout << line << std::endl;
	fx_spot = stod(line);
	//leggo nominale:
	std::cout << "Reading nominal from the trade file..." << std::endl;
	std::getline(tradeFile,line);
	std::getline(tradeFile,line);
	std::cout << line << std::endl;
	double nominal = stod(line);
	//estraggo dalla curva i parametri del modello
	//
	//	prendo come proxy dello short rate il tasso a 1 g	
	std::shared_ptr<YieldTermStructure> ccy1_ts=createTermStructure(ccy1CurveValues, offsets_ccy1);
	ccy1_r0=ccy1_ts->zeroRate(1/365,Compounding::Continuous,Annual,true);
	std::shared_ptr<YieldTermStructure> ccy2_ts=createTermStructure(ccy2CurveValues, offsets_ccy2);
	ccy2_r0=ccy2_ts->zeroRate(1/365,Compounding::Continuous,Annual,true);
	std::shared_ptr<YieldTermStructure> assetCurve_ts=createTermStructure(assetCurveValues, offsets_assetCurve);
	assetCurve_r0=assetCurve_ts->zeroRate(1/365,Compounding::Continuous,Annual,true);
	std::shared_ptr<YieldTermStructure> numeraireCurve_ts=createTermStructure(numeraireCurveValues, offsets_numeraireCurve);
	numeraireCurve_r0=numeraireCurve_ts->zeroRate(1/365,Compounding::Continuous,Annual,true);

	rateModel_ccy1=std::shared_ptr<OneFactorAffineModel>(new Vasicek(ccy1_r0));
	rateModel_ccy2=std::shared_ptr<OneFactorAffineModel>(new Vasicek(ccy2_r0));
	rateModel_assetCurve=std::shared_ptr<OneFactorAffineModel>(new Vasicek(assetCurve_r0));
	rateModel_numeraireCurve=std::shared_ptr<OneFactorAffineModel>(new Vasicek(numeraireCurve_r0));

	ShortRateModelCalibrator sr_calibrator;
	sr_calibrator.calibrate(offsets_ccy1,ccy1CurveValues,rateModel_ccy1,today,ccy1_r0);
	sr_calibrator.calibrate(offsets_ccy2,ccy2CurveValues,rateModel_ccy2,today,ccy2_r0);
	sr_calibrator.calibrate(offsets_assetCurve,assetCurveValues,rateModel_assetCurve,today,assetCurve_r0);
	sr_calibrator.calibrate(offsets_numeraireCurve,numeraireCurveValues,rateModel_numeraireCurve,today,numeraireCurve_r0);

	//modello per il tasso di cambio
	Handle<Quote> h1(ext::make_shared<SimpleQuote>(SimpleQuote(ccy1_r0)));
	Handle<Quote> h2(ext::make_shared<SimpleQuote>(SimpleQuote(0.30)));
	Handle<YieldTermStructure> flatRate(
			ext::shared_ptr<YieldTermStructure>(
				new FlatForward(0, NullCalendar(),
					h1, dayCount_)));
	Handle<BlackVolTermStructure> flatVol(
			ext::shared_ptr<BlackVolTermStructure>(
				new BlackConstantVol(0, NullCalendar(),
					h2, dayCount_)));
	process_fx = std::shared_ptr<BlackScholesProcess> (
			new BlackScholesProcess(Handle<Quote>(ext::shared_ptr<Quote>(new SimpleQuote(fx_spot))),
				flatRate, flatVol));
	//pricer
	//
	ccsPricer = CCSPricer(today,settlementDate,settlementDate + 1*Years, nominal);
}

double GSAFunction::eval(double ccy1_dw, 
		double ccy2_dw, 
		double assetCurve_dw, 
		double numeraireCurve_dw,
		double fx_dw,
		double t) const{

	//1.calcolo lo short rate per tutte le curve in base al modello
	double ccy1_r = rateModel_ccy1->dynamics()->process()->evolve(0, ccy1_r0, t, ccy1_dw);

	double ccy2_r = rateModel_ccy2->dynamics()->process()->evolve(0, ccy2_r0, t, ccy2_dw);

	double assetCurve_r = rateModel_assetCurve->dynamics()->process()->evolve(0, assetCurve_r0, t, assetCurve_dw);

	double numeraireCurve_r = rateModel_numeraireCurve->dynamics()->process()->evolve(0, numeraireCurve_r0, t, numeraireCurve_dw);

	//2.calcolo gli zero rates delle varie curve

	Handle<YieldTermStructure> ccy1_ts(createTermStructure(ccy1_r, rateModel_ccy1, offsets_ccy1, periods_ccy1, dates_ccy1));
	Handle<YieldTermStructure> ccy2_ts(createTermStructure(ccy2_r, rateModel_ccy2, offsets_ccy2, periods_ccy2, dates_ccy2));
	Handle<YieldTermStructure> assetCurve_ts(createTermStructure(assetCurve_r, rateModel_assetCurve, offsets_assetCurve, periods_assetCurve, dates_assetCurve));
	Handle<YieldTermStructure> numeraireCurve_ts(createTermStructure(numeraireCurve_r, rateModel_numeraireCurve, offsets_numeraireCurve, periods_numeraireCurve, dates_numeraireCurve));

	//3.calcolo il tasso di cambio in base al modello
	//
	double fx = process_fx->evolve(0, fx_spot, t, fx_dw);

	//4.invoco il ccs pricer

	return ccsPricer.eval(fx,ccy1_ts,ccy2_ts,assetCurve_ts,numeraireCurve_ts);

};


ext::shared_ptr<YieldTermStructure> GSAFunction::createTermStructure(double rate,const std::shared_ptr<OneFactorAffineModel> & rateModel,const std::vector<double> & offsets, const std::vector<Time> & curveTimes, const std::vector<Date> & dates) const{

	Calendar calendar = TARGET();
	std::vector<double> rates;
	rates.push_back(0.0);
	for (size_t i = 0; i < curveTimes.size(); ++i ){

		double bondDiscount = rateModel->discountBond(0,curveTimes[i],rate);
		Real modelRate = (1 - bondDiscount) / (bondDiscount * curveTimes[i]);
		rates.push_back(modelRate);
	}
	ext::shared_ptr<YieldTermStructure> termStructure(
			new InterpolatedZeroCurve<Linear>(
				dates, rates,
				dayCount_,
				calendar));
	return termStructure;

};

std::shared_ptr<YieldTermStructure> GSAFunction::createTermStructure(std::vector<double> rates, std::vector<double> offsets){
	Calendar calendar = TARGET();
	std::vector<Date> dates;
	for (size_t i = 0; i < offsets.size(); ++i ){

		dates.push_back(calendar.adjust(calendar.advance(settlementDate,offsets[i],Days)));
	}
	std::shared_ptr<YieldTermStructure> termStructure(
			new InterpolatedZeroCurve<Linear>(
				dates, rates,
				dayCount_,
				calendar));
	return termStructure;

};

void GSAFunction::fillCurve(std::ifstream & file, std::vector<double> & curveValues, std::vector<double> & curveOffsets, std::vector<Time> & times, std::vector<Date> & dates){
	Calendar calendar = TARGET();
	std::string line;
	std::getline(file,line);
	std::getline(file,line);
	curveValues = splitToDouble(line,',');
	std::getline(file,line);
	std::getline(file,line);
	curveOffsets = splitToDouble(line,',');
	dates.push_back(today);
	for (size_t i = 0; i < curveOffsets.size(); ++i ){

		Date maturity = calendar.adjust(calendar.advance(today,curveOffsets[i],Days));
		times.push_back(dayCount_.yearFraction(today,maturity,today,maturity));
		dates.push_back(maturity);
	}
};

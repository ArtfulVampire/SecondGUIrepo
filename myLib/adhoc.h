#ifndef ADHOC_H
#define ADHOC_H

#include <vector>

#include <QString>

namespace myLib
{

#if 0
	/// wavelet
	edfFile fl("/media/Files/Data/AAX/AAX_final.edf");
	std::valarray<double> sig = smLib::contSubsec(fl.getData(10), 0, 2000); /// 10 sec
	auto m = wvlt::myCWT(sig, smLib::range<std::vector<double>>(5., 21., 1.));
	for(const auto & row : m)
	{
		std::cout << smLib::sigma(row) / smLib::sigma(m[0]) << "\n";
	}
	std::cout << std::endl;

	wvlt::drawWavelet(m).save("/media/Files/Data/1.jpg", nullptr, 100);
	exit(0);
#endif

#if 0
	/// filtering
	edfFile fl("/media/Files/Data/AAX/AAX_final.edf");
	std::valarray<double> sig = smLib::contSubsec(fl.getData(10), 0, 2000); /// 10 sec
	myLib::drw::drawOneSignal(sig).save("/media/Files/Data/1.jpg");

	auto sig2 = myLib::refilterFFT(sig, 1, 4, false, 250);
	myLib::drw::drawOneSignal(sig2).save("/media/Files/Data/6.jpg");

	std::valarray<double> res1 = butter::butterworthBandPass(sig, 1, 4, 250);
	myLib::drw::drawOneSignal(res1).save("/media/Files/Data/2.jpg");

	std::valarray<double> res2 = smLib::contReverse(res1);
	myLib::drw::drawOneSignal(res2).save("/media/Files/Data/3.jpg");

	res1 = butter::butterworthBandPass(res2, 1, 4, 250);
	myLib::drw::drawOneSignal(res1).save("/media/Files/Data/4.jpg");

	sig = smLib::contReverse(res1);
	myLib::drw::drawOneSignal(sig).save("/media/Files/Data/5.jpg");
	exit(0);
#endif

/// some useful functions mainly for mainwindow_customF.cpp
std::vector<QString> readBurdenkoLog(const QString & logPath);

std::vector<std::pair<int, int>> handleBurdenkoAnnots(const std::vector<QString> & annotations,
													  double srate);
void burdenkoAnnotsSimpler();
void burdenkoStuff();
void burdenkoReactionTime();
void burdenkoReadEdfPlus();
void burdenkoReadEdf();
void burdenkoEdfPlusToEdf();


std::vector<QString> composeMarkersList(const QString & workPath); /// really unused


void elenaCalculation(const QString & realsPath,
					  const QString & outSpectraPath,
					  const QString & outTableDir);

std::vector<QString> checkChannels(const QString & inPath, const std::vector<QString> & refChans);

void XeniaPretable();
void SettingsXenia();
void XeniaProcessing(const QString & workPath);
void XeniaWithAverage(const QString & workPath);
void XeniaFinal();


void preprocessDir(const QString & inPath);
void SettingsGalya();
void GalyaProcessing(const QString & workPath);

void elenaCalculation(const QString & realsPath,
					  const QString & outSpectraPath,
					  const QString & outTableDir);

void feedbackFinal();

void BaklushevHistograms();
void matiConcat();
void testEgiChns128();

void checkFeedbackFiles();
void binomialToFile();
void binomialThresholds();
void drawFftWindows();
void drawEnvelope();
void HiguchiTests();

} /// end namespace myLib

#endif // ADHOC_H

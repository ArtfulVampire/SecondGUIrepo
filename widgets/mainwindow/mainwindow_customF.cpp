#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <other/subjects.h>
#include <other/feedback_autos.h>
#include <other/autos.h>
#include <other/coords.h>
#include <myLib/adhoc.h>
#include <myLib/drw.h>
#include <myLib/iitp.h>
#include <myLib/signalProcessing.h>
#include <myLib/ica.h>
#include <myLib/dataHandlers.h>
#include <myLib/temp.h>
#include <myLib/statistics.h>
#include <myLib/wavelet.h>
#include <myLib/valar.h>

using namespace myOut;

void MainWindow::customFunc()
{
//	const QString workDir = "c:/Data/tactile";
//	autos::cutFilesInFolder(workDir, 8);

//	myLib::preprocessDir("C:/Data/tactile/coma_processed");
//	myLib::GalyaProcessing("C:/Data/tactile/coma_processed");

//	myLib::checkChannels("/media/Files/Data/Galya/all_buben", coords::lbl19);
//	exit(0);

	myLib::XeniaWithAverage("/media/Files/Data/Xenia/FINAL");
	exit(0);
#if 0
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
}

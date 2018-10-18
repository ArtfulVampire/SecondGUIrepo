#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <other/subjects.h>
#include <other/feedback_autos.h>
#include <myLib/adhoc.h>
#include <myLib/drw.h>
#include <myLib/iitp.h>
#include <myLib/signalProcessing.h>
#include <myLib/ica.h>
#include <myLib/dataHandlers.h>
#include <myLib/temp.h>
#include <myLib/statistics.h>
#include <myLib/wavelet.h>

#include <thread>

using namespace myOut;

void MainWindow::customFunc()
{
//	return;
	myLib::elenaCalculation("/media/Files/Data/Elena/Karpenuk/Reals",
							"/media/Files/Data/Elena/Karpenuk/SpectraSmooth",
							"/media/Files/Data/Elena/Karpenuk");

//	const QString workDir = "c:/Data";
//	repair::fullRepairDir(workDir);
//	autos::cutFilesInFolder(workDir, 8);
//	myLib::GalyaProcessing("tactile15Oct18");
//	repair::fullRepairDir(def::GalyaFolder + "/kids15Oct18");
//	myLib::GalyaProcessing("kids15Oct18");
//	exit(0);
//	myLib::XeniaFinal();
//	exit(0);
}

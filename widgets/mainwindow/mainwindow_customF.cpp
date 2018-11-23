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

//	myLib::preprocessDir("C:/Data/aut/Group");
	myLib::GalyaProcessing("C:/Data/aut");


//	myLib::preprocessDir("C:/Data/svzv-test/PERTSULIANNI");
//	myLib::GalyaProcessing("C:/Data/svzv-test");

	//myLib::checkChannels("C:/Data/aut", coords::lbl19);
//	exit(0);
}

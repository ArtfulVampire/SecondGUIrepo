#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

//#include <other/subjects.h>
#include <classifier/classifierdata.h>
#include <other/autos.h>
#include <other/feedback_autos.h>
#include <other/coords.h>
#include <other/defs.h>
#include <myLib/adhoc.h>
//#include <myLib/drw.h>
//#include <myLib/iitp.h>
//#include <myLib/signalProcessing.h>
//#include <myLib/ica.h>
//#include <myLib/dataHandlers.h>
//#include <myLib/statistics.h>
//#include <myLib/wavelet.h>
//#include <myLib/valar.h>

using namespace myOut;

void MainWindow::customFunc()
{
//	const QString workDir = "c:/Data/tactile";
//	autos::cutFilesInFolder(workDir, 8);

//	myLib::preprocessDir("C:/Data/tactile/coma_processed");
//	myLib::GalyaProcessing("C:/Data/tactile/coma_processed");

//	myLib::checkChannels("/media/Files/Data/Galya/all_buben", coords::lbl19);
//	exit(0);

	return;

	DEFS.setUser(username::PolinaM);
	const QString path = "/media/Files/Data/PolinaDataClean/";
	for(QString fl : QDir(path).entryList({"*.edf"}))
	{
		setEdfFile(path + fl);
		sliceAll(); /// slice as Polina
		countSpectraSimple(2048, 10);
		ClassifierData dt(path + "SpectraSmooth");
		dt.toCsv(path + globalEdf.getExpNameShort() + ".txt");
		myLib::cleanDir(path + "Reals");
		myLib::cleanDir(path + "SpectraSmooth");
//		break;
	}
	exit(0);
}

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
	myLib::GalyaProcessing("C:/Data/tactile/coma_processed");

//	myLib::checkChannels("/media/Files/Data/Galya/all_buben", coords::lbl19);
//	exit(0);

	if(01)
	{
//		QString filePath = QFileDialog::getOpenFileName(this,
//														tr("choose txt file"),
//														"/media/Files/Data/Elena/VegetTxt",
//														"*.txt");
		QString filePath = "/media/Files/Data/Elena/VegetTxt/Kristina_verb.txt";
		edfFile fil(filePath, inst::veget);
		filePath.replace(".txt", ".edf");
		fil.writeEdfFile(filePath);
//		exit(0);
	}

//	myLib::XeniaWithAverage("/media/Files/Data/Xenia/FINAL");
//	exit(0);

}

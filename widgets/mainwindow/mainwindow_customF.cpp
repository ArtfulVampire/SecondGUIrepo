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

	if(01)
	{
		QString dataPath = QFileDialog::getOpenFileName(this,
														tr("Choose data file"),
														DEFS.getDirPath(),
														"*.txt");
		edfFile fil(dataPath, inst::veget);

		const QString marksPath = QFileDialog::getOpenFileName(this,
															   tr("Choose markers file"),
															   DEFS.getDirPath(),
															   "*.txt");

		int sta = fil.findMarker({1,2,3,4});

		std::vector<std::pair<QString, double>> marks = myLib::readVegetMarkers(marksPath);
		for(const auto & in : marks)
		{
			int val{0};
			if(in.first.contains("Kross"))							{ val = 255; }
			else if(in.first.contains("Answer"))					{ val = 254; }
			else
			{
				QRegExp reg = QRegExp(R"([0-9]{1,3})");
				int a = reg.indexIn(in.first);
				if(a != -1)			{ val = reg.cap(0).toInt(); }
			}
			if(val != 0)
			{
				std::cout << sta << "\t" << val << std::endl;
				fil.setMarker(sta, val);
			}
			sta += in.second * fil.getFreq();
		}
		dataPath.replace(".txt", ".edf");
		fil.writeEdfFile(dataPath);
	}
}

#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <widgets/choosechans.h>

#include <myLib/drw.h>
#include <myLib/iitp.h>
#include <myLib/signalProcessing.h>
#include <myLib/ica.h>
#include <myLib/dataHandlers.h>
#include <myLib/temp.h>
#include <myLib/statistics.h>
#include <myLib/wavelet.h>
#include <myLib/adhoc.h>
#include <other/subjects.h>
#include <other/feedback_autos.h>
#include <thread>

using namespace myOut;

void MainWindow::customFunc()
{
	const QString workDir = "c:/Data";
	repair::fullRepairDir(workDir);
	autos::cutFilesInFolder(workDir, 8);

}
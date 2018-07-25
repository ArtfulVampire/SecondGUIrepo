#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/signalProcessing.h>
#include <myLib/output.h>
#include <myLib/dataHandlers.h>
#include <myLib/ica.h>

using namespace myOut;

void MainWindow::ICA() /// fastICA
{
	const uint numOfICs = ui->numOfIcSpinBox->value(); /// usually 19
	const double eigenValuesTreshold = std::pow(10., - ui->svdDoubleSpinBox->value());
	const double vectorWTreshold = std::pow(10., - ui->vectwDoubleSpinBox->value());
	const int numDataChannels = 19;

    QTime wholeTime;
    wholeTime.start();

    QTime myTime;
    myTime.start();

	matrix centeredData = globalEdf.getData();

	/// remember all the rest channels (eog, veget, mark, etc)
	matrix resMatBackup = centeredData.subRows(
							  smLib::range<std::vector<uint>>(numDataChannels,
															  centeredData.rows()));
	centeredData.resizeRows(numDataChannels);


	myLib::ICAclass icaClassInstance(centeredData);
	/// set params
	icaClassInstance.setNumIC(numOfICs);
	icaClassInstance.setVectWThreshold(vectorWTreshold);
	icaClassInstance.setEigValThreshold(eigenValuesTreshold);
	/// set output paths
	icaClassInstance.setExpName(globalEdf.getExpNameShort());
	icaClassInstance.setOutPaths(DEFS.dirPath() + "/Help");
	/// calculate result
	icaClassInstance.calculateICA();
	/// write something to files
	icaClassInstance.printExplainedVariance();
	icaClassInstance.printMapsFile();
	icaClassInstance.drawMaps();
	outStream << "number of bad evaluated figures = "
			  << icaClassInstance.getNumOfErrors(DEVNULL)
			  << std::endl;


	matrix components = icaClassInstance.getComponents();
#if 0
	/// append all non-EEG channels
	components.vertCat(resMatBackup);
#else
	/// append only markers
	components.push_back(globalEdf.getMarkArr());
#endif

	/// save ICA file as a new edf
	std::vector<int> chanList(numOfICs);
	std::iota(std::begin(chanList), std::end(chanList), 0);
    chanList.push_back(globalEdf.getMarkChan());

	/// write new edf
	QString helpString = DEFS.dirPath()
						 + "/" + globalEdf.getExpName() + "_ica.edf";
	globalEdf.writeOtherData(components, helpString, chanList);


	outStream << "ICA: time = " << wholeTime.elapsed() / 1000. << " sec" << std::endl;
}

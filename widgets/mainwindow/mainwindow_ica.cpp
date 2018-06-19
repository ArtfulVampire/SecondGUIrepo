#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <myLib/signalProcessing.h>
#include <myLib/output.h>
#include <myLib/dataHandlers.h>
#include <myLib/ica.h>

using namespace myOut;

void MainWindow::ICA() // fastICA
{
	const uint numOfICs = ui->numOfIcSpinBox->value(); /// usually 19
	const double eigenValuesTreshold = std::pow(10., - ui->svdDoubleSpinBox->value());
	const double vectorWTreshold = std::pow(10., - ui->vectwDoubleSpinBox->value());
	const QString pathForAuxFiles = DEFS.dirPath() + "/Help/ica";
	const int numDataChannels = 19;
	const QString eigMatPath = pathForAuxFiles
							   + "/" + globalEdf.getExpName()
							   + "_eigenMatrix.txt";
	const QString eigValPath = pathForAuxFiles
							   + "/" + globalEdf.getExpName()
							   + "_eigenValues.txt";
	const QString explVarPath = pathForAuxFiles
								+ "/" + globalEdf.getExpName()
								+ "_explainedVariance.txt";
	const QString mapsFilePath = pathForAuxFiles
								 + "/" + globalEdf.getExpName()
								 + "maps.txt";

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
	icaClassInstance.setEigMatPath(eigMatPath);
	icaClassInstance.setEigValPath(eigValPath);
	icaClassInstance.setExplVarPath(explVarPath);
	icaClassInstance.setMapsFilePath(mapsFilePath);
	icaClassInstance.setDrawMapsPath(pathForAuxFiles + "/maps");
	icaClassInstance.setExpName(globalEdf.getExpNameShort());
	/// calculate result
	icaClassInstance.calculateICA();
	/// write something to files
	icaClassInstance.printExplainedVariance();
	icaClassInstance.printMapsFile();
	icaClassInstance.drawMaps();
	std::cout << "number of bad evaluated figures = "
			  << icaClassInstance.getNumOfErrors(DEVNULL)
			  << std::endl;


	matrix components = icaClassInstance.getComponents();
//	components.vertCat(resMatBackup); /// comment to drop all non-EEG channels
	components.push_back(globalEdf.getMarkArr());

	/// save ICA file as a new edf
	std::vector<int> chanList(numOfICs);
	std::iota(std::begin(chanList), std::end(chanList), 0);
    chanList.push_back(globalEdf.getMarkChan());

	/// write new edf
	QString helpString = DEFS.dirPath()
						 + "/" + globalEdf.getExpName() + "_ica.edf";
	globalEdf.writeOtherData(components, helpString, chanList);


	std::cout << "ICA ended. time = " << wholeTime.elapsed() / 1000. << " sec" << std::endl;
}

#include <widgets/cut.h>
#include "ui_cut.h"

#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/dataHandlers.h>
#include <myLib/signalProcessing.h>
#include <myLib/qtlib.h>
#include <myLib/output.h>

using namespace myOut;


void Cut::iitpAutoCorrSlot()
{
	edfFile tmpFile = edfFil;
	tmpFile.iitpSyncAutoCorr(ui->rightLimitSpinBox->value(),
							 ui->leftLimitSpinBox->value(),
							 ui->iitpByEegCheckBox->isChecked());
	QString newName = tmpFile.getFileNam();
	newName.replace(".edf", "_sync.edf");
	std::cout << "iitpAutoSlot: newFileName = " << newName << std::endl;
	tmpFile.writeEdfFile(tmpFile.getDirPath() + "/" + newName);
}

void Cut::iitpAutoJumpSlot()
{
	auto lims = edfFil.iitpSyncAutoJump(ui->rightLimitSpinBox->value(),
										ui->leftLimitSpinBox->value(),
										ui->iitpByEegCheckBox->isChecked());

	ui->rightLimitSpinBox->setValue(lims.second);
	ui->leftLimitSpinBox->setValue(lims.first);
	paint();

}

void Cut::rectifyEmgSlot()
{
	this->subtractMeansSlot();
	for(int i = 0; i < edfFil.getNs(); ++i)
	{
		if(edfFil.getLabels(i).contains("EMG", Qt::CaseInsensitive))
		{
			dataCutLocal[i] = dataCutLocal[i].apply(std::abs);
		}
	}
	paint();
}

void Cut::iitpManualSlot()
{
	int artefac	= ui->leftLimitSpinBox->value();
	int ecg		= ui->rightLimitSpinBox->value();

	if(ui->iitpInverseCheckBox->isChecked()) { std::swap(ecg, artefac); }

	edfFil.iitpSyncManual(ecg,
						  artefac,
						  std::min(200, ui->leftLimitSpinBox->value()));
	iitpLog("sync");

	setValuesByEdf();
	ui->iitpDisableEcgCheckBox->setChecked(true);
	paint();
}


void Cut::iitpLog(const QString & typ, int num, const QString & add)
{
	std::ofstream outStr;
	QString name = edfFil.getExpNameShort();
	outStr.open((edfFil.getDirPath() + "/" +
				 name + "_syncLog.txt").toStdString(), std::ios_base::app);
	outStr << edfFil.getExpName() << "\t"
		   << typ << "\t"
		   << ui->leftLimitSpinBox->value() << "\t";
	if(num == 2)
	{
		outStr << ui->rightLimitSpinBox->value() << "\t";
	}
	if(!add.isEmpty())
	{
		outStr << add << "\t";
	}
	outStr << std::endl;
	outStr.close();
}



void Cut::saveNewNumSlot()
{
	QString newName = edfFil.getFileNam();
	newName.replace(QRegExp("[0-9]{2}"), rn(ui->iitpSaveNewNumSpinBox->value(), 2));

	edfFil.writeOtherData(dataCutLocal.subCols(ui->leftLimitSpinBox->value(),
										ui->rightLimitSpinBox->value()),
						  edfFil.getDirPath() + "/" + newName);

	std::cout << "Cut::saveNewNumSlot: edfFile saved - " << newName << std::endl;
	iitpLog("savNewN", 2, newName);

}

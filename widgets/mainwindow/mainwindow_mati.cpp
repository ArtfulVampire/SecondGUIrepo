#include <widgets/mainwindow.h>
#include <ui_mainwindow.h>
#include <myLib/mati.h>


void MainWindow::markerSaveEdf()
{
	QString helpString;
	helpString = DEFS.dirPath() + "/" + globalEdf.getExpName() + ui->newEdfNameLineEdit->text();
	if(!helpString.endsWith(".edf", Qt::CaseInsensitive))
	{
		helpString += ".edf";
	}
	helpString = myLib::setFileName(helpString);
	globalEdf.writeEdfFile(helpString);
}


void MainWindow::markerSetSecTime(int a)
{
	ui->markerSecTimeDoubleSpinBox->setValue(double(a) / DEFS.getFreq());
}

void MainWindow::markerGetSlot()
{
	bool byteMarker[16]; /// remake
	int timeIndex = ui->markerBinTimeSpinBox->value();
	int marker = 0;
	marker = globalEdf.getData()[globalEdf.getMarkChan()][timeIndex];
	QString helpString;

	for(int h = 0; h < 16; ++h)
	{
		byteMarker[16-1 - h] = (marker%(int(std::pow(2, h+1)))) / (int(std::pow(2,h)));
	}

	helpString.clear();
	for(int h = 0; h < 8; ++h)
	{
		helpString += nm(byteMarker[h]);
	}
	ui->markerBin0LineEdit->setText(helpString);

	helpString.clear();
	for(int h = 0; h < 8; ++h)
	{
		helpString += nm(byteMarker[h + 8]);
	}
	ui->markerBin1LineEdit->setText(helpString);

	ui->markerDecimalLineEdit->setText(nm(marker));
}

void MainWindow::markerSetSlot()
{
	int timeIndex = ui->markerBinTimeSpinBox->value();
	int marker = ui->markerDecimalLineEdit->text().toInt();
	globalEdf.setData(globalEdf.getMarkChan(), timeIndex, marker);
}

void MainWindow::markerSetDecValueSlot()
{
	int marker = 0;
	QString helpString;

	helpString = ui->markerBin0LineEdit->text();
	for(int h = 0; h < 8; ++h)
	{
		if(helpString[h] != '0' && helpString[h] != '1') { return; }
		marker += std::pow(2, 8) * (helpString[h] == '1') * std::pow(2, 7-h);
	}

	helpString = ui->markerBin1LineEdit->text();
	for(int h = 0; h < 8; ++h)
	{
		if(helpString[h] != '0' && helpString[h] != '1') { return; }
		marker += (helpString[h] == '1') * std::pow(2, 7-h);
	}
	ui->markerDecimalLineEdit->setText(nm(marker));
}

void MainWindow::markerSetBinValueSlot()
{
	int marker = ui->markerDecimalLineEdit->text().toInt();


	std::vector<bool> byteMarker = myLib::matiCountByte(double(marker));

	QString helpString;
	for(int h = 15; h >= 8; --h)
	{
		helpString += (byteMarker[h] ? QString("1") : QString("0"));
	}
	ui->markerBin0LineEdit->setText(helpString);

	helpString.clear();
	for(int h = 7; h >= 0; --h)
	{
		helpString += (byteMarker[h] ? "1" : "0");
	}
	ui->markerBin1LineEdit->setText(helpString);
}



void MainWindow::matiPreprocessingSlot()
{
	/// REWORK
	/// SPLIT INTO SOME FUNCTIONS
	/// for eeg+amod vertical concatenation

	DEFS.setDir(DEFS.dirPath()); /// "/media/Files/Data/Mati/"
	QStringList dirList = DEFS.dirEntryList(QDir::Dirs|QDir::NoDotAndDotDot); /// list of folders
	bool flagCommaDot = false;
	bool flagAmodLogToEdf = false;
	bool flagSliceEdfBySessions = false;
	bool flagAppendAmodToEeg = false;
	bool flagMakeDiffMark = false;
	bool flagSliceSessionsToPieces = false;

//	flagCommaDot = true;
//	flagAmodLogToEdf = true;
//	flagSliceEdfBySessions = true;
//	flagAppendAmodToEeg = true;
//	flagSliceSessionsToPieces = true;
	flagMakeDiffMark = true;


	QString fileSuffix = "_w";
//	fileSuffix = "_rr_f";


	for(int dirNum = 0; dirNum < dirList.length(); ++dirNum)
	{
		if(!dirList[dirNum].contains("SDA")) { continue; }

		if(flagCommaDot) /// change , to . in amod logFiles
		{
			ui->progressBar->setValue(0.);
			QTime myTime;
			myTime.start();
			outStream << "comma->dot replace start" << std::endl;

			char ch;
			QString helpString = DEFS.dirPath() + "/" + dirList[dirNum] + "/amod";
			DEFS.setDir(helpString);
			QStringList lst = DEFS.dirEntryList(QStringList("*.txt"));
			FILE * fil;
			FILE * fil1;
			for(int i = 0; i < lst.length(); ++i)
			{
				helpString = DEFS.dirPath()
						+ "/" + dirList[dirNum]
						+ "/amod"
						+ "/" + lst[i];
				fil = fopen(helpString.toStdString().c_str(), "r");
				helpString = DEFS.dirPath()
						+ "/" + dirList[dirNum]
						+ "/amod"
						+ "/" + lst[i] + "_";
				fil1 = fopen(helpString.toStdString().c_str(), "w");
				while(!feof(fil))
				{
					fread(&ch, sizeof(char), 1, fil);
					if(ch == ',') { ch = '.'; }
					fwrite(&ch, sizeof(char), 1, fil1);
				}
				fclose(fil);
				fclose(fil1);
				ui->progressBar->setValue(i * 100. / lst.length());
			}

			outStream << "comma->dot replace: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
			ui->progressBar->setValue(0.);
		}

		if(flagAmodLogToEdf) /// corrected logFiles to amod edfs
		{
			ui->progressBar->setValue(0.);
			QTime myTime;
			myTime.start();
			outStream << "amod.txt -> amod.edf start" << std::endl;
			QString helpString = DEFS.dirPath() + "/" + dirList[dirNum] + "/amod";
			DEFS.setDir(helpString);
			QStringList lst = DEFS.dirEntryList(QStringList("*.txt_")); /// _ for corrected logs

			edfFile tempEdf;
			for(int i = 0; i < lst.length(); ++i)
			{
				helpString = DEFS.dirPath()
						+ "/" + dirList[dirNum]
						+ "/amod"
						+ "/" + lst[i];
				tempEdf = edfFile(helpString); /// edfFile(QString) for MATI only

				helpString = DEFS.dirPath()
						+ "/auxEdfs"
						+ "/" + dirList[dirNum]
						+ "/" + myLib::getExpNameLib(lst[i])
						+ "_amod.edf";
				tempEdf.writeEdfFile(helpString);

				ui->progressBar->setValue(i * 100. / lst.length());
			}
			outStream << "amod.txt -> amod.edf: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
			ui->progressBar->setValue(0);
		}

		if(flagSliceEdfBySessions) /// slice initial edf to edf sessions
		{
			QTime myTime;
			myTime.start();
			outStream << "slice edf by sessions start" << std::endl;

			QString helpString;
			helpString = DEFS.dirPath()
					+ "/" + dirList[dirNum]
					+ "/" + dirList[dirNum] + fileSuffix
					+ ".edf";
			if(!QFile(helpString).exists())
			{
				outStream << "cant open ExpName_fileSuffix.edf (flagSliceEdfBySessions)" << std::endl;
				break;
			}
			setEdfFile(helpString);
			sliceMati();

			outStream << "slice edf by sessions: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
		}


		if(flagAppendAmodToEeg) /// append amod data to EEG data
		{
			QTime myTime;
			myTime.start();
			outStream << "append amod.edf to eeg.edf start" << std::endl;


			QString outPath;
			QString addPath;
			QString helpString;
			edfFile tempEdf;
			for(int type = 0; type < 3; ++type)
			{
				for(int session = 0; session < 6; ++session)
				{
					/// session path
					helpString = DEFS.dirPath()
							+ "/" + dirList[dirNum]
							+ "/auxEdfs"
							+ "/" + dirList[dirNum] + fileSuffix
							+ "_" + nm(type)
							+ "_" + nm(session)
							+ ".edf";

					if(!QFile(helpString).exists())
					{
						outStream << "cant open session.edf (flagAppendAmodToEeg)" << std::endl;
						continue;
					}

					tempEdf.readEdfFile(helpString);

					/// amod file
					addPath = DEFS.dirPath()
							+ "/" + dirList[dirNum]
							+ "/auxEdfs"
							+ "/" + dirList[dirNum]
							+ "_" + nm(type)
							+ "_" + nm(session)
							+ "_amod.edf"; /// generality

					/// eeg+amod path
					outPath = DEFS.dirPath()
							+ "/" + dirList[dirNum]
							+ "/auxEdfs"
							+ "/" + dirList[dirNum] + fileSuffix
							+ "_a"
							+ "_" + nm(type)
							+ "_" + nm(session)
							+ ".edf"; /// generality

					tempEdf.vertcatFile(addPath, outPath);

#if 0
					/// copy the same files into amod dir

					helpString = DEFS.dirPath()
								 + "/" + dirList[dirNum]
								 + "/amod"
								 + "/" + dirList[dirNum] + fileSuffix
								 + "_a"
								 + "_" + nm(type)
								 + "_" + nm(session)
								 + ".edf"; /// generality
					if(QFile::exists(helpString)) QFile::remove(helpString);
					QFile::copy(outPath, helpString);
#endif


				}
			}
			outStream << "append amod.edf to eeg.edf: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
		}

		if(flagMakeDiffMark) /// make files of markers differences
		{
			QTime myTime;
			myTime.start();
			outStream << "make diffMark files start" << std::endl;

			edfFile fil;
			QString diffMark;
			QString helpString;
			for(int type = 0; type < 3; ++type)
			{
				for(int session = 0; session < 6; ++session)
				{
					/// eeg+amod path
					helpString = DEFS.dirPath()
							+ "/" + dirList[dirNum]

							+ "/auxEdfs"

							+ "/" + dirList[dirNum] + fileSuffix
							+ "_a"
							+ "_" + nm(type)
							+ "_" + nm(session)
							+ ".edf";
					if(!QFile::exists(helpString)) { continue; }

					diffMark = helpString;
					diffMark.replace(".edf", "_diffMark.txt");

					fil.readEdfFile(helpString);
					std::ofstream outStr(diffMark.toStdString().c_str());
					outStr << "board" << '\t' << "amod" << '\t' << std::endl;

					for(int i = 0; i < fil.getDataLen(); ++i)
					{
						if(fil.getData()[fil.getMarkChan()][i] == 0) { continue; }
						if(myLib::matiCountBit(fil.getData()[fil.getMarkChan()][i], 14))
						{
							for(int j = i - 40; j < i + 40; ++j)
							{
								if(fil.getData()[37][j+1] != fil.getData()[37][j]) /// 37 for EEG + AMOD
								{
									outStr << i << "\t" << j+1 << "\t" << (i-j-1)*4 << '\n';
									break;
								}
							}
						}
					}
					outStr.flush();
					outStr.close();
				}
			}

			outStream << "make diffMark files: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
		}

		if(flagSliceSessionsToPieces) /// slice constructed eeg+amod files for small pieces
		{
			/// not finished
			QTime myTime;
			myTime.start();
			outStream << "slice sessions to pieces start" << std::endl;

			QString outPath;
			QString helpString;
			helpString = DEFS.dirPath()
					+ "/" + dirList[dirNum]

					+ "/auxEdfs"

					+ "/" + dirList[dirNum] + fileSuffix
					+ "_a"
					+ "_0"
					+ "_1"
					+ ".edf"; /// generality
			outPath = DEFS.dirPath()
					+ "/" + dirList[dirNum]
					+ "/" + dirList[dirNum] + fileSuffix
					+ "_a"
					+ ".edf"; /// generality
			if(QFile::exists(outPath)) { QFile::remove(outPath); }
			QFile::copy(helpString, outPath);

			setEdfFile(outPath);

			sliceMatiPieces();

			outStream << "slice sessions to pieces: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
		}
	}
}

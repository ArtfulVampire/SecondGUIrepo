#include <widgets/mainwindow.h>
#include "ui_mainwindow.h"

#include <functional>

#include <other/defs.h>
#include <other/consts.h>
#include <myLib/mati.h>
#include <myLib/output.h>
#include <myLib/dataHandlers.h>
#include <myLib/valar.h>

using namespace myOut;


void MainWindow::sliceAll() /////// aaaaaaaaaaaaaaaaaaaaaaaaaa//////////////////
{
	QTime myTime;
	myTime.start();

	switch(DEFS.getUser())
	{
	case username::ElenaC:
	{
		sliceElena();
		break;
	}
	case username::MichaelA:
	{
		/// remake using myLib::sliceData ??
		sliceOneByOne();
//		sliceOneByOneNew(); /// by number after 241/247 for early research before 2015
		break;
	}
	case username::MichaelB:
	{
#if 0
		/// old anagram tasks
		sliceBak(1, 60, "241");
		sliceBak(61, 120, "247");
		sliceBak(121, 180, "241");
		sliceBak(181, 240, "247");
#elif 1
		/// new "categorial"
		sliceBak2017();
#endif

		break;
	}
	case username::Mati:
	{
		/// almost equal time, should use sessionEdges
#if 1
		sliceMati();
		sliceMatiPieces();
#else
		sliceMatiSimple();
#endif
		break;
	}
	case username::IITP:
	{
		if(ui->windsButton->isChecked())
		{
			sliceJustWinds();
		}
		else
		{
			sliceWinds();
		}
		break;
	}
	case username::PolinaM:
	{
#if 0
		/// Polina
		else if(ui->pauseRadioButton->isChecked())
		{
			bool a = ui->typeCheckButton->isChecked();
			pausePieces(a);
		}
#endif
		break;
	}
	default:
	{
		break;
	}
	}
	ui->progressBar->setValue(0);
	outStream << "sliceAll: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void MainWindow::sliceJustWinds()
{
	const edfFile & fil = globalEdf;

	const int timeShift = ui->timeShiftSpinBox->value() * fil.getFreq();
	const int wndLength = ui->windowLengthSpinBox->value() * fil.getFreq();

	int windowCounter = 0;
	for(int i = 0; i < fil.getDataLen() - wndLength; i += timeShift)
	{
		QString helpString = DEFS.windsFromRealsDir()
							 + "/" + fil.getExpName()
							 + "." + rn(windowCounter++, 4);
		fil.saveSubsection(i, i + wndLength, helpString);

		ui->progressBar->setValue( i * 100. / fil.getDataLen() );

		qApp->processEvents();
		if(stopFlag)
		{
			stopFlag = false;
			break;
		}
	}
	ui->progressBar->setValue(0);
}

void MainWindow::sliceWinds()
{
	QTime myTime;
	myTime.start();

	const std::vector<int> staMarks{241, 247};
	const std::vector<int> endMarks{254};
	const std::vector<int> allMarks = smLib::unite<std::vector<int>>({staMarks, endMarks});

	const edfFile & fil = globalEdf;

#if 1
	const int windLen = std::round(fil.getFreq() * ui->windowLengthSpinBox->value());
	const int windStep = std::round(fil.getFreq() * ui->timeShiftSpinBox->value());
#else
	const int windLen = 1024;
	const int windStep = 1024;
#endif

	const auto & markers = fil.getMarkers();

	const int numSkipStartWinds = 2;				/// magic constant
#if 1
	const int restSkipTime = 0;	/// in time-bins, magic constant
#else
	const int restSkipTime = 1.5 * fil.getFreq();	/// in time-bins, magic constant
#endif

	int numReal = 0;

	std::vector<std::tuple<int, int, QString>> forSave; /// start, typ, filepath
	forSave.reserve(fil.getDataLen() / windStep);

	auto itSta = std::find_if(std::begin(markers), std::end(markers), [&allMarks](const auto & in)
	{ return myLib::contains(allMarks, in.second); });
	for(auto itMark = itSta + 1; itMark != std::end(markers); /*nothing*/++numReal)
	{
		auto itEnd = std::find_if(itMark, std::end(markers), [&allMarks](const auto & in)
		{ return myLib::contains(allMarks, in.second); });

		if(itEnd == std::end(markers)) { break; } /// end of file, cut later

		int typ = myLib::indexOfVal(allMarks, (*itSta).second);	/// new typ
		QString marker = nm((*itSta).second);					/// new marker

		int windowCounter = numSkipStartWinds;
		int skipStart = numSkipStartWinds * windStep;
		if((*itSta).second == 254) { skipStart = restSkipTime; } /// magic constant

		for(int startWind = (*itSta).first + skipStart;
			startWind < ((*itEnd).first - windLen);
			startWind += windStep)
		{
			QString helpString = DEFS.windsFromRealsDir()
								 + "/" + fil.getExpName()
								 + "." + rn(numReal, 4)
								 + "_" + marker
								 + "." + rn(windowCounter++, 2);
			forSave.push_back(std::tuple<int, int, QString>(startWind, typ, helpString));
		}

		itSta = itEnd;											/// new real/rest start
		itMark = itSta + 1;										/// new start to look for itEnd

		qApp->processEvents(); if(stopFlag) { stopFlag = false; break; }
	}

	/// cut last rest
	auto itLast = std::end(markers) - 1;
	for(; itLast != std::begin(markers); --itLast)
	{
		if((*itLast).second == 254) { break; } /// should be == itSta but not necessary
	}
	int windowCounter = 0;
	for(int startWind = (*itLast).first + restSkipTime;
		startWind < std::min(fil.getDataLen(),
							 static_cast<int>((*itLast).first + fil.getFreq() * 8)) - windLen;
		startWind += windStep)
	{
		QString helpString = DEFS.windsFromRealsDir()
							 + "/" + fil.getExpName()
							 + "." + rn(numReal, 4)
							 + "_" + "254"
							 + "." + rn(windowCounter++, 2);
		forSave.push_back(std::tuple<int, int, QString>(startWind, 2, helpString));
	}


	auto it = std::end(forSave) - 1;
	while(std::get<1>(*it) == 2) { --it; } /// don't save last rest

	/// save all or some last
	if(ui->succPrecleanCheckBox->isChecked())
	{
		const int succMax = suc::learnSetStay * 2; /// ~=120
		std::valarray<int> succCounter(3); succCounter = 0; /// 3 - numOfClasses

		/// save succMax each type
		for(; (succCounter != succMax).max() && it != std::begin(forSave); --it)
		{
			int currWindTyp = std::get<1>(*it);
			if(succCounter[currWindTyp] < succMax)
			{
				fil.saveSubsection(std::get<0>(*it),
								   std::get<0>(*it) + windLen,
								   std::get<2>(*it));
				++succCounter[currWindTyp];

				ui->progressBar->setValue( succCounter.sum() * 100. / (3. * succMax));
			}
		}
	}
	else
	{
		int c = 0;
		for(const auto & in : forSave)
		{
			fil.saveSubsection(std::get<0>(in),
							   std::get<0>(in) + windLen,
							   std::get<2>(in));

			ui->progressBar->setValue(c++ * 100. / forSave.size());
		}
	}
}

void MainWindow::sliceElena() /// slice only, eda in fileName
{
	/// CONSTS START
	const edfFile & fil = globalEdf;
	const int numOfTasks = 180;

	/// rest "task codes"
	/// 210 - closed start,	211 - closed end
	/// 212 - open start,	213 - open end
	const std::vector<std::vector<int>> eyesMarks{{210, 211}, {212, 213}};

	/// rest "operational codes", 214 - closed, 215 - open
	const std::vector<int> eyesCodes{214, 215};

	/// for windows operational codes are +100

	const double restWindow = 16.;	/// window length in seconds
	const double restShift = 16.;	/// time shift between windows in seconds

	const auto & marks = fil.getMarkers();

	/// make a set of all task numbers (to check later which ones were not processed)
	/// really should use smLib::range and std::set(iter1, iter2);
	std::set<int> allNumbers;
	for(int i = 1; i <= numOfTasks; ++i)
	{
		allNumbers.emplace(i);
	}
	/// CONSTS END

	auto edaBaseMean = [&fil](int startBin) -> double
	{
		int EDAnum = fil.findChannel("KGR");
		if(EDAnum == -1) { return 0.; }
		return smLib::mean(smLib::contSubsec(
							   fil.getData(EDAnum),
							   std::max(0., startBin - 2 * fil.getFreq()), /// -2 sec
							   startBin)
						   );
	};

	auto fileName = [&fil](double eda, int taskNumber, int taskMark, int operMark) -> QString
	{
		return fil.getDirPath()
				+ "/Reals"
				+ "/" + fil.getExpName()
				+ "_eda_" + nm(eda)
				+ "_n_" + nm(taskNumber)
				+ "_m_" + nm(taskMark)
				+ "_t_" + nm(operMark)
				+"_.edf";
	};

	/// slice rest backgrounds
	for(int typ = 0; typ < 2; ++typ)	/// 0 - closed, 1 - open
	{
		/// start iterator
		auto eyesSta = std::find_if(std::begin(marks),
									std::end(marks),
									[eyesMarks, typ](const auto & in)
		{ return in.second == eyesMarks[typ][0]; });

		/// finish iterator
		auto eyesFin = std::find_if(std::begin(marks),
									std::end(marks),
									[eyesMarks, typ](const auto & in)
		{ return in.second == eyesMarks[typ][1]; });

		/// if not found both markers - do nothing
		if(eyesSta == std::end(marks) || eyesFin == std::end(marks)) { continue; }

		/// save rest whole signal
		fil.saveSubsection((*eyesSta).first,
						   (*eyesFin).first,
						   fileName(edaBaseMean((*eyesSta).first),
									typ,
									eyesMarks[typ][0] + 100,
									eyesCodes[typ] + 100));


		/// rest windows
		int windCounter = 0;
		for(int i = (*eyesSta).first;
			i < (*eyesFin).first - restWindow * fil.getFreq();
			i += restShift * fil.getFreq(), ++windCounter)
		{
			/// save window signal
			fil.saveSubsection(i,
							   i + restWindow * fil.getFreq(),
							   fileName(edaBaseMean(i),
										windCounter,
										eyesMarks[typ][0],
										eyesCodes[typ]));
		}

	}

	int marker = -1;
	int number = -1;
	int start = -1;
	bool startFlag = false;

	/// (241-244) - instruction (task code)
	/// (1-numOfTasks) - number(start)
	/// (255) - optional click
	/// (245-254) - operational code
	/// 255 - ready for next task

	for(const auto & mark : marks)
	{
		if(!startFlag)
		{
			if(241 <= mark.second && mark.second <= 244)
			{
				marker = mark.second;
				startFlag = true;
			}
			else if(mark.second == 255)
			{
				/// 255 - do nothing
			}
			else
			{
				outStream << "sliceElena: startFlag == false, "
						  << "expecting marker 241-244 or 255, but have "
						  << mark.second
						  << ", time = " << mark.first / fil.getFreq() << " sec" << std::endl;
			}
		}
		else /// if(startFlag)
		{
			if(1 <= mark.second && mark.second <= 240) /// taskNumber - task started
			{
				number = mark.second;
				start = mark.first;
			}
			else if(245 <= mark.second && mark.second <= 254) /// operMark - task ended
			{
				if(start != -1) /// it was already set by task start
				{
					if(mark.first - start < 60 * fil.getFreq()) /// task < 1 minute
					{
						/// save the task signal
						fil.saveSubsection(start,
										   mark.first,
										   fileName(edaBaseMean(start),
													number,
													marker,
													mark.second));
						allNumbers.erase(number);
					}
					else /// if the task lasts more than 1 minute
					{
						outStream << "sliceElena: too long task, "
								  << "start time = " << start / fil.getFreq() << " sec, "
								  << "end time = " << mark.first / fil.getFreq() << " sec" << std::endl;
					}
				}
				else /// if start was not set, but we've found task end (operMark)
				{
					outStream << "sliceElena: task end (operMark) found but there was no start, "
							  << "mark = " << mark.second << ", "
							  << "its time = " << mark.first / fil.getFreq() << " sec" << std::endl;
				}
				startFlag = false;
				start = -1;
				marker = 0;
				ui->progressBar->setValue(mark.first * 100. / fil.getDataLen());
				qApp->processEvents();
				if(stopFlag)
				{
					stopFlag = false;
					return;
				}
			}
		}
	}
}


#if 0
/// old before edaBase sending
void MainWindow::sliceElena()
{
	/// CONSTS START
	const edfFile & fil = globalEdf;
	const int numOfTasks = 180;

	/// rest "task codes"
	/// 210 - closed start, 211 - closed end
	/// 212 - open start, 213 - open end
	const std::vector<std::vector<int>> eyesMarks{{210, 211}, {212, 213}};

	/// rest "operational codes", 214 - closed, 215 - open
	const std::vector<int> eyesCodes{214, 215};

	const double restWindow = 10.;	/// window length in seconds
	const double restShift = 7.;	/// time shift between windows in seconds
	const int numSmoothWind = 15;

	const int windFft = smLib::fftL(restWindow * fil.getFreq());
	DEFS.setFftLen(windFft);

	const auto eegChannels = fil.findChannels(coords::lbl19);
	const auto & marks = fil.getMarkers();

	/// for table 13-Mar-18
	const std::vector<std::pair<double, double>> integrLimits
	{
		/// 4-Hz wide
		{4, 8},
		{8, 12},
		{12, 16},
		{16, 20},
		{20, 24},

		/// 2-Hz wide
		{4, 6}, {6, 8},
		{8, 10}, {10, 12},

		/// 1-Hz wide
		{4, 5}, {5, 6}, {6, 7}, {7, 8},
		{8, 9}, {9, 10}, {10, 11}, {11, 12},
		{12, 13}, {13, 14}, {14, 15}, {15, 16},
		{16, 17}, {17, 18}, {18, 19}, {19, 20},
		{20, 21}, {21, 22}, {22, 23}, {23, 24},
	};
	const std::vector<std::vector<QString>> integrChans
	{
		{"F3", "F7"},
		{"F4", "F8"},
		{"T3", "T5"},
		{"C3"}, {"C4"},
		{"T4", "T6"},
		{"P3"}, {"P4"},
		{"O1"}, {"O2"},
	};

	std::vector<QString> tableCols
	{
		"taskNum",
		"taskMark",
		"operMark",
		"SGRval",
		"SGRlat",
		"RDfreq",
		"PPGampl",
		"PPGfreq",
		"reacTime",
	};

	/// alpha peak names
	for(int i : eegChannels)
	{
		QString lab = fil.getLabels(i);
		lab.remove("EEG ");
		lab.resize(lab.indexOf('-'));
		tableCols.push_back("alpha_" + lab);
	}

	/// add average spectra names
	for(const auto & chs : integrChans)
	{
		/// F3_F7_12-16
		QString chanStr{};
		for(const auto & ch : chs)
		{
			chanStr += ch + "_";
		}

		for(const auto & lim : integrLimits)
		{
			tableCols.push_back(chanStr + nm(lim.first) + "-" + nm(lim.second));
		}
	}

	/// new 6-Mar-18
	/// RD - rekoorseeya dyihaniya (don't know how it's called in English)
	/// FPG - FotoPletizmoGramma (really PPG - PhotoPlethismoGram)
	/// KGR - Kozhno Galvanicheskaya Reakciya (really SGR - Skin Galvanic Reaction
	/// or EDA - ElectroDermal Activity)
	const QString RDstring{"RD"};
	const QString PPGstring{"FPG"};
	const QString EDAstring{"KGR"};

	const int RDnum = fil.findChannel(RDstring);
	const int PPGnum = fil.findChannel(PPGstring);
	const int EDAnum = fil.findChannel(EDAstring);
	const bool writePoly = ui->elenaPolyCheckBox->isChecked();

	/// make a set of all task numbers (to check later which ones were not processed)
	/// really should use smLib::range and std::set(iter1, iter2);
	std::set<int> allNumbers;
	for(int i = 1; i <= numOfTasks; ++i)
	{
		allNumbers.emplace(i);
	}
	/// CONSTS END







	for(const auto & A :
		std::vector<std::pair<int, QString>>{
	{RDnum, RDstring},
	{EDAnum, EDAstring},
	{PPGnum, PPGstring},
})
	{
#if CPP17
		const auto & [a, b] = A;
		#else
		const auto & a = A.first;
		const auto & b = A.second;
#endif
		if(a == -1)
		{
			outStream << "sliceElena: " << b << " channel is absent" << std::endl;
		}
	}

	/// SHOULD SPECIFY PARAMETERS TO CAPTURE
	/// DON'T USE GENERAL "BY REFERENCE"
	auto saveSpecPoly = [&](int startBin,
						int finBin,
						const QString & pieceNumber,
						const QString & taskMark,
						const QString & operMark) -> std::valarray<double>
	{


		/// save spectre+polygraph
		const matrix subData = fil.getData().subCols(startBin, finBin);

		/// count spectra
		matrix spec = myLib::countSpectre(subData.subRows(eegChannels),
										  windFft,
										  numSmoothWind);

		/// check bad file
		if(spec.isEmpty())
		{
			return {};
		}

		/// write spectre to file
		QString savePath = fil.getDirPath()
						   + "/SpectraSmooth"
						   + "/" + fil.getExpName()
						   + "_n_" + pieceNumber
						   + "_m_" + taskMark
						   + "_t_" + operMark
				+ ".edf";
		myLib::writeMatrixFile(savePath,
							   spec.subCols(DEFS.left(),
											DEFS.right()));

		std::valarray<double> edaBase{};
		if(EDAnum != -1)
		{
			edaBase = smLib::contSubsec(fil.getData(EDAnum),
										std::max(0., startBin - 2 * fil.getFreq()), /// -2 sec
										startBin);
		}

		/// check bad file
		if(edaBase.size() < fil.getFreq())
		{
			return {};
		}

		/// breathing frequency
		double RDfr{};
		if(RDnum != -1) { RDfr = myLib::RDfreq(subData[RDnum], windFft); }

		/// amplitude and frequency of PPG
		double PPGampl{};
		double PPGfreq{};
		if(PPGnum != -1)
		{
			PPGampl = myLib::PPGrange(subData[PPGnum]);
			PPGfreq = myLib::RDfreq(subData[PPGnum], windFft);
		}

		/// Electrodermal activity
		std::pair<double, double> EDAval{};
		if(EDAnum != -1) { EDAval = myLib::EDAmax(subData[EDAnum], edaBase); }

		/// calculate integrated spectra (averaged over limits or not - look inside)
		matrix integratedSpectra = myLib::integrateSpectra(spec, fil.getFreq(), integrLimits);

		/// average over channels
		matrix integratedSpectraOut(integrChans.size(), integratedSpectra.cols());
		int counter = 0;
		for(const auto & chs : integrChans) /// each subset
		{
			std::valarray<double> res(0., integratedSpectra.cols());
			for(const auto & ch : chs) /// each chan from subset
			{
				res += integratedSpectra[fil.findChannel(ch)];
			}
			res /= chs.size();
			integratedSpectraOut[counter++] = res;
		}


		/// a row that will be pushed into table
		std::vector<double> forTable{};

		/// magic constant for veget and additional info
		forTable.reserve(30 + integrChans.size() * integrLimits.size());

		/// process pieceNumber (for rest windows)
		if(1)
		{
			bool ok = false;
			pieceNumber.toInt(&ok);

			if(ok) /// regular case - a task or a whole rest
			{
				forTable.push_back(pieceNumber.toInt());
			}
			else /// a rest window
			{
				/// in this case pieceNumber should end with a number like 0_137
				int a = pieceNumber.lastIndexOf(QRegExp(R"(\D)"));
				forTable.push_back(pieceNumber.mid(a + 1).toInt() + 1000);
			}
		}
		forTable.push_back(taskMark.toInt());
		forTable.push_back(operMark.toInt());

		/// veget data into table
		if(1)
		{
			forTable.push_back(EDAval.first);						/// value of max EDA
//			forTable.push_back(EDAval.second);						/// EDAmax latency in bins
			forTable.push_back(EDAval.second / fil.getFreq());		/// EDAmax latency in seconds
			forTable.push_back(RDfr);								/// breathing frequency
			forTable.push_back(PPGampl);							/// PPG pulse wave max amplitude
			forTable.push_back(PPGfreq);							/// PPG frequency
			forTable.push_back(subData.cols() / fil.getFreq());		/// peice length in seconds
			for(int i : eegChannels)
			{
				/// could use spec here (already calculated subData spectra)
				forTable.push_back(myLib::alphaPeakFreq(
									   myLib::smoothSpectre(myLib::spectreRtoR(subData[i])),
									   subData.cols(),
									   fil.getFreq(),
									   8,
									   12));
			}
		}


		/// resize to final size (we've already reserved the memory, so no realloc should occur)
		const int prevSize = forTable.size();
		forTable.resize(prevSize + integrChans.size() * integrLimits.size());

		/// "push_back" spectra
		const std::vector<double> specRow = integratedSpectraOut.toVectorByRows();
		std::copy(std::begin(specRow),
				  std::end(specRow),
				  std::begin(forTable) + prevSize);

		return smLib::vecToValar(forTable);
	};
	/// saveSpecPoly finish


	/// resulting table
	matrix table{};

	/// slice rest backgrounds
	for(int typ = 0; typ < 2; ++typ)	/// 0 - closed, 1 - open
	{
		/// start iterator
		auto eyesSta = std::find_if(std::begin(marks),
									std::end(marks),
									[eyesMarks, typ](const auto & in)
		{ return in.second == eyesMarks[typ][0]; });

		/// finish iterator
		auto eyesFin = std::find_if(std::begin(marks),
									std::end(marks),
									[eyesMarks, typ](const auto & in)
		{ return in.second == eyesMarks[typ][1]; });

		/// if not found both markers - do nothing
		if(eyesSta == std::end(marks) || eyesFin == std::end(marks)) { continue; }

		QString restFileName = fil.getDirPath()
							   + "/Reals"
							   + "/" + fil.getExpName()
							   + "_n_0_" + nm(1500 + typ * 100)		/// taskNumber (1500 - closed, 1600 - open)
							   + "_m_" + nm(eyesMarks[typ][0])		/// taskMark
								+ "_t_" + nm(eyesCodes[typ] + 100)	/// operMark
				+ ".edf";

		/// save rests signal
		fil.saveSubsection((*eyesSta).first,
						   (*eyesFin).first,
						   restFileName);

		/// save values for whole rest piece
		if(writePoly)
		{
			table.push_back(
						saveSpecPoly((*eyesSta).first,
									 (*eyesFin).first,
									 nm(1500 + typ * 100),		/// taskNumber (1500 - closed, 1600 - open)
									 nm(eyesMarks[typ][0]),		/// taskMark
									nm(eyesCodes[typ] + 100)	/// operMark
						)
					);
		}

		/// save values for rest windows
		int windCounter = 0;
		for(int i = (*eyesSta).first;
			i < (*eyesFin).first - restWindow * fil.getFreq();
			i += restShift * fil.getFreq(), ++windCounter)
		{
			QString helpString = fil.getDirPath()
								 + "/Reals"
								 + "/" + fil.getExpName()
								 + "_n_0_" + nm(windCounter)		/// _0_ added for rest
								 + "_m_" + nm(eyesMarks[typ][0])	/// taskMark
					+ "_t_" + nm(eyesCodes[typ]);					/// operMark

			/// save window signal
			fil.saveSubsection(i,
							   i + restWindow * fil.getFreq(),
							   helpString);
			if(writePoly)
			{
				table.push_back(
							saveSpecPoly(i,
										 i + restWindow * fil.getFreq(),
										 QString("0_" + nm(windCounter)),	/// taskNumber
										 nm(eyesMarks[typ][0]),				/// taskMark
										nm(eyesCodes[typ])					/// operMark
							)
						);
			}
		}

	}

	int marker = -1;
	int number = -1;
	int start = -1;
	bool startFlag = false;

	/// (241-244) - instruction (task code)
	/// (1-numOfTasks) - number(start)
	/// (255) - optional click
	/// (245-254) - operational code
	/// 255 - ready for next task

	for(const auto & mark : marks)
	{
		if(!startFlag)
		{
			if(241 <= mark.second && mark.second <= 244)
			{
				marker = mark.second;
				startFlag = true;
			}
			else if(mark.second == 255)
			{
				/// 255 - do nothing
			}
			else
			{
				outStream << "sliceElena: startFlag == false, "
						  << "expecting marker 241-244 or 255, but have "
						  << mark.second
						  << ", time = " << mark.first / fil.getFreq() << " sec" << std::endl;
			}
		}
		else /// if(startFlag)
		{
			if(1 <= mark.second && mark.second <= 240) /// taskNumber - task started
			{
				number = mark.second;
				start = mark.first;
			}
			else if(245 <= mark.second && mark.second <= 254) /// operMark - task ended
			{
				QString helpString = DEFS.dirPath()
									 + "/Reals"
									 + "/" + fil.getExpName()
									 + "_n_" + nm(number)				/// taskNumber
									 + "_m_" + nm(marker)				/// taskMark
									 + "_t_" + nm(mark.second);			/// operMark

				if(start != -1) /// it was already set by task start
				{
					if(mark.first - start < 60 * fil.getFreq()) /// task < 1 minute
					{
						/// save the task signal
						fil.saveSubsection(start,
										   mark.first,
										   helpString);

						/// new 6-Mar-18
						if(writePoly)
						{
							table.push_back(
										saveSpecPoly(start,
													 mark.first,
													 nm(number),
													 nm(marker),
													 nm(mark.second))
										);
						}

						allNumbers.erase(number);
					}
					else /// if the task lasts more than 1 minute
					{
						outStream << "sliceElena: too long task, "
								  << "start time = " << start / fil.getFreq() << " sec, "
								  << "end time = " << mark.first / fil.getFreq() << " sec" << std::endl;
					}
				}
				else /// if start was not set, but we've found task end (operMark)
				{
					outStream << "sliceElena: task end (operMark) found but there was no start, "
							  << "mark = " << mark.second << ", "
							  << "its time = " << mark.first / fil.getFreq() << " sec" << std::endl;
				}

				startFlag = false;
				start = -1;
				marker = 0;

				ui->progressBar->setValue(mark.first * 100. / fil.getDataLen());
				qApp->processEvents();
				if(stopFlag)
				{
					stopFlag = false;
					return;
				}

			}
		}
	}

	/// table into file
	std::ofstream tableStream((fil.getDirPath() + "/" + "table.txt").toStdString());
	tableStream << tableCols << "\r\n";
	tableStream.precision(4);

	if(01) /// sort the rows of the table by pieceNumber
	{
		std::sort(std::begin(table), std::end(table),
				  [](const auto & a1, const auto a2)
		{
			return a1[0] < a2[0];
		});
	}

	/// remove empty rows
	std::vector<int> inds{};
	for(int i = 0; i < table.rows(); ++i)
	{
		if(table[i].size() == 0) { inds.push_back(i); }
	}
	table.eraseRows(inds);

	tableStream << table;
	tableStream.close();

	/// cout unprocessed tasks
	if(!allNumbers.empty())
	{
		outStream << "sliceElena: not detected reals:" << std::endl;
		for(auto each : allNumbers)
		{
			outStream << each << " ";
		}
		outStream << std::endl;
	}
}
#endif

/// beginning - from mark1 to mark 2, end 250 Marker - included in filename
void MainWindow::sliceBak(int marker1, int marker2, const QString & marker)
{
    /// for Baklushev
    QString helpString;

    int number = 0;
    int k;
	int j = 0;                                     /// flag of marker1 read
	int h = 0;                                     /// flag of marker2 read
    const edfFile & fil = globalEdf;
	const auto & markerChan = fil.getMarkArr();

    for(int i = 0; i < fil.getDataLen(); ++i)
    {
		if(markerChan[i] >= marker1
		   && markerChan[i] <= marker2
		   && h== 0)
        {
            j = i;
            h = 1;
            continue;
        }
		if(markerChan[i] == 250.)
        {
            k = i;
			if(h == 1) { ++h; }
        }
        if(h == 2)
        {
            ++number;
			helpString = DEFS.dirPath()
					+ "/Reals"
					+ "/" + fil.getExpName()
                    + "_" + marker
					+ "." + rn(number, 4);

            /// to test?
			fil.saveSubsection(j, k, helpString);

            i += 17;
            h = 0;
        }
    }
}

void MainWindow::sliceBak2017()
{
	QString marker = "000";

	const edfFile & fil = globalEdf;

	int start = -1;
	int type = -1; /// 0 - k, 1 - l, 2 - n
	int number = -1;

	// 242, 241, (1-150), (251||252||253) ]
	/// remake fil.getMarkers()
	for(const auto & marks : fil.getMarkers())
	{
		if(marks.second <= 150) //all not interesting markers
		{
			/// start stimulus
			type = int(marks.second - 1) / 50;
			number = marks.second;
			start = marks.first;
			continue;
		}
		else if(marks.second == 251 || marks.second == 252 || marks.second == 253)
		{
			/// answer, finish stim
			int ans = marks.second;
			QString typStr{};
			QString correct = "wrong";
			switch(type)
			{
			case 0: { typStr = "k"; if(ans == 251) { correct = "right"; } break; }
			case 1: { typStr = "l"; if(ans == 253) { correct = "right"; } break; }
			case 2: { typStr = "n"; if(ans == 252) { correct = "right"; } break; }
			default: { typStr = ""; break; }
			}
			QString helpString = DEFS.getDirPath()
								 + "/Reals/" + fil.getExpName()
								 + "_" + nm(number)
								 + "_" + typStr
								 + "_" + nm(ans)
								 + "_" + correct;

			fil.saveSubsection(start, marks.first, helpString);
		}
		ui->progressBar->setValue(marks.first * 100. / fil.getDataLen());

		qApp->processEvents();
		if(stopFlag)
		{
			stopFlag = 0;
			break;
		}
	}
}


void MainWindow::sliceOneByOne()
{

    int number = 0;
    QString marker = "000";
	int start = 0;

	const edfFile & fil = globalEdf;

	/// 200, (241||247, (1), 254, 255)
	for(const std::pair<int, int> & in : fil.getMarkers())
    {
		if(in.second != 241
		   && in.second != 247
		   && in.second != 254)
		{
			continue;
		}

		const int finish = in.first;

		QString helpString = DEFS.dirPath()
							 + "/Reals"
							 + "/" + fil.getExpName()
							 + "." + rn(number++, 4);

		if(finish > start)
		{
			if(finish - start <= DEFS.getFreq() * 62) /// magic const generality limit
			{
				helpString += "_" + marker;
				fil.saveSubsection(start,
								   finish,
								   helpString);
			}
			else /// pause rest
			{
				if(def::writeLongStartEnd)
				{
					helpString += "_000";
					fil.saveSubsection(start,
									   finish,
									   helpString);
				}
				else
				{
#if 0
					/// why do I need this?
					helpString += "_" + marker;
					matrix tempData(fil.getNs(), 100, 0.);
					tempData[fil.getMarkChan()][0] = markChanArr[start];
					myLib::writePlainData(helpString, tempData);
#endif
				}
			}
		}
		ui->progressBar->setValue(in.first * 100. / fil.getDataLen());

		qApp->processEvents();
		if(stopFlag)
		{
			stopFlag = false;
			return;
		}
		marker = nm(in.second);
		start = finish;
    }


    /// write final
    {
		QString helpString = DEFS.dirPath()
							 + "/Reals"
							 + "/" + fil.getExpName()
							 + "." + rn(number++, 4);
		if(fil.getDataLen() - start < 40 * DEFS.getFreq()) /// if last realisation or interstimulus
		{
			helpString += "_" + marker;
			fil.saveSubsection(start,
							   fil.getDataLen(),
							   helpString);
		}
		else /// just last big rest with eyes closed/open
		{
			if(def::writeLongStartEnd)
			{
				helpString += "_000";
				fil.saveSubsection(start,
								   fil.getDataLen(),
								   helpString);
			}
			else /// not to loose the last marker
			{
#if 0
				/// ???
				helpString += "_" + marker;
				matrix tempData(fil.getNs(), 100, 0.);
				tempData[fil.getMarkChan()][0] = markChanArr[start];
				myLib::writePlainData(helpString, tempData);
#endif
			}
		}
	}
}

/// deprecated numChanWrite - always with markers
void MainWindow::sliceOneByOneNew()
{
    int number = 0;
    int j = 0;
	int h = 0; /// h == 0 - 241, h == 1 - 247
    QString marker = "000";

    const edfFile & fil = globalEdf;

	const std::valarray<double> & markChanArr = fil.getMarkArr();

    /// 200, 255, (241||247, num, 254, 255)
    /// with feedback 200 (241||247, num, 231||237, (234), 254, 255)
	/// remake with markers pairs
    for(int i = 0; i < fil.getDataLen(); ++i)
    {
        if(markChanArr[i] == 0.)
        {
            continue;
        }
		if((markChanArr[i] > 200. && markChanArr[i] < 241.) ||
		   markChanArr[i] == 255. ||
		   markChanArr[i] == 250. ||
		   markChanArr[i] == 251.) /// all not interesting markers
        {
            continue;
		}
		if(markChanArr[i] == 241. || markChanArr[i] == 247.)
		{
			marker = "254";
			if(markChanArr[i] == 241.)			{ h = 0; }
			else if (markChanArr[i] == 247.)	{ h = 1; }
			continue; /// wait for num marker
		}
		/// marker can be num <= 200, ==254, smth else
		{
			if(marker.isEmpty())
			{
				marker = "sht";
			}

			QString helpString = DEFS.dirPath()
								 + "/Reals"
								 + "/" + fil.getExpName()
								 + "." + rn(number++, 4);
			if(i > j)
			{
				/// new 26-Sep-2018, write everything
				helpString += "_" + marker;
				fil.saveSubsection(j, i, helpString);
#if 0
				/// old 26-Sep-2018
				if(i - j <= DEFS.getFreq() * 60) /// const generality limit
				{
					helpString += "_" + marker;
					fil.saveSubsection(j, i, helpString);
				}
				else /// rest in the beginning, long pauses
				{
					if(def::writeLongStartEnd)
					{
						helpString += "_000";
						fil.saveSubsection(j, i, helpString);
					}
					else
					{
						helpString += "_" + marker;
						matrix tempData(fil.getNs(), 100, 0.);
						tempData[fil.getMarkChan()][0] = markChanArr[j];

						fil.saveSubsection(j, i, helpString);

						/// remake using edfs
//						myLib::writePlainData(helpString, tempData);
					}
				}
#endif
            }
            ui->progressBar->setValue(i * 100. / fil.getDataLen());

            qApp->processEvents();
            if(stopFlag)
            {
                stopFlag = false;
                break;
            }

            marker.clear();
			if(h == 0)			{ marker = "241"; }
			else if(h == 1)		{ marker = "247"; }
            h = -1;
            j = i;
            continue;
        }
    }
    /// write final
    {
		QString helpString = DEFS.dirPath()
							 + "/Reals"
							 + "/" + fil.getExpName()
							 + "." + rn(number++, 4);
		/// new 26-Sep-2018, write everything
		helpString += "_" + marker;
		fil.saveSubsection(j, fil.getDataLen(), helpString);
#if 0
		/// old 26-Sep-2018
		if(fil.getDataLen() - j < 40 * DEFS.getFreq()) /// if last realisation or interstimulus
		{
			helpString += "_" + marker;
			fil.saveSubsection(j, fil.getDataLen(), helpString);
		}
		else /// just last big rest with eyes closed/open
		{
			if(def::writeLongStartEnd)
			{
				helpString += "_000";
				fil.saveSubsection(j, fil.getDataLen(), helpString);
			}
			else /// not to loose the last marker
			{
				helpString += "_" + marker;
				matrix tempData(fil.getNs(), 100, 0.);
				tempData[fil.getMarkChan()][0] = markChanArr[j];

				/// remake using edfs
//				myLib::writePlainData(helpString, tempData);
			}
		}
#endif

    }

}

void MainWindow::sliceMatiSimple()
{
#if 0
    QTime myTime;
    myTime.start();

    QString helpString;
    int start = 0;
    int end = -1;
    std::vector<bool> markers;
    bool state[3];
    QString fileMark;
	int session[4]; /// generality
    int type = 3;

    for(int i = 0; i < 4; ++i)
    {
        session[i] = 0;
    }

    const edfFile & fil = globalEdf;
    double currMarker;
    int number;
	double piece = ui->matiPieceLengthSpinBox->value() * DEFS.getFreq();

    for(int i = 0; i < fil.getDataLen(); ++i)
    {
		currMarker = fil.getMarkArr(i);
        if(currMarker == 0)
        {
            continue;
        }
        else
        {
			markers = myLib::matiCountByte(currMarker);
			/// decide whether the marker is interesting: 15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0
            for(int i = 0; i < 3; ++i)
            {
				state[i] = markers[i + 8]; /// always elder byte is for count adn type of the session
            }

            if(!(state[0] || state[1] || state[2])) continue; /// if all are zeros

			if(state[2] == 1) /// the end of a session
            {
				if(state[1] == 0 && state[0] == 1) /// end of a counting session
                {
                    type = 0;
					fileMark = "241"; /// count
                }
				if(state[1] == 1 && state[0] == 0) /// end of a tracking session
                {
                    type = 1;
					fileMark = "247"; /// follow
                }
				if(state[1] == 1 && state[0] == 1) /// end of a composed session
                {
                    type = 2;
					fileMark = "244"; /// composed
                }
            }
			else /// if the start of a session
            {
                type = 3;
				fileMark = "254"; /// rest. start of the session is sliced too
            }
            end = i + 1; /// end marker should be included
        }

		/// save session edf
        if(end > start)
        {
            if(state[2]) /// if not rest
            {
				number = static_cast<int>(std::ceil((end-start) / static_cast<double>(piece)));

                for(int j = 0; j < number; ++j) /// num of pieces
                {
					helpString = (DEFS.dirPath()
														  + "/Reals"
														  + "/" + globalEdf.getExpName()
														  + "_" + nm(type)
														  + "_" + nm(session[type])
														  + "_" + rn(j, 2)
                                                          + '.' + fileMark);

					int NumOfSlices = std::min(end - start - j * piece, piece);
					/// PEWPEWPEWPEWPEWPEWPEW, use edf
//					myLib::writePlainData(helpString,
					fil.getData().subCols(NumOfSlices, start + j * piece));
                }
                fileMark.clear();
                ++session[type];
            }

            start = end - 1; /// = i /// start marker should be included
            end = -1;
        }
        ui->progressBar->setValue(end * 100. / fil.getDataLen());

        qApp->processEvents();
        if (stopFlag == 1)
        {
            break;
        }
    }
    ui->progressBar->setValue(0);

	outStream << "sliceMatiSimple: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
    stopFlag = false;
#endif
}

void MainWindow::sliceMati()
{
    QTime myTime;
    myTime.start();

    int start = 0;
	int end = -1;
	QString fileMark{};
	std::array<int, 4> session{}; /// generality
    int type = 3;

	for(int & in : session)
    {
		in = 0;
    }

    const edfFile & fil = globalEdf;
    double currMarker;


    for(int i = 0; i < fil.getDataLen(); ++i)
    {
		currMarker = fil.getMarkArr(i);
		if(currMarker == 0.)
        {
            continue;
        }
		/// else
        {
			std::vector<bool> markers = myLib::matiCountByte(currMarker);
			/// decide whether the marker is interesting: 15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0

			std::array<bool, 3> state{};
			for(int i = 0; i < 3; ++i)
            {
				state[i] = markers[i + 8]; /// always elder byte is for count adn type of the session
            }

			if(!(state[0] || state[1] || state[2])) { continue; } /// if all are zeros

			if(state[2] == 1) /// the end of a session
            {
				if(state[1] == 0 && state[0] == 1) /// end of a counting session
                {
                    type = 0;
					fileMark = "241"; /// count
                }
				if(state[1] == 1 && state[0] == 0) /// end of a tracking session
                {
                    type = 1;
					fileMark = "247"; /// follow
                }
				if(state[1] == 1 && state[0] == 1) /// end of a composed session
                {
                    type = 2;
					fileMark = "244"; /// composed
                }
            }
			else /// if the start of a session
            {
                type = 3;
				fileMark = "254"; /// rest. start of the session is sliced too
            }
            end = i + 1; /// end marker should be included
        }

		/// save session edf
        if(end > start)
        {
            if(type != 3) /// dont write rests
            {
				QString helpString = DEFS.dirPath()
									 + "/auxEdfs"
									 + "/" + fil.getExpName()
									 + "_" + nm(type)
									 + "_" + nm(session[type])
									 + ".edf";

                fil.saveSubsection(start,
                                   end,
                                   helpString);
            }

			start = end - 1; /// start marker should be included
            end = -1;
            ++session[type];
        }
        ui->progressBar->setValue(end * 100. / fil.getDataLen());

        qApp->processEvents();
        if (stopFlag == 1)
        {
            break;
        }
    }
    ui->progressBar->setValue(0);
	outStream << "sliceMati: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
    stopFlag = false;
}

/// add markChan alias
void MainWindow::sliceMatiPieces()
{
    QTime myTime;
    myTime.start();

    QString helpString;
    int dataLen;
    QString fileMark;
    QString folder;
    int pieceNum;
    int currStart;
    int currEnd;
    const double pieceLength = ui->matiPieceLengthSpinBox->value();
	const bool adjustPieces = ui->matiAdjustPiecesCheckBox->isChecked();

    DEFS.setDir(globalEdf.getDirPath());
    edfFile fil;

    if(pieceLength <= 4.)
    {
		folder = "winds";
    }
    else
    {
		folder = "Reals";
    }

	for(int type = 0; type < 3; ++type) /// magic const
    {
		for(int session = 0; session < 15; ++session) /// magic const
        {
            /// edf session path
			helpString = DEFS.dirPath()
						 + "/auxEdfs"
						 + "/" + globalEdf.getExpName()
						 + "_" + nm(type)
						 + "_" + nm(session)
						 + ".edf";

            if(QFile::exists(helpString))
            {
                fil.readEdfFile(helpString);
                dataLen = fil.getDataLen();
                pieceNum = 0;
                currStart = 0;
                currEnd = -1; /// [currStart, currEnd)

				switch(type)
				{
				case 0:		{ fileMark = "241"; break; }
				case 1:		{ fileMark = "247"; break; }
				case 2:		{ fileMark = "244"; break; }
				default:	{ fileMark = "254"; break; }
				}

                if(adjustPieces)
                {
                    do
                    {
						currEnd = std::min(static_cast<int>(currStart
															+ pieceLength * DEFS.getFreq()),
										   dataLen);

                        if(type == 0 || type == 2)
                        {
                            /// std::search
							while ( ! (myLib::matiCountBit(fil.getMarkArr(currEnd - 1), 14) ||
									   myLib::matiCountBit(fil.getMarkArr(currEnd - 1), 10)) ) /// while not (given answer OR session End)
                            {
                                --currEnd;
                            }
                        }
                        else if(currEnd == dataLen) /// should do nothing due to edfFile::cutZerosAtEnd
                        {
							while ( ! (myLib::matiCountBit(fil.getMarkArr(currEnd - 1), 10)) ) /// while not session end
                            {
                                --currEnd;
                            }
                        }

                        if(currEnd <= currStart) /// no count answers during pieceLength seconds
                        {
							currEnd = std::min(static_cast<int>(currStart
																+ pieceLength * DEFS.getFreq()),
											   dataLen);
                        }

                        /// type and session already in the fil.ExpName

						helpString = DEFS.dirPath()
									 + "/" + folder
									 + "/" + fil.getExpName()
									 + "_" + rn(pieceNum, 2)
									 + '_' + fileMark;


						fil.saveSubsection(currStart, currEnd, helpString);
                        ++pieceNum;
                        currStart = currEnd;

					} while (!myLib::matiCountBit(fil.getMarkArr(currEnd - 1), 10) );
                }
                else
                {
                    while(currStart < dataLen)
                    {
						currEnd = std::min(static_cast<int>(currStart
															+ pieceLength * DEFS.getFreq()),
										   dataLen);
						helpString = (DEFS.dirPath()
															  + "/" + folder
															  + "/" + fil.getExpName()
															  + "_" + rn(pieceNum, 2)
                                                              + '_' + fileMark);
						fil.saveSubsection(currStart, currEnd, helpString);
                        ++pieceNum;
                        currStart = currEnd;
                    }
                }
            }
        }
    }
	outStream << "sliceMatiPieces: time = " << myTime.elapsed() / 1000. << " sec" << std::endl;
}

void MainWindow::eegVegetSlot()
{
	QStringList files = QFileDialog::getOpenFileNames(this,
													  tr("Choose veget and eeg files"),
													  DEFS.getDirPath(),
													  tr("EDF files (*.EDF *.edf)"));
	if(files.size() != 2) { return; }
	QString veget{};
	QString eeg{};
	for(int i = 0; i < 2; ++i)
	{
		if(files[i].contains("_veget"))
		{
			veget = files[i];
			eeg = files[(i + 1) % 2];
		}
	}
	if(veget.isEmpty())
	{
		outStream << "eegVegetSlot: no _veget file" << std::endl;
		return;
	}
	edfFile fil1(eeg);
	fil1 = fil1.vertcatFile(veget);
	for(int i = 0; i < fil1.getNs() - 1; ++i)
	{
		if(fil1.getLabels(i).contains("Markers")
		   && fil1.getLabels().back().contains("Markers"))
		{
			fil1.removeChannel(i);
			break;
		}
	}
	QString outPath = veget;
	outPath.remove("_veget");
	outPath.replace(".edf", "_unite.edf");
	fil1.writeEdfFile(outPath);
}

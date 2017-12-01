#include <other/autos.h>

#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>

using namespace myOut;

namespace autos
{
QPixmap IITPdrawCoh(const std::valarray<std::complex<double>> & inData,
					double minFreq,
					double maxFreq,
					double srate,
					double fftLen,
					double maxVal,
					double confidence)
{

	const double graphHeight = 0.95;
	const double leftGap = 0.05;
	const double rightGap = 0.01;

	const double spStep = srate / fftLen;
	const int beg = std::floor(minFreq / spStep);
	const int en  = std::floor(maxFreq / spStep);

	std::valarray<double> drawData(en - beg);
	for(int i = 0; i < drawData.size(); ++i)
	{
		drawData[i] = std::abs(inData[beg + i]);
	}

	std::cout << drawData.size() << std::endl;

	if( maxVal == 0.) { maxVal = drawData.max(); }

	maxVal = std::ceil(maxVal * 100.) / 100.; /// = n * 0.01



	QPixmap pic(800, 600);
	pic.fill();
	QPainter pnt;
	pnt.begin(&pic);
	for(int i = 0; i < drawData.size() - 1; ++i)
	{
//		std::cout << i << "\t" << arr[i] << std::endl;
		pnt.drawLine( pic.width() * (leftGap + i / double(drawData.size()) * (1. - (leftGap +  rightGap))),
					  pic.height() * (1. - drawData[i] / maxVal) * graphHeight,
					  pic.width() * (leftGap + (i + 1) / double(drawData.size()) * (1. - (leftGap +  rightGap))),
					  pic.height() * (1. - drawData[i + 1] / maxVal) * graphHeight);
	}
	const QFont font = QFont("Helvetica", 8);
	pnt.setFont(font);

	/// Y values
	const double valStep = std::pow(10., std::floor(log10(maxVal)));
	for(double val = 0.; val < maxVal; val += valStep)
	{
		const double y = pic.height() * (1. - val / maxVal) * graphHeight;
		pnt.drawLine( 0,
					  y,
					  10,
					  y);
		pnt.drawText(10 + 2,
					 y + QFontMetrics(font).height() / 2,
					 QString::number(val));
	}

	/// draw Hz
	for(int i = beg; i < en; ++i)
	{
		const double x = pic.width() * (leftGap + (i - beg) / double(en - beg) * (1. - (leftGap +  rightGap)));
		pnt.drawLine( x,
					  pic.height() * graphHeight,
					  x,
					  pic.height() * (graphHeight + 0.03));
		pnt.drawText(x - 4,
					 pic.height() * (graphHeight + 0.03) + QFontMetrics(font).height() / 2,
					 QString::number(i));
	}

	/// confidence line
	pnt.drawLine(0,
				 pic.height() * (1. - confidence / maxVal) * graphHeight,
				 pic.width(),
				 pic.height() * (1. - confidence / maxVal) * graphHeight);

	/// zero line
	pnt.drawLine(0,
				 pic.height() * graphHeight,
				 pic.width(),
				 pic.height() * graphHeight);

	/// left line
	pnt.drawLine(pic.width() * leftGap,
				 0,
				 pic.width() * leftGap,
				 pic.height() * graphHeight);

	pnt.end();
	return pic;
}

void IITPrename(const QString & guyName)
{
	const QString pth = def::iitpFolder + "/" + guyName + "/";
	if(!QFile::exists(pth + "rename.txt")) return;

	const QString postfix = "_rn";

	std::ifstream inStr;
	inStr.open((pth + "rename.txt").toStdString());
	while(1)
	{
		int oldNum;
		int newNum;
		inStr >> oldNum >> newNum;
		if(!inStr.eof())
		{
			for(QString ending : {
				"_eeg.edf",
				"_emg.edf",
				".dat"
		})
			{
				QString oldName = guyName + "_" + rn(oldNum, 2) + ending;
				QString newName = guyName + "_" + rn(newNum, 2) + postfix + ending;
				if(QFile::exists(pth + oldName))
				{
					QFile::rename(pth + oldName,
								  pth + newName);
				}
			}
		}
		else
		{
			break;
		}
	}
	inStr.close();

	for(QString nam : QDir(pth).entryList({"*" + postfix + "*"}))
	{
		QString newName = nam;
		newName.remove(postfix);
		QFile::rename(pth + nam,
					  pth + newName);
	}
}

/// *.dat headers to dats.txt
void IITPdat(const QString & guyName)
{
	def::ntFlag = true;
	QStringList files = QDir(def::iitpFolder + "/" + guyName).entryList({"*.dat"},
																		QDir::Files,
																		QDir::Name);
	std::ofstream outStr;
	outStr.open((def::iitpFolder + "/" + guyName + "/" + guyName + "_dats.txt").toStdString());

	for(const QString & fil : files)
	{
		const QString filePath = def::iitpFolder + "/" +
								 guyName + "/" +
								 fil;
		int num = fil.mid(fil.indexOf('_') + 1, 2).toInt();
//		std::cout << num << std::endl;

		QFile f(filePath);
		f.open(QIODevice::ReadOnly);
		QTextStream ts(&f);
		ts.readLine();
		ts.setCodec("windows-1251");
		outStr << num << "\t" << ts.readLine() << "\r\n";
		f.close();
	}
	outStr.close();
}

/// filter gonios
void IITPfilterGonios(const QString & guyName,
					  const std::vector<QString> & joints)
{
	QString postfix = iitp::getPostfix(QDir(def::iitpFolder + "/" + guyName).entryList({"*.edf"})[0]);
	for(int fileNum : iitp::fileNums)
	{
		const QString ExpNamePre = def::iitpFolder + "/" +
								   guyName + "/" +
								   guyName + "_" + rn(fileNum, 2);
		QString filePath;
		edfFile fil;

		/// filter goniogramms
		filePath = ExpNamePre + postfix + ".edf";
		if(!QFile::exists(filePath)) continue;
		fil.readEdfFile(filePath);

		std::vector<uint> chanList;
		for(int i = 0; i < fil.getNs(); ++i)
		{
			for(auto joint : joints)
			{
				if(fil.getLabels()[i].contains(joint, Qt::CaseInsensitive))
				{
					chanList.push_back(i);
					break;
				}
			}
		}

		filePath = ExpNamePre + postfix + "_gon" + ".edf";
		fil.refilter(0.1, 3, false, chanList).writeEdfFile(filePath);
	}
}


void IITPtestCoh2(const QString & guyName)
{
	/// test Cz-Tb_l/Ta_r coherency during static stress

	iitp::iitpData dt;
	const QString direct = def::iitpSyncFolder + "/" + guyName + "/";
//	const QString direct = def::iitpFolder + "/" + guyName + "/";

//	QString postfix = iitp::getPostfix(QDir(direct).entryList({"*.edf"})[0]);
//	QString postfix = "_dsp_up";
//	QString postfix = "_dsp_down";
//	QString postfix = "_fft_up";
//	QString postfix = "_fft_down";

	const double lowFreq = 8.;
	const double highFreq = 45.;

	/// Test guy
	std::vector<int> nums{1,2,3,4};
	std::vector<QString> names{"1_e", "2_e", "3_e", "4_e"}; /// enveloped
//	std::vector<QString> names{"1", "2", "3", "4"};

	/// take Ta_l - file 24 and Ta_r - file 25
//	std::vector<int> nums{24, 25};
//	std::vector<QString> names{"coh_l", "coh_r"};

	QString postfix = "_sum_new_abs";
	{
		std::vector<double> outCoh;
		auto filePath = [=](int i) -> QString
		{
			return direct + guyName + "_" + rn(i, 2) + postfix + ".edf";
		};

		for(int i = 0; i < nums.size(); ++i)
		{
			if(!QFile::exists(filePath(nums[i])))
			{
				std::cout << "IITPtestCoh2: file doesn't exist: "
						  << filePath(nums[i]) << std::endl;
				continue;
			}

			outCoh.clear();
			dt.readEdfFile(filePath(nums[i]));
			dt.cutPieces(1.024);
			for(double freq = lowFreq; freq <= highFreq; freq += 1.)
			{
				outCoh.push_back(std::abs(dt.coherency(dt.findChannel("Cz"),
													   dt.findChannel("Ta_r"),
													   freq)));
			}
			myLib::drw::drawOneArray(myLib::drw::drawOneTemplate(10, true, lowFreq, highFreq),
									 smLib::vecToValar(outCoh)).save(
						"/media/Files/Data/iitp/" + names[i] + postfix + ".jpg", 0, 100);
//			std::cout << postfix << std::endl;
			std::cout << names[i] << " coh max = " << *std::max_element(std::begin(outCoh), std::end(outCoh)) << std::endl;
		}
	}

}

void IITPtestCoh(const QString & guyName)
{
	/// test coherency in all files
	iitp::iitpData dt;
//	const QString direct = def::iitpFolder + "/" + guyName + "/";
	const QString direct = def::iitpSyncFolder + "/" + guyName + "/";
	QString postfix = iitp::getPostfix(QDir(direct).entryList({"*.edf"})[0]);

	auto filePath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + postfix + ".edf";
	};

	auto resPath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + "_res.txt";
	};

	/// chan1, chan2, freq, len
	std::vector<std::vector<std::vector<std::valarray<std::complex<double>>>>> vals;
	std::vector<std::vector<std::vector<std::complex<double>>>> cohs;

	std::vector<int> eegChans = iitp::interestEeg;
	std::vector<int> emgChans;

	const int minFreq = 8;
	const int numFreq = 44;
	const int numLen = 1;

	std::ofstream ofile;
	for(int fileNum : iitp::fileNums)
	{
		if(!QFile::exists(filePath(fileNum))) continue;

		ofile.open(resPath(fileNum).toStdString());
		dt.readEdfFile(filePath(fileNum));

		/// Test Guy
//		emgChans = iitp::interestEmg[fileNum];
		emgChans = {iitp::emgChans::Ta_r, iitp::emgChans::Fcr_r};

		/// test intresting EMGs
		std::cout << "fileNum = " << fileNum << "\tinteresting:" << std::endl;
//		std::cout << eegChans << std::endl;
//		std::cout << emgChans << std::endl;

		int numChansEeg = eegChans.size();
		int numChansEmg = emgChans.size();

		/// resize vals
		vals.resize(numChansEeg);
		cohs.resize(numChansEeg);
		for(int c1 = 0; c1 < numChansEeg; ++c1)
		{
			vals[c1].resize(numChansEmg);
			cohs[c1].resize(numChansEmg);

			for(int c2 = 0; c2 < numChansEmg; ++c2)
			{
				vals[c1][c2].resize(numFreq);
				cohs[c1][c2].resize(numFreq);

				for(int fff = 0; fff < numFreq; ++fff)
				{
					vals[c1][c2][fff].resize(numLen);
				}
			}
		}

		/// fillVals
		/// for each pieceLength
		for(int lenInd = 0; lenInd < numLen; ++lenInd)
//		int lenInd = 24;
		{
//			dt.cutPieces(1. + 0.001 * lenInd);
			dt.cutPieces(1.024);

			/// for each freq
			for(int fff = 0; fff < numFreq; ++fff)
			{
//				for(int c1 = 0; c1 < numChansEeg; ++c1)
				int eegCounter = -1;
				for(int c1 : eegChans)
				{
					++eegCounter;
//					for(int c2 = 0; c2 < numChansEmg; ++c2)
					int emgCounter = -1;
					for(int c2 : emgChans)
					{
						++emgCounter;
						/// coherencyMine or coherencyUsual
						std::complex<double> tmpCoh =
								dt.coherency(
									dt.findChannel(iitp::eegNames[c1]),
									dt.findChannel(iitp::emgNames[c2]),
									fff + minFreq);
						vals[eegCounter][emgCounter][fff][lenInd] = tmpCoh;
					}
				}
			}
		}
		std::cout << "IITPfin: values counted" << std::endl;

		/// are stable?
		std::valarray<double> abss(numLen);
		std::valarray<double> args(numLen);
//		for(int c1 = 0; c1 < numChansEeg; ++c1)
		int eegCounter = -1;
		for(int c1 : eegChans)
		{
			++eegCounter;
//			for(int c2 = 0; c2 < numChansEmg; ++c2)
			int emgCounter = -1;
			for(int c2 : emgChans)
			{
				++emgCounter;
				for(int fff = 0; fff < numFreq; ++fff)
				{
					/// values for different pieceLengths
					const std::valarray<std::complex<double>> & tmp = vals[eegCounter][emgCounter][fff];

					std::transform(std::begin(tmp),
								   std::end(tmp),
								   std::begin(abss),
								   [](const std::complex<double> & in)
					{
						return std::abs(in);
					});

					std::transform(std::begin(tmp),
								   std::end(tmp),
								   std::begin(args),
								   [](const std::complex<double> & in)
					{
						return std::arg(in);
					});

					if(smLib::mean(abss) > 0.05
//					   && smLib::sigma(abss) * 2 < smLib::mean(abss)
//					   && smLib::sigma(args) < 0.4
					   )
					{
						ofile
								<< c1 << '\t'
								<< dt.getLabels(dt.findChannel(iitp::eegNames[c1])) << '\t'


								<< c2 << '\t'
								<< dt.getLabels(dt.findChannel(iitp::emgNames[c2])) << '\t'

								<< "freq = " << fff + minFreq << '\t'

								<< "val = " << smLib::doubleRound(smLib::mean(tmp), 3) << '\t'

								<< "abs = " << smLib::doubleRound(smLib::mean(abss), 3) << '\t'
								<< "sgm = " << smLib::doubleRound(smLib::sigma(abss), 3) << '\t'

								<< "arg = " << smLib::doubleRound(smLib::mean(args), 3) << '\t'
								<< "sgm = " << smLib::doubleRound(smLib::sigma(args), 3) << '\t'

								<< std::endl;

//						cohs[c1][c2][fff] = smLib::mean(tmp);
					}
				}
			}
		}
		ofile.close();
	}
}

void IITPconcat(const QString & guyName,
				const QString & eegPostfix,
				const QString emgPostfix)
{
	/// iitp
	///  0 - downsample EMG, 1 - upsample EEG
#ifdef UP_DOWN_S
#undef UP_DOWN_S
#define UP_DOWN_S 0
#endif



	def::ntFlag = true;

	for(int fileNum : iitp::fileNums)
	{
		const QString ExpNamePre = def::iitpFolder + "/" +
								   guyName + "/" +
								   guyName + "_" + rn(fileNum, 2);
		QString filePath;
		edfFile fil;

#if 01
		/// resample
#if UP_DOWN_S
		/// upsample EEGs
		filePath = ExpNamePre + eegPostfix + ".edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);
			filePath = ExpNamePre + eegPostfix + "_up.edf";
			fil.upsample(1000., filePath);
		}
#else
		/// downsample EMGs
		filePath = ExpNamePre + emgPostfix +".edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);
			filePath = ExpNamePre + emgPostfix + "_down.edf";
			fil.downsample(250., filePath);
		}
#endif

#endif


#if 01
		/// vertcat eeg+emg
#if UP_DOWN_S
		/// upsampled EEG
		filePath = ExpNamePre + eegPostfix + "_up.edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);
			filePath = ExpNamePre + emgPostfix + ".edf";
			if(QFile::exists(filePath))
			{
				fil.vertcatFile(filePath, {}).writeEdfFile(ExpNamePre + "_sum.edf");
			}
		}
#else
		/// downsampled EMG
		filePath = ExpNamePre + eegPostfix + ".edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);
			filePath = ExpNamePre + emgPostfix + "_down.edf";
			if(QFile::exists(filePath))
			{
				fil.vertcatFile(filePath, {}).writeEdfFile(ExpNamePre + "_sum.edf");
			}
		}
#endif

#endif
	}
}

void IITPcopyChannel(const QString & guy,
					 const QString & whatChan,
					 const QString & whereChan)
{
	def::ntFlag = true;

	for(int fileNum : iitp::fileNums)
	{
		edfFile fil;
		const QString filePath = def::iitpFolder + "/" + guy
								 + "/" + guy + "_" + rn(fileNum, 2)
								 + "_emg.edf";
		if(!QFile(filePath).exists()) { continue; }

		fil.readEdfFile(filePath);
		fil.setData(fil.findChannel(whereChan), fil.getData(whatChan));
		fil.writeEdfFile(filePath);
	}
}

void IITPdatToEdf(const QString & guyName)
{
	def::ntFlag = true;

	for(int fileNum : iitp::fileNums)
	{
		const QString ExpNamePre = def::iitpFolder + "/" +
								   guyName + "/" +
								   guyName + "_" + rn(fileNum, 2);
		QString filePath;

		/// dat to edf
		filePath = ExpNamePre + ".dat";
		if(QFile::exists(filePath))
		{
			edfFile fil1(filePath, inst::iitp);
			filePath = ExpNamePre + "_emg.edf";
			fil1.writeEdfFile(filePath);
		}
	}
}

void IITPfilter(const QString & guyName,
				const QString & postfix,
				bool emg,
				bool gonios,
				bool eeg)
{
	def::ntFlag = true;

	for(int fileNum : iitp::fileNums)
	{
		const QString ExpNamePre = def::iitpFolder + "/" +
								   guyName + "/" +
								   guyName + "_" + rn(fileNum, 2);
		QString filePath;
		edfFile fil;

		filePath = ExpNamePre + postfix + ".edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);

			/// notch EMG
			if(emg)
			{
				auto emgChans = fil.findChannels(iitp::emgNamesPrefixes);
				if(!emgChans.empty())
				{
					/// notch 50
					for(int fr = 50; fr <= 450; fr += 50)
					{
						fil.refilter(fr - 5, fr + 5, true, emgChans);
					}

					/// notch low freq
//					fil.refilter(0., 8, true, emgChans);
				}
			}

			/// gonios
			if(gonios)
			{
				auto gonioChans = fil.findChannels({"elbow", "wrist", "knee", "ankle"});
				if(!gonioChans.empty())
				{
					fil.refilter(0.1, 6, false, gonioChans); /// magic constants
				}
			}


			/// filter EEG
			if(eeg)
			{
				auto eegChans = fil.findChannels("EEG ");
				if(!eegChans.empty())
				{
					fil.refilter(45, 55, true, eegChans);	/// notch
					fil.refilter(0.5, 70, false, eegChans);	/// usual
				}
			}

			filePath = ExpNamePre + postfix
					   + ".edf";
			fil.writeEdfFile(filePath);
		}
	}
}


/// dsp and fft, up adn down
void IITPpre2(const QString & guyName)
{
	def::ntFlag = true;

	for(int fileNum : iitp::fileNums)
	{
		const QString ExpNamePre = def::iitpFolder + "/" +
								   guyName + "/" +
								   guyName + "_" + rn(fileNum, 2);
		QString filePath;
		edfFile fil;

#if 01
		/// dat to edf
		filePath = ExpNamePre + ".dat";
		if(QFile::exists(filePath))
		{
			edfFile fil1(filePath, inst::iitp);
			filePath = ExpNamePre + "_emg.edf";
			fil1.writeEdfFile(filePath);
		}

#endif


#if 01
		/// filter EMG notch + goniogramms
		for(QString addName :{"dsp", "fft"})
		{
//			if(addName == "dsp")
//			{
//				myLib::refilter = &myDsp::refilter;
//			}
//			else
//			{
//				myLib::refilter = &myLib::refilterFFT;
//			}

			filePath = ExpNamePre + "_emg.edf";
			if(QFile::exists(filePath))
			{
				fil.readEdfFile(filePath);

				for(int fr = 50; fr <= 450; fr += 50)
				{
					fil.refilter(fr - 5, fr + 5, true);
				}

				/// filter goniogramms
				std::vector<uint> chanList;
				for(int i = 0; i < fil.getNs(); ++i)
				{
					for(auto joint : {"elbow", "wrist", "knee", "ankle"})
					{
						if(fil.getLabels()[i].contains(joint, Qt::CaseInsensitive))
						{
							chanList.push_back(i);
							break;
						}
					}
				}
				filePath = ExpNamePre + "_emg_" + addName + ".edf";
				/// magic constants
				fil.refilter(0.1, 6, false, chanList).writeEdfFile(filePath);
			}
		}
#endif



#if 01
		/// divide EEG chans to prevent oversclaing amplitude
		filePath = ExpNamePre + "_eeg.edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);
			std::vector<uint> chanNums(fil.getNs());
			std::iota(std::begin(chanNums), std::end(chanNums), 0.);
//			chanNums.pop_back(); /// ECG
			fil.divideChannels(chanNums, 2.);
			filePath = ExpNamePre + "_eeg_div.edf";
			fil.writeEdfFile(filePath);
		}
#endif


#if 01
		/// filter EEG edfs, but not ECG
		for(QString addName :{"dsp", "fft"})
		{
//			if(addName == "dsp")
//			{
//				myLib::refilter = &myDsp::refilter;
//			}
//			else
//			{
//				myLib::refilter = &myLib::refilterFFT;
//			}

			filePath = ExpNamePre + "_eeg_div.edf";
			if(QFile::exists(filePath))
			{
				fil.readEdfFile(filePath);

				filePath = ExpNamePre + "_eeg_" + addName + ".edf";
				fil.refilter(45, 55, true).refilter(0.5, 70).writeEdfFile(filePath);
			}
		}
#endif


#if 01
		/// resample
		/// upsample EEGs
		for(QString addName :{"dsp", "fft"})
		{
//			if(addName == "dsp")
//			{
//				myLib::refilter = &myDsp::refilter;
//			}
//			else
//			{
//				myLib::refilter = &myLib::refilterFFT;
//			}

			filePath = ExpNamePre + "_eeg_" + addName + ".edf";
			if(QFile::exists(filePath))
			{
				fil.readEdfFile(filePath);
				filePath = ExpNamePre + "_eeg_" + addName + "_up.edf";
				fil.upsample(1000., filePath);
			}

			/// downsample EMGs
			filePath = ExpNamePre + "_emg_" + addName + ".edf";
			if(QFile::exists(filePath))
			{
				fil.readEdfFile(filePath);
				filePath = ExpNamePre + "_emg_" + addName + "_down.edf";
				fil.downsample(250., filePath);
			}
		}
#endif





		/// vertcat eeg+emg

		for(QString addName :{"dsp", "fft"})
		{
			/// upsampled EEG
			filePath = ExpNamePre + "_eeg_" + addName + "_up.edf";
			if(QFile::exists(filePath))
			{
				fil.readEdfFile(filePath);
				filePath = ExpNamePre + "_emg_" + addName + ".edf";
				if(QFile::exists(filePath))
				{
//					fil.vertcatFile(filePath, {}).writeEdfFile(ExpNamePre + "_sum_f.edf");
					fil.vertcatFile(filePath, {}).writeEdfFile(ExpNamePre + "_" + addName + "_up.edf");
				}
			}
			/// downsampled EMG
			filePath = ExpNamePre + "_eeg_" + addName + ".edf";
			if(QFile::exists(filePath))
			{
				fil.readEdfFile(filePath);
				filePath = ExpNamePre + "_emg_" + addName + "_down.edf";
				if(QFile::exists(filePath))
				{
//					fil.vertcatFile(filePath, {}).writeEdfFile(ExpNamePre + "_sum_f.edf");
					fil.vertcatFile(filePath, {}).writeEdfFile(ExpNamePre + "_" + addName + "_down.edf");
				}
			}
		}
	}
}

void IITPremoveZchans(const QString & guyName, const QString & dirPath)
{
	const QStringList fils = QDir(dirPath + "/" + guyName).entryList({"*_eeg.edf"});
	for(const QString & fl : fils)
	{
		edfFile fil;
		fil.readEdfFile(dirPath + "/" + guyName + "/" + fl);
		int a = fil.findChannel("Fpz");
		int b = fil.findChannel("Oz");
		fil.removeChannels({a, b});
		fil.writeEdfFile(fil.getFilePath());
	}
}

void IITPremoveZchans(const QString & hauptDir)
{
	const QStringList dirs = QDir(hauptDir).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(const QString & dr : dirs)
	{
//		if(!dr.contains("Victor")) continue;
		const QStringList fils = QDir(hauptDir + "/" + dr).entryList({"*.edf"});
		for(const QString & fl : fils)
		{
//			if(!fl.contains("_1") && !fl.contains("_2")) continue;
			edfFile fil;
			fil.readEdfFile(hauptDir + "/" + dr + "/" + fl);
			int a = fil.findChannel("Fpz");
			int b = fil.findChannel("Oz");
			fil.removeChannels({a, b});
			fil.writeEdfFile(fil.getFilePath());
		}
	}
}


void IITPemgToAbs(const QString & guyName,
				  QString postfix,
				  const QString & dirPath)
{
	/// replace EMG with its abs
	if(postfix.isEmpty())
	{
		std::cout << "IITPemgToAbs: empty postfix" << std::endl;
		return;
	}

	const QString direct = dirPath + "/" + guyName + "/";

	auto filePath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + postfix + ".edf";
	};

	auto outPath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + postfix + ".edf"; /// rewrite
	};

	iitp::iitpData dt;

	for(int fileNum : iitp::fileNums)
	{
		if(!QFile::exists(filePath(fileNum)))
		{
			std::cout << "IITPemgToAbs: file doesn't exist = "
					  << filePath(fileNum) << std::endl;
			continue;
		}

		dt.readEdfFile(filePath(fileNum));
		for(QString emgChan : iitp::emgNames)
		{
			auto num = dt.findChannel(emgChan);
			if(num == -1) { continue; }

			auto ab = dt.getData(num);
			ab -= smLib::mean(ab);
			ab = ab.apply(std::abs);
			dt.setData(num, ab);
		}
		dt.writeEdfFile(outPath(fileNum));
	}
}

void IITPcopyToCar(const QString & guyName)
{
	const QString inPath = def::iitpSyncFolder + "/" + guyName;
	const QString outPath = def::iitpSyncFolder + "/" + guyName + "_car";
	QDir().mkpath(outPath);
	for(const QString fileName : QDir(inPath).entryList({"*_stag*"}))
	{
		QFile::copy(inPath + "/" + fileName,
					outPath + "/" + fileName);
	}
}

void IITPrerefCAR(const QString & guyName,
				  const QString & dirPath,
				  const QString & addFilter)
{

	const auto & usedLabels = coords::lbl19;	/// to build reref array
	const auto & rerefLabels = coords::lbl19;	/// list to reref

	const QString workPath = dirPath + "/" + guyName
							 + "_car"			/// pewpew
							 ;


	QStringList edfs = QDir(workPath).entryList(def::edfFilters);

	for(QString fileName : edfs)
	{
		if(!addFilter.isEmpty() && !fileName.contains(addFilter)) { continue; }

		edfFile fil;
		fil.readEdfFile(workPath + "/" + fileName);

		/// refArr = (Fp1 + Fp2 + ... + O1 + O2)/19 - Ref
		std::valarray<double> refArr(fil.getDataLen());
		for(QString chanName : usedLabels)
		{
			int ref = fil.findChannel(chanName);
			refArr += fil.getData(ref);
		}
		refArr /= usedLabels.size();

		for(int i = 0; i < fil.getNs(); ++i)
		{
			auto it = std::find_if(std::begin(rerefLabels),
								   std::end(rerefLabels),
								   [fil, i](const QString & in)
			{ return fil.getLabels(i).contains(in); });

			if(it != std::end(rerefLabels))
			{

				fil.setData(i, fil.getData(i) - refArr);

				/// set new label *-CAR
				QString newLabel = fil.getLabels(i);
				newLabel = myLib::fitString(newLabel.left(newLabel.indexOf('-')) + "-CAR", 16);
				fil.setLabel(i, newLabel);
			}
		}
		fil.writeEdfFile(workPath + "/" + fileName
//						 .replace(".edf", "_car.edf") /// add name or not
						 );
	}
}

void IITPstagedToEnveloped(const QString & guyName,
						   QString postfix,
						   const QString & dirPath)
{
	/// replace EMG with its envelope
	if(postfix.isEmpty())
	{
		QStringList l = QDir(dirPath + "/" + guyName).entryList({"*_stag.edf"});
		if(!l.isEmpty())
		{
			postfix = iitp::getPostfix(l[0]);
		}
	}
	if(postfix.isEmpty())
	{
		QStringList l = QDir(dirPath + "/" + guyName).entryList({"*_new.edf"});
		if(!l.isEmpty())
		{
			postfix = iitp::getPostfix(l[0]);
		}
	}
	if(postfix.isEmpty())
	{
		std::cout << "IITPstagedToEnveloped: empty postfix" << std::endl;
		return;
	}

	const QString direct = dirPath + "/" + guyName + "/";

	auto filePath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + postfix + ".edf";
	};

	auto outPath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + postfix + "_env.edf";
	};

	iitp::iitpData dt;

	for(int fileNum : iitp::fileNums)
	{
		if(!QFile::exists(filePath(fileNum)))
		{
			std::cout << "IITPstagedToEnveloped: file doesn't exist = "
					  << filePath(fileNum) << std::endl;
			continue;
		}

		dt.readEdfFile(filePath(fileNum));
		for(QString emgChan : iitp::emgNames)
		{
			int num = dt.findChannel(emgChan);
			if(num == -1) { continue; }

			auto env = myLib::hilbertPieces(dt.getData(num));
			dt.setData(num, env);
		}
		dt.writeEdfFile(outPath(fileNum));
	}
}

void IITPstaging(const QString & guyName,
				 const QString & dirPath)
{
	QString postfix = iitp::getPostfix(QDir(dirPath + "/" + guyName).entryList({"*.edf"})[0]);
	postfix.remove("_stag");

	for(int fileNum : iitp::fileNums)
	{
		const QString ExpNamePre = dirPath + "/" +
								   guyName + "/" +
								   guyName + "_" + rn(fileNum, 2);
		QString filePath;
		iitp::iitpData fil;

		filePath = ExpNamePre + postfix + ".edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);

			if(fileNum < iitp::interestGonios.size())
			{
				for(int ch : iitp::interestGonios[fileNum])
				{
					fil.staging(ch);
				}
			}
			filePath = ExpNamePre + postfix + "_stag" + ".edf";
			fil.writeEdfFile(filePath);
		}

	}
}

void IITPdrawSameScale(const QString & guyName, const std::vector<int> & nums)
{
	std::vector<QString> paths;
	const QString workDir = def::iitpResFolder + "/" + guyName + "/sp";
	for(QString fileName : QDir(workDir).entryList({"*_sp.txt"}))
	{
		int fileNum = iitp::getFileNum(fileName);
		if(std::find(std::begin(nums), std::end(nums), fileNum) != std::end(nums))
		{
			paths.push_back(workDir + "/" + fileName);
		}
	}

	QPixmap templ = myLib::drw::drawTemplate(true, iitp::leftFr, iitp::rightFr, 19);

	auto res = myLib::drw::drawArraysSameScale(templ, paths);

	QString outPath;
	for(int i = 0; i < res.size(); ++i)
	{
		outPath = paths[i];
		outPath.replace("/sp/", "/pic/");
		outPath.replace("_sp.txt", ".jpg");
//		std::cout << outPath << std::endl;
		res[i].save(outPath, 0, 100);
	}
//	exit(0);
}

void IITPprocessStaged(const QString & guyName,
					   const QString & dirPath)
{
	const double periodicOverlap	= 0.5;
	const double continiousOverlap	= 0.5;
	const double confidenceLevel	= 0.05; /// for threshold value only

	for(const QString add : {"", "_car"})
	{
		const QString guyDir = guyName + add;
		const QString direct = dirPath + "/" + guyDir + "/";
		const QString resultsPathPrefix = def::iitpResFolder + "/" + guyDir + "/";
		const QString postfix = iitp::getPostfix(QDir(direct).entryList({"*_stag.edf"})[0]);

		if(!QDir(direct).exists()) { continue; }

		QDir a(def::iitpResFolder);
		a.mkdir(guyDir);
		a.cd(guyDir);
		a.mkdir("coh");
		a.mkdir("sp");
		a.mkdir("pic");
		a.mkdir("cohPics");
		a.mkdir("cohPicsMax");

		auto filePath = [=](int i) -> QString
		{
			return direct + guyName + "_" + rn(i, 2) + postfix + ".edf";
		};

		auto resFlex = [=](int i, QString goni) -> QString
		{
			return resultsPathPrefix + "coh/" + guyName + "_" + rn(i, 2) + "_" + goni + "_flexion.txt";
		};

		auto resExt = [=](int i, QString goni) -> QString
		{
			return resultsPathPrefix + "coh/" + guyName + "_" + rn(i, 2) + "_" + goni + "_extension.txt";
		};

//		///
//		static const std::vector<QString> cohTypes{"free", "flex", "ext"};
//		/// coh, fileNum, 0-rest, 1-flex, 2-ext
//		typedef std::tuple<iitp::iitpData::cohsType, int, int> cohItem;
//		std::vector<cohItem> cohs;

		/// added 2, 6, 10 due to letter 03.11.17
		QVector<int> interestingForLegs{0, 1, 2, 4, 6, 8, 10, 13, 14, 22, 23, 24, 25};  /// for maps drawing

		/// added due to talk 28.11.17
		QVector<int> interestingForWrists{0, 1, 6, 8, 10, 13, 14, 26, 27, 28, 29};  /// for maps drawing



		std::vector<iitp::forMap> forMapsVector;

		std::cout << guyName << std::endl;
		for(int fileNum : iitp::fileNums)
		{
			std::cout << "file " << fileNum << " in process" << std::endl;

			/// check bad files - no EMG, terrible EMG, etc
			try
			{
				if(myLib::contains(iitp::badFiles.at(guyName), fileNum))
				{
					std::cout << "IITPprocessStaged: bad file = "
							  << guyName << " " << fileNum << std::endl;
					continue;
				}
			}
			catch (...)
			{
				continue;
			}


			if(!QFile::exists(filePath(fileNum)))
			{
				std::cout << "IITPprocessStaged: file doesn't exist = "
						  << filePath(fileNum) << std::endl;
				continue;
			}

			iitp::iitpData dt;
			dt.readEdfFile(filePath(fileNum));

			/// rest, stat, imag - continious task
			if(iitp::interestGonios[fileNum].size() == 0)
			{
//				dt.countContiniousTaskSpectra();
				dt.cutPiecesW(continiousOverlap);
				dt.countContiniousTaskSpectraW(def::iitpResFolder + "/" + guyDir,
											   continiousOverlap);
//				continue;

				/// coherencies
				{

					std::ofstream outStr;
					outStr.open((resultsPathPrefix + "coh/"
								 + guyName + "_" + rn(fileNum, 2)
								 + "_" + iitp::trialTypesNames[iitp::trialTypes[fileNum]]
								+ ".txt").toStdString());

					outStr << std::fixed;
					outStr.precision(3);


					/// eeg-eeg
					for(int eeg1 : iitp::interestEeg)
					{
						for(int eeg2 : iitp::interestEeg)
						{
							if(eeg2 <= eeg1) continue;

							for(double fr : iitp::interestFrequencies)
							{
								auto val = dt.coherency(dt.findChannel(iitp::eegNames[eeg1]),
														dt.findChannel(iitp::eegNames[eeg2]),
														fr);
								if(val == std::complex<double>{})
								{
									std::cout << "IITPprocessStaged: bad val static eeg-eeg, "
											  << "fileNum = " << fileNum << ", "
											  << "chan1 = " << iitp::eegNames[eeg1]
											  << ", "
											  << "chan2 = " << iitp::eegNames[eeg2]
											  << std::endl;
								}

								//							if(std::abs(val) > 0.01)
								{
									outStr
											<< QString(iitp::eegNames[eeg1]) << '\t'
											<< QString(iitp::eegNames[eeg2]) << '\t'
											<< fr << '\t'
											<< std::pow(std::abs(val), 2) << '\t'
											<< std::abs(val) << '\t'
											<< std::arg(val) << '\t'
											<< "\r\n";
								}
							}
						}
					}

					/// eeg-emg
					for(int eeg : iitp::interestEeg)
					{
						for(int emg : iitp::interestEmg[fileNum])
						{
							for(double fr : iitp::interestFrequencies)
							{
								auto val = dt.coherency(dt.findChannel(iitp::eegNames[eeg]),
														dt.findChannel(iitp::emgNames[emg]),
														fr);
								if(val == std::complex<double>{})
								{
									std::cout << "IITPprocessStaged: bad val static eeg-emg, "
											  << "fileNum = " << fileNum << ", "
											  << "chan1 = " << iitp::eegNames[eeg]
											  << ", "
											  << "chan2 = " << iitp::emgNames[emg]
											  << std::endl;
								}

								//							if(std::abs(val) > 0.01)
								{
									outStr
											<< QString(iitp::eegNames[eeg]) << '\t'
											<< QString(iitp::emgNames[emg]) << '\t'
											<< fr << '\t'
											<< std::pow(std::abs(val), 2) << '\t'
											<< std::abs(val) << '\t'
											<< std::arg(val) << '\t'
											<< "\r\n";
								}
							}
						}
					}
					outStr << "confidenceLevel " << dt.confidence(confidenceLevel)
						   << "\tnumOfPieces " << dt.getPieces().size()
						   << std::endl;
					outStr.close();
				}
				if(interestingForLegs.contains(fileNum)
				   || interestingForWrists.contains(fileNum)
				   )
				{
//					cohs.push_back(cohItem(dt.getCoherencies(), fileNum, 0));
//					std::cout << 1 << std::endl;
					auto a = iitp::forMap(dt.getMSCoherencies(), dt, fileNum, "free");
//					std::cout << 2 << std::endl;
					forMapsVector.push_back(a);
//					std::cout << 3 << std::endl;
//					exit(0);
				}
			}

			else /// real or passive - periodic task
			{
				for(int gonio : iitp::interestGonios[fileNum]) /// for each joint
				{
					int minMarker = iitp::gonioMinMarker(gonio);
//					dt.countFlexExtSpectra(minMarker, minMarker + 1);
//					std::cout << iitp::gonioNames[gonio] << "\t";
					dt.countFlexExtSpectraW(def::iitpResFolder + "/" + guyDir,
											minMarker, minMarker + 1, periodicOverlap);
//					continue;


					for(int type : {0, 1}) /// 0 - flexion, 1 - extension
					{
						std::ofstream outStr;
						if(type == 0)
						{
//							dt.setPieces(minMarker, minMarker + 1);
							dt.setPiecesW(minMarker, minMarker + 1, periodicOverlap);
							outStr.open(resFlex(fileNum, iitp::gonioNames[gonio]).toStdString());
						}
						else
						{
//							dt.setPieces(minMarker + 1, minMarker);
							dt.setPiecesW(minMarker + 1, minMarker, periodicOverlap);
							outStr.open(resExt(fileNum, iitp::gonioNames[gonio]).toStdString());
						}

						outStr << std::fixed;
						outStr.precision(3);

						/// eeg-eeg
						for(int eeg1 : iitp::interestEeg)
						{
							for(int eeg2 : iitp::interestEeg)
							{
								if(eeg2 <= eeg1) continue;

								for(double fr : iitp::interestFrequencies)
								{
									auto val = dt.coherency(dt.findChannel(iitp::eegNames[eeg1]),
															dt.findChannel(iitp::eegNames[eeg2]),
															fr);
									if(val == std::complex<double>{})
									{
										std::cout << "IITPprocessStaged: bad val period eeg-eeg, "
												  << "fileNum = " << fileNum << ", "
												  << "chan1 = " << iitp::eegNames[eeg1]
												  << ", "
												  << "chan2 = " << iitp::eegNames[eeg2]
												  << std::endl;
									}

//									if(std::abs(val) > 0.01)
									{
										outStr
												<< QString(iitp::eegNames[eeg1]) << '\t'
												<< QString(iitp::eegNames[eeg2]) << '\t'
												<< fr << '\t'
												<< std::pow(std::abs(val), 2) << '\t'
												<< std::abs(val) << '\t'
												<< std::arg(val) << '\t'
												<< "\r\n";
									}
								}
							}
						}

						/// eeg-emg
						for(int eeg : iitp::interestEeg)
						{
							for(int emg : iitp::interestEmg[fileNum])
							{
								for(double fr : iitp::interestFrequencies)
								{
									auto val = dt.coherency(dt.findChannel(iitp::eegNames[eeg]),
															dt.findChannel(iitp::emgNames[emg]),
															fr);
									if(val == std::complex<double>{})
									{
										std::cout << "IITPprocessStaged: bad val period eeg-emg, "
												  << "fileNum = " << fileNum << ", "
												  << "chan1 = " << iitp::eegNames[eeg]
												  << ", "
												  << "chan2 = " << iitp::emgNames[emg]
												  << std::endl;
									}
//									if(std::abs(val) > 0.01)
									{
										outStr
												<< QString(iitp::eegNames[eeg]) << '\t'
												<< QString(iitp::emgNames[emg]) << '\t'
												<< fr << '\t'
												<< std::pow(std::abs(val), 2) << '\t'
												<< std::abs(val) << '\t'
												<< std::arg(val) << '\t'
												<< "\r\n";
									}
								}
							}
						}
						outStr << "confidenceLevel " << dt.confidence(confidenceLevel)
							   << "\tnumOfPieces " << dt.getPieces().size()
							   << std::endl;
						outStr.close();
						if((interestingForLegs.contains(fileNum) && type == 0) // flexion only
						   || (interestingForWrists.contains(fileNum) && type == 0)
					)
						{
//							cohs.push_back(cohItem(dt.getCoherencies(), fileNum, type + 1));
							forMapsVector.push_back(iitp::forMap(dt.getMSCoherencies(), dt, fileNum, "flex"));
						}
					} /// end for flex/ext
				} /// end for gonio joints
			} /// end of real/passive
		} /// end for all fileNums
#if 01
		IITPdrawCohMaps(forMapsVector, guyName, resultsPathPrefix);
#elif 0
		/// old
		std::cout << "cohs size = " << cohs.size() << std::endl;

		/// draw maps by cohs vector
		/// find maxValue
		double maxVal = 0.;
		for(const auto & cohTuple : cohs)
		{
			for(const auto & chan1 : iitp::interestEeg)
			{
				for(const auto & chan2 : {iitp::Ta_l, iitp::Ta_r})
				{
					for(const auto & freq : iitp::interestFrequencies)
					{
						maxVal = std::max(maxVal,
										  std::abs(
											  std::get<0>(cohTuple)
											  [dt.findChannel(iitp::eegNames[chan1])]
										  [dt.findChannel(iitp::emgNames[chan2])]
								 [freq]));
					}
				}
			}
		}
		/// round to 1 decimal up
		maxVal = std::ceil(maxVal * 10) / 10.;

		std::cout << "guy = " << guyName << "\t"
				  << "maxValue = " << maxVal << std::endl;

		std::valarray<double> drawData(19);
		drawData = 0;
		for(const auto & cohTuple : cohs)
		{
			for(const auto & freq : iitp::interestFrequencies)
			{
				for(const auto & chan2 : {iitp::Ta_l, iitp::Ta_r})
				{
					for(const auto & chan1 : iitp::interestEeg)
					{
						drawData[chan1] = std::abs(
											  std::get<0>(cohTuple)
											  [dt.findChannel(iitp::eegNames[chan1])]
										  [dt.findChannel(iitp::emgNames[chan2])]
										  [freq]);
					}
					myLib::drw::drawOneMap(drawData, maxVal, myLib::drw::ColorScale::jet).save(
								def::iitpResFolder + "/" + guyName + "/cohPics/" +
								guyName +
								"_" + rn(std::get<1>(cohTuple), 2) +		// fileNum
								"_" + cohTypes[std::get<2>(cohTuple)] +		// free/flex/ext
							"_" + iitp::emgNames[chan2] +					// Ta_l/Ta_r
							"_fr_" + rn(freq, 2) +							// _fr_10
							".jpg"
							, 0, 100);
					drawData = 0;
				}
			}
		}
#endif
	}
}

void IITPdrawCohMaps(const std::vector<iitp::forMap> & forMapsVector,
					 const QString & guyName,
					 const QString & resultsPathPrefix)
{



	/// add something about left leg due to letter 3.11.17
	/// added something about wrists 28.11.17

	/// find maxs
	const int numEmgs = iitp::forMapEmgNames.size();
	const int numRhythms = iitp::forMapRangeNames.size();



//	std::cout << 321 << std::endl;
//	QPixmap a(310, 240);
//	std::cout << 322 << std::endl;

	/// for meanVal
	matrix maxes(numEmgs, numRhythms); /// [numEmg][numRange]
	maxes = 0.;

	/// for maxVal
	matrix maxesM(numEmgs, numRhythms); /// [numEmg][numRange]
	maxesM = 0.;


//	std::cout << 321 << std::endl;
//	QPixmap b(310, 240);
//	std::cout << 322 << std::endl;

	for(iitp::forMap in : forMapsVector)
	{
		for(int i = 0; i < numEmgs; ++i)
		{
			if(in.fmChans[i] == -1) { continue; } /// e.g. no Wrists after 13th file
			for(int j = 0; j < numRhythms; ++j)
			{
				maxes[i][j] = std::max(maxes[i][j],
									   in.forMapRanges[i][j].meanVal.max());
				maxesM[i][j] = std::max(maxesM[i][j],
										in.forMapRanges[i][j].maxVal.max());
			}
		}
	}

	/// round to 1 decimal?
	if(0)
	{
		for(int i = 0; i < numEmgs; ++i)
		{
			for(int j = 0; j < numRhythms; ++j)
			{
				maxes[i][j] = std::ceil(maxes[i][j] * 10) / 10.;
				maxesM[i][j] = std::ceil(maxesM[i][j] * 10) / 10.;
			}
		}
	}

	if(0)
	{
		for(int i = 0; i < numEmgs; ++i)
		{
			std::cout << iitp::forMapEmgNames[i] << std::endl;
			for(int j = 0; j < numRhythms; ++j)
			{
				std::cout << "max " << iitp::forMapRangeNames[j] << " = "
						  << maxes[i][j] << "\n";
			}
			std::cout << std::endl;

		}
	}

//	std::cout << "forMaps size = " << sizeof(forMapsVector) << std::endl;

	/// draw
	for(iitp::forMap in : forMapsVector)
	{
		for(int i = 0; i < numEmgs; ++i)
		{
			if(in.fmChans[i] == -1) { continue; } /// e.g. no Wrists after 13th file
			if(!myLib::contains(iitp::interestEmg[in.fileNum],
								myLib::indexOfVal(iitp::emgNames,
												  iitp::forMapEmgNames[i]))) { continue; }
			for(int j = 0; j < numRhythms; ++j)
			{
//				std::this_thread::sleep_for(std::chrono::seconds{5});
				if(01)
				{
					std::cout << "arr = " << in.forMapRanges[i][j].meanVal << std::endl;
					std::cout << "max = " << maxes[i][j] << std::endl;
					std::cout << "outPath = " << resultsPathPrefix + "cohPics/" +
								 guyName +
								 "_" + rn(in.fileNum, 2) +			// fileNum
								 "_" + in.fileType +				// free/flex/ext
								 "_" + iitp::forMapEmgNames[i] +	// muscle
								 "_" + iitp::forMapRangeNames[j] +	// rhythm
								 ".jpg" << std::endl;
				}


				myLib::drw::drawOneMap(in.forMapRanges[i][j].meanVal,
									   maxes[i][j],
									   myLib::drw::ColorScale::jet,
									   true).save(
							resultsPathPrefix + "cohPics/" +
							guyName +
							"_" + rn(in.fileNum, 2) +			// fileNum
							"_" + in.fileType +					// free/flex/ext
							"_" + iitp::forMapEmgNames[i] +		// muscle
							"_" + iitp::forMapRangeNames[j] +	// rhythm
							".jpg"
							, 0, 100);
//				std::cout << 1 << std::endl;


				/// doubt
//				myLib::drw::drawOneMap(in.forMapRanges[i][j].maxVal,
//									   maxesM[i][j],
//									   myLib::drw::ColorScale::jet,
//									   true).save(
//							resultsPathPrefix + "cohPicsMax/" +
//							guyName +
//							"_" + rn(in.fileNum, 2) +			// fileNum
//							"_" + in.fileType +					// free/flex/ext
//							"_" + iitp::forMapEmgNames[i] +		// muscle
//							"_" + iitp::forMapRangeNames[j] +	// rhythm
//							".jpg"
//							, 0, 100);
			}
		}
	}
}

void IITPdrawSpectralMaps(const QString & guyName,
						  const QString & dirPath)
{
	/// read sp [4; 40) Hz, draw maps

	for(const QString add : {"", "_car"})
	{
		const QString guyDir = guyName + add;
		const QString inPath = dirPath + "/" + guyDir + "/sp/";
		const QString outPath = def::iitpResFolder + "/" + guyDir + "/specPics/";

		QDir a(def::iitpResFolder);
		a.mkdir(guyDir);
		a.cd(guyDir);
		a.mkdir("specPics");

		/// interesting fileNums
		QStringList markers{"*_04_*", "*_06_*", "*_08_*"};

		for(QString fileName : QDir(inPath).entryList(markers))
		{
			std::valarray<double> spec;
			myLib::readFileInLine(inPath + fileName, spec);

			matrix specMat(spec, uint(19));
			std::valarray<double> drawSpec(19);
			for(std::pair<int, int> a : {
				std::make_pair(4, 8),
				std::make_pair(8, 13),
				std::make_pair(13, 25),
				std::make_pair(25, 40)
		})
			{
				for(int i = 0; i < 19; ++i)
				{
					/// magic const 4, maybe iitp::
					drawSpec[i] = std::accumulate(std::begin(specMat[i]) + (a.first - 4),
												  std::begin(specMat[i]) + (a.second - 4),
												  0.);
				}
				/// repair T6
				drawSpec[16] = 0.;
				/// repair P4
				drawSpec[15] = (drawSpec[14] + drawSpec[16] + drawSpec[18] + drawSpec[10]) / 4.;
				/// repair F4
				drawSpec[5] = (drawSpec[4] + drawSpec[6] + drawSpec[1] + drawSpec[10]) / 4.;


				QString addName;
				switch(a.first)
				{
				case 4: { addName = "_theta"; break; }
				case 8: { addName = "_alpha"; break; }
				case 13: { addName = "_beta"; break; }
				case 25: { addName = "_gamma"; break; }
				default: { addName = "_pew"; break; }
				}

				fileName.remove(".txt");
				myLib::drw::drawOneMap(drawSpec, 0, myLib::drw::ColorScale::jet, true).save(
							outPath + fileName + addName + ".jpg", 0, 100);
			}
		}
	}
}

void IITPmaxCoh(const QString & filePath,
				int markerStart, int markerFin,
				int fftLen)
{
	int minFreq = 8;
	int maxFreq = 30;

	iitp::iitpData idt;
	idt.readEdfFile(filePath);
	if(markerStart != 0 && markerFin !=0)
	{
		idt.setPieces(markerStart, markerFin);
	}
	else
	{
		idt.cutPieces(fftLen / 1000.);
	}
	idt.setFftLen(fftLen);

	std::vector<std::vector<double>> vals;

	for(int c1 = 0; c1 < 19; ++c1)
	{
		for(int c2 = 20; c2 < 28; ++c2)
		{
			for(int fr = minFreq; fr < maxFreq; ++fr)
			{
				vals.push_back(std::vector<double>{std::abs(idt.coherency(c1, c2, fr)),
												   double(c1),
												   double(c2),
												   double(fr)});
			}
		}
	}
	std::sort(std::begin(vals), std::end(vals),
			  [](const std::vector<double> & in1, const std::vector<double> & in2)
	{
		return in1[0] > in2[0];
	});

	for(int i = 0; i < 10; ++i)
	{
		std::cout
				<< vals[i][1] << "\t"
							  << vals[i][2] << "\t"
							  << vals[i][3] << "\t"
							  << idt.coherency(vals[i][1], vals[i][2], vals[i][3])
				<< std::endl;
	}
}

} // end namespace autos



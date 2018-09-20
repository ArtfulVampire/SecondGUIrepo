
#include <widgets/mainwindow.h>

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <thread>

#include <other/matrix.h>
#include <other/edffile.h>
#include <other/autos.h>
#include <other/consts.h>
#include <other/defs.h>

#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/wavelet.h>
#include <myLib/output.h>

using namespace myOut;

namespace myLib
{

std::vector<QString> readBurdenkoLog(const QString & logPath)
{
	QFile inStr(logPath);
	inStr.open(QIODevice::ReadOnly);

	std::vector<QString> res(240);/// magic const

	/// discard first 5 lines
	for(int i = 0; i < 5; ++i)
	{
		inStr.readLine();
	}
	for(auto & inRes : res)
	{
		QString line = QString(inStr.readLine().toStdString().c_str());
		inRes = line.split('\t', QString::SkipEmptyParts)[2].mid(1);
	}
//	std::cout << res << std::endl;
	inStr.close();
	return res;
}

/// {timeBin, marker}
std::vector<std::pair<int, int>> handleAnnots(const std::vector<QString> & annotations,
											  double srate)
{
	const std::vector<std::pair<QString, int>> markers
	{
		{"Presentation stimulation code: 0 (stimPresentation)", 10},
		{"Presentation stimulation code: 1 (stimPresentation)", 11},
		{"stimPresentation", 15},
		{"eventButton", 20},
	};

	const QString ch00 = QString(QChar(0));
	const QString ch20 = QString(QChar(20));

	const QString sep1 = ch20 + ch00;
	const QString sep2 = ch20 + ch20;

	std::vector<std::pair<int, int>> res{};

	for(const QString & annot : annotations)
	{
		/// chop epoch time start
		int start{0};
		for(int i = 0; i < annot.size() - 1; ++i)
		{
			if(annot[i].unicode() == 20 && annot[i + 1].unicode() != 20) { start = i; break; }
		}
		QString tmp = annot.mid(start + 2);
//		std::cout << tmp << std::endl; continue;

		auto marks = tmp.split(sep1, QString::SkipEmptyParts);
		for(auto & mark : marks)
		{
			/// workaround "empty" parts
			mark.replace(QRegExp(" {2,}"), " ");
			mark.replace(QRegExp("\\0{2,}"), " ");
			if(mark.size() < 10) continue; /// magic const
			if(mark.contains(QChar(21))) continue; /// edf+ "duration"

//			std::cout << mark << std::endl; continue;
			auto par = mark.split(sep2, QString::SkipEmptyParts);
			int outMark{0};
			for(const auto & mrk : markers)
			{
				if(par[1].contains(mrk.first)) { outMark = mrk.second; break; }
			}
			if(outMark)
			{
				std::pair<int, int> newPair{std::round(par[0].remove('+').toDouble() * srate),
							outMark};

				/// same time (10 or 11) and 15
				if(outMark == 10 || outMark == 11) { --newPair.first; }

				if(res.size() < 3
				   ||
				   (res[res.size() - 1] != newPair
				   && res[res.size() - 2] != newPair))
				{
					res.push_back(newPair);
				}
			}
		}
	}
	return res;
}

} /// end namespace myLib


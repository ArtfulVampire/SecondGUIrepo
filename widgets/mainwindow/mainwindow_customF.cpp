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
	myLib::XeniaFinal();
//	myLib::GalyaProcessing();
	exit(0);

#if 0
	/// count correctness, average times, ICA
#if 0 /// new (~10 people)
	const QString dear = "FeedbackNewMark";
	const auto & guysList = subj::guysFBnew;
	const QString postfix = "_fin";
#else /// final (~16 people)
	const QString dear = "FeedbackFinalMark";
	const QString postfix = "_fin";
//	const QString postfix = "";
	const auto & guysList = subj::guysFBfinal.at(subj::fbGroup::all);
#endif

#if 01
	/// calculate successive for "New" and "Final" schemes on "New" data
	std::cout
			<< "NEW" << "\t"
			<< "FINAL" << "\t"
			<< "BASE" << "\t"
			<< std::endl;
	auto res = fb::calculateSuccessiveBoth(dear, guysList, postfix);

//	for(const auto & in : res)
//	{
//		std::cout
//				<< std::get<0>(in).first << "\t"
//				<< std::get<1>(in).first << "\t"
//				<< std::get<2>(in).first << "\t"
//				<< std::endl;
//	}
	exit(0);
#endif

//	fb::calculateICA(dear, guysList, postfix);

#if 01
	auto results = fb::coutAllFeatures(dear, guysList, postfix);
	for(const subj::fbGroup & group :
	{
		subj::fbGroup::experiment,
		subj::fbGroup::control,
		subj::fbGroup::improved,
		subj::fbGroup::not_improved}
		)
	{
		for(const auto & in : subj::guysFBfinal.at(group))
		{
			outStream << in.second << "\t" << results[in.second] << std::endl;
		}
		outStream << std::endl << std::endl;
	}
#endif

	exit(0);	
#endif

}

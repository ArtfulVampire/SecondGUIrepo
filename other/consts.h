#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <vector>

#include <QString>
#include <QColor>

using uint = unsigned int;

#if __cplusplus >= 201703L
#define CPP17 1
#else
#define CPP17 0
#endif

#ifndef WAVELET_MATLAB
#define WAVELET_MATLAB 0
#endif

/// iitp
///  0 - downsample EMG, 1 - upsample EEG
#ifndef UP_DOWN_S
#define UP_DOWN_S 01
#endif

/// classifier - should check
#ifndef INERTIA
#define INERTIA 0
#endif

#ifndef SHOW_MATI_WIDGETS
#define SHOW_MATI_WIDGETS 0
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

namespace def
{
	/// consts
	const bool withMarkersFlag{true};			/// remove everywhere as true
	const bool writeLongStartEnd{true};			/// don't know, do I need it?

	const QString spectraDataExtension			{"psd"};
	const QStringList edfFilters				{"*.edf", "*.EDF"};
	const std::vector<QColor> colours{ QColor("blue"),
				QColor("red"),
				QColor("green"),
				QColor("black"),
				QColor("gray")};

	const QString XeniaFolder			{"/media/Files/Data/Xenia"};
	const QString mriFolder				{"/media/Files/Data/MRI"};
	const QString GalyaFolder			{"/media/Files/Data/Galya"};
	const QString DashaFolder			{"/media/Files/Data/Dasha/AUDIO"};
	const QString uciFolder				{"/media/Files/Data/UCI"};
	const QString helpPath				{"/media/Files/Data/FeedbackFinalMark/Help"};

	const QString iitpFolder			{"/media/Files/Data/iitp"};
	const QString iitpSyncFolder		{"/media/Files/Data/iitp/SYNCED"};
	const QString iitpResFolder			{"/media/Files/Data/iitp/Results"};
	const QString matiFolder			{"/media/Files/Data/Mati"};

#if 0
	/// rhythms
	const std::pair<double, double> deltaRhythm{1., 4.};
	const std::pair<double, double> thetaRhythm{4., 8.};
	const std::pair<double, double> alphaRhythm{8., 13.};
	const std::pair<double, double> lowAlphaRhythm{8., 10.5};
	const std::pair<double, double> highAlphaRhythm{10.5, 13.};
	const std::pair<double, double> betaRhythm{13., 40.};
	const std::pair<double, double> lowBetaRhythm{13., 25.};
	const std::pair<double, double> highBetaRhythm{25., 40.};
	const std::pair<double, double> gammaRhythm{40., 70.};

	/// enum class for rhythms?
	const std::map<QString, std::pair<double, double>> rhythms /// unused yet
	{
		{"delta", {1., 4.}},
		{"theta", {4., 8.}},
		{"alpha", {8., 13.}},
		{"alphaLow", {8., 10.5}},
		{"alphaHigh", {10.5, 13.}},
		{"beta", {13., 40.}},
		{"betaLow", {13., 25.}},
		{"betaHigh", {25., 40.}},
		{"gamma", {40., 70.}}
	};
#endif
} /// end namespace def



#endif /// DEFAULTS_H

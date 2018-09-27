#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <QString>
#include <QColor>
#include <vector>
#include <map>

using uint = unsigned int;

#if __cplusplus >= 201703L
#define CPP17 1
#else
#define CPP17 0
#endif

#ifndef WAVELET_MATLAB
#define WAVELET_MATLAB 01
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


#ifndef SHOW_IITP_WIDGETS
#define SHOW_IITP_WIDGETS 01
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

namespace coords
{

/// relative coords
const double scale = 250./1600.;

/// + 2 EOG
const int numOfChan = 19; /// for drawTemplate - move to draw namespace
/// 1/32 + n * 6/32
const std::vector<double> x {0.21875, 0.59375,
							 0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
							 0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
							 0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
							 0.21875, 0.59375,
							 0.03125, 0.78125};
/// 6/32 * n = 3/16 * n
const std::vector<double> y {0.1875, 0.1875,
							 0.375, 0.375, 0.375, 0.375, 0.375,
							 0.5625, 0.5625, 0.5625, 0.5625, 0.5625,
							 0.75, 0.75, 0.75, 0.75, 0.75,
							 0.9375, 0.9375,
							 0.9375, 0.9375};
/// for BaseMonopolar rereference
const std::vector<QString> lbl_A1 {"Fp1",
								   "F7", "F3", "Fz",
								   "T3", "C3",
								   "T5", "P3", "Pz",
								   "O1",
								   "EOG2"};
const std::vector<QString> lbl_A2 {"Fp2",
								   "F4", "F8",
								   "Cz", "C4", "T4",
								   "P4", "T6",
								   "O2",
								   "EOG1"};
const std::vector<QString> lbl19_with_ears {"Fp1", "Fp2",
											"F7", "F3", "Fz", "F4", "F8",
											"T3", "C3", "Cz", "C4", "T4",
											"T5", "P3", "Pz", "P4", "T6",
											"O1", "O2",
											"A1", "A2"};
const std::vector<QString> lbl19 {"Fp1", "Fp2",
								  "F7", "F3", "Fz", "F4", "F8",
								  "T3", "C3", "Cz", "C4", "T4",
								  "T5", "P3", "Pz", "P4", "T6",
								  "O1", "O2"};

const std::vector<QString> lbl21 {"Fp1", "Fp2",
								  "F7", "F3", "Fz", "F4", "F8",
								  "T3", "C3", "Cz", "C4", "T4",
								  "T5", "P3", "Pz", "P4", "T6",
								  "O1", "O2",
								  "EOG1", "EOG2"};




/// Dasha/Neurotravel
const std::vector<QString> lbl31_less {"Fp1", "27", "Fp2",
									   "F3", "Fz", "F4", "F7", "F8",
									   "36", "39", "35", "29", "28",
									   "C3", "Cz", "C4", "38", "40",
									   "37", "P3", "Pz", "P4", "34",
									   "33", "T3", "T4", "T5", "T6",
									   "O1", "Oz", "O2"};

const std::vector<QString> lbl31_more_withEOG {"Fp1", "Fp2",
									   "F3", "F4", "C3", "C4", "P3", "P4",
									   "O1", "O2", "Pg1", "Pg2",
									   "F7", "F8", "T3", "T4", "T5", "T6",
									   "Cb1", "Cb2",
									   "Fz", "Cz", "Pz", "33", "Oz",
									   "A2", "A1",
									   "27", "28", "29",
									   "34", "35", "36", "37", "38", "39", "40"};
const std::vector<QString> lbl31_more {"Fp1", "Fp2",
									   "F3", "F4", "C3", "C4", "P3", "P4",
									   "O1", "O2",
									   "F7", "F8", "T3", "T4", "T5", "T6",
									   "Fz", "Cz", "Pz", "33", "Oz",
									   "A2", "A1",
									   "27", "28", "29",
									   "34", "35", "36", "37", "38", "39", "40"};

const std::vector<QString> lbl31_good {"Fp1", "27", "Fp2",
									   "F7", "F3", "Fz", "F4", "F8",
									   "29", "36", "39", "35", "28",
									   "T3", "C3", "Cz", "C4", "T4",
									   "34", "38", "40", "37", "33",
									   "T5", "P3", "Pz", "P4", "T6",
									   "O1", "Oz", "O2"};
/// fMRI
const std::vector<QString> lbl32 {"Fp1", "Fp2",
								  "F7", "F3", "Fz", "F4", "F8",
								  "T3", "C3", "Cz", "C4",
								  "Fpz", "Ft7", "Fc3", "Fc4", "Ft8", "Tp7",
								  "T4", "T5", "P3", "Pz", "P4", "T6",
								  "O1", "O2",
								  "Cp3", "Cpz", "Cp4", "Tp8", "Po7", "Po8", "Oz"
								 };
/// most wide list of channles - should add Neurotravel?
const std::vector<QString> lbl_all {"Fp1", "Fp2",
								  "F7", "F3", "Fz", "F4", "F8",
								  "T3", "C3", "Cz", "C4",
								  "Fpz", "Ft7", "Fc3", "Fc4", "Ft8", "Tp7",
								  "T4", "T5", "P3", "Pz", "P4", "T6",
								  "O1", "O2",
								  "Cp3", "Cpz", "Cp4", "Tp8", "Po7", "Po8", "Oz",
								  "A1", "A2"
								 };
const std::vector<int> leest19 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};


}



namespace def
{
	/// consts
	const bool withMarkersFlag{true};			/// remove everywhere as true
	const bool writeLongStartEnd{true};			/// don't know, do I need it?

//	const QString plainDataExtension			{"scg"}; /// deprecated
//	const QStringList plainFilters				{"*.scg"}; /// deprecated
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
#endif

	const std::map<QString, std::pair<double, double>> rhythms
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
} /// end namespace def

namespace coords
{

const std::map<int, QString> kyrToLatin{
	{1072, "a"},
	{1073, "b"},
	{1074, "v"},
	{1075, "g"},
	{1076, "d"},
	{1077, "e"},
	{1078, "zh"},
	{1079, "z"},
	{1080, "i"},
	{1081, "j"},
	{1082, "k"},
	{1083, "l"},
	{1084, "m"},
	{1085, "n"},
	{1086, "o"},
	{1087, "p"},
	{1088, "r"},
	{1089, "s"},
	{1090, "t"},
	{1091, "u"},
	{1092, "f"},
	{1093, "kh"},
	{1094, "tz"},
	{1095, "ch"},
	{1096, "sh"},
	{1097, "sch"},
	{1098, ""},
	{1099, "yi"},
	{1100, ""},
	{1101, "e"},
	{1102, "yu"},
	{1103, "ya"},
	{1040, "A"},
	{1041, "B"},
	{1042, "V"},
	{1043, "G"},
	{1044, "D"},
	{1045, "E"},
	{1046, "ZH"},
	{1047, "Z"},
	{1048, "I"},
	{1049, "J"},
	{1050, "K"},
	{1051, "L"},
	{1052, "M"},
	{1053, "N"},
	{1054, "O"},
	{1055, "P"},
	{1056, "R"},
	{1057, "S"},
	{1058, "T"},
	{1059, "U"},
	{1060, "F"},
	{1061, "KH"},
	{1062, "TZ"},
	{1063, "CH"},
	{1064, "SH"},
	{1065, "SCH"},
	{1066, ""},
	{1067, "YI"},
	{1068, ""},
	{1069, "E"},
	{1070, "YU"},
	{1071, "YA"},
	{1105, "yo"},
	{1025, "YO"}
};

}

namespace coords
{
namespace egi
{

const int manyChannels = 100; /// replace with geodesicsFlag in edfFile
const std::vector<int> chans128to20
{
			21,				8,				/// Fp1, Fp2
	32,		23,		10,		123,	121,	/// F7, F3, Fz, F4, F8
	44,		35,		54,		103,	107,	/// T7(T3), C3, CPz, C4, T8(T4)
	57,		51,		61,		91,		95,		/// P7(T5), P3, Pz, P4, P8(T6)
			69,				82,				/// O1, O2
	7, 13, 20, 24, 124, 127,				/// EOGs
	128,
};									/// markers

const std::vector<QString> chans128to20str
{
			" 22 ",			" 9 ",						/// Fp1, Fp2
	" 33 ",	" 24 ",	" 11 ",	" 124 "," 122 ",			/// F7, F3, Fz, F4, F8
	" 45 ",	" 36 ",	" 55 ",	" 104 "," 108 ",			/// T7(T3), C3, CPz, C4, T8(T4)
	" 58 ",	" 52 ",	" 62 ",	" 92 ",	" 96 ",				/// P7(T5), P3, Pz, P4, P8(T6)
			" 70 ",			" 83 ",						/// O1, O2
	" 8 ", " 14 ", " 21 ", " 25 ", " 125 ", " 128 ",	/// EOGs
	"Status",											/// markers
};

const std::vector<QString> chans128groups
{
	"Fp1", "Fp2",
	"F_l", "F7", "F3", "Fz", "F_mid", "F4", "F8", "F_r",
	"FCz", "SMF",
	"C_l", "C3", "C4", "C_r"
	"T_l", "T7", "T8", "T_r",
	"TPO_l", "T5", "T6", "TPO_r"
	"P_l", "P3", "Pz", "P_mid", "P4", "P_r",
	"O_l", "O1", "O2", "O_r",
	"eog",
	"other",
};

const std::map<QString, std::vector<QString>> chans128
{
	/// Fp
	{"Fp1",
		{
			" 22 ",
		}
	},
	{"Fp2",
		{
			" 9 ",
		}
	},

	/// F
	{"F_l",
		{
			" 20 ",
			" 23 ",
			" 27 ",
			" 28 ",
			" 29 ",
		}
	},
	{"F7",
		{
			" 33 ",
		}
	},
	{"F3",
		{
			" 24 ",
		}
	},
	{"Fz",
		{
			" 11 ",
		}
	},
	{"F_mid",
		{
			" 4 ",
			" 5 ",
			" 10 ",
			" 12 ",
			" 16 ",
			" 18 ",
			" 19 ",
		}
	},
	{"F4",
		{
			" 124 ",
		}
	},
	{"F8",
		{
			" 122 ",
		}
	},
	{"F_r",
		{
			" 3 ",
			" 111 ",
			" 117 ",
			" 118 ",
			" 123 ",
		}
	},

	///FC = SMF
	{"FCz",
		{
			" 6 ",
		}
	},
	{"SMF",
		{
			" 7 ",
			" 13 ",
			" 30 ",
			" 106 ",
			" 112 ",
			" 105 ",
		}
	},

	/// C
	{"C_l",
		{
			" 31 ",
			" 35 ",
			" 37 ",
			" 41 ",
		}
	},
	{"C3",
		{
			" 36 ",
		}
	},

	{"C4",
		{
			" 104 ",
		}
	},
	{"C_r",
		{
			" 80 ",
			" 87 ",
			" 103 ",
			" 110 ",
		}
	},

	/// T
	{"T_l",
		{
			" 34 ",
			" 39 ",
			" 40 ",
			" 46 ",
			" 50 ",
		}
	},
	{"T7",
		{
			" 45 ",
		}
	},
	{"T8",
		{
			" 108 ",
		}
	},
	{"T_r",
		{
			" 101 ",
			" 102 ",
			" 109 ",
			" 115 ",
			" 116 ",
		}
	},

	/// P
	{"P_l",
		{
			" 42 ",
			" 47 ",
			" 51 ",
			" 53 ",
			" 60 ",
		}
	},
	{"P3",
		{
			" 52 ",
		}
	},
	{"Pz",
		{
			" 62 ",
		}
	},
	{"P_mid",
		{
			" 54 ",
			" 55 ",
			" 61 ",
			" 67 ",
			" 72 ",
			" 77 ",
			" 78 ",
			" 79 ",
		}
	},
	{"P4",
		{
			" 92 ",
		}
	},
	{"P_r",
		{
			" 85 ",
			" 86 ",
			" 93 ",
			" 97 ",
			" 98 ",
		}
	},

	/// TPO
	{"TPO_l",
		{
			" 65 ",
			" 59 ",
			" 64 ",
		}
	},
	{"T5",
		{
			" 58 ",
		}
	},
	{"T6",
		{
			" 96 ",
		}
	},
	{"TPO_r",
		{
			" 90 ",
			" 95 ",
			" 91 ",
		}
	},


	/// O
	{"0_l",
		{
			" 66 ",
			" 69 ",
			" 71 ",
			" 74 ",
		}
	},
	{"O1",
		{
			" 70 ",
		}
	},
	{"O2",
		{
			" 83 ",
		}
	},
	{"O_r",
		{
			" 76 ",
			" 82 ",
			" 84 ",
			" 89 ",
		}
	},

#if 0
	/// unneeded
	{"Fp_l",
		{
			" 26 ",
		}
	},
	{"Fpz",
		{
			" 15 ",
		}
	},
	{"Fp_mid",
		{
		}
	},
	{"Fp_r",
		{
			" 2 ",
		}
	},
	{"Oz",
		{
			" 75 ",
		}
	},
	{"O_mid",
		{
		}
	},
#endif

	{"eog",
		{
			" 125 ",	/// right-right
			" 8 ",		/// right-right-up
			" 14 ",		/// right-left-up
			" 21 ",		/// left-right-up
			" 25 ",		/// left-left-up
			" 128 ",	/// left-left
		}
	},
	{"other",
		{
			" 26 ",		/// Fp_l
			" 15 ",		/// Fpz
			" 2 ",		/// Fp_r
			" 75 ",		/// Oz

			/// too low line
			" 32 ",
			" 38 ",
			" 43 ",
			" 44 ",
			" 48 ",
			" 49 ",
			" 56 ",
			" 63 ",
			" 68 ",
			" 73 ",
			" 81 ",
			" 88 ",
			" 94 ",
			" 99 ",
			" 107 ",
			" 113 ",
			" 120 ",
			" 119 ",
			" 114 ",
			" 121 ",
			" 1 ",


			" 57 ",		/// left mastoid
			" 100 ",	/// right mastoid
			" 127 ",	/// left cheek
			" 126 ",	/// right cheek
			" 17 ",		/// nasion
		}
	},

};

}
}


#endif /// DEFAULTS_H

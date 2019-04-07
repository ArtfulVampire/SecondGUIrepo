#include <other/coords.h>

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
const std::vector<QString> lbl_A1 {			"Fp1",
								   "F7",	"F3",	"Fz",
								   "T3",	"C3",
								   "T5",	"P3",	"Pz",
											"O1",
																"EOG2"};

const std::vector<QString> lbl_A2 {							"Fp2",
															"F4",	"F8",
													"Cz",	"C4",	"T4",
															"P4",	"T6",
															"O2",
											"EOG1"};

const std::vector<QString> lbl19_with_ears {		"Fp1",			"Fp2",
											"F7",	"F3",	"Fz",	"F4",	"F8",
											"T3",	"C3",	"Cz",	"C4",	"T4",
											"T5",	"P3",	"Pz",	"P4",	"T6",
													"O1",			"O2",
													"A1",			"A2"};
const std::vector<QString> lbl19 {			"Fp1",			"Fp2",
									"F7",	"F3",	"Fz",	"F4",	"F8",
									"T3",	"C3",	"Cz",	"C4",	"T4",
									"T5",	"P3",	"Pz",	"P4",	"T6",
											"O1",			"O2"};

const std::vector<QString> lbl16noz {		"Fp1",		"Fp2",
									 "F7",	"F3",		"F4", "F8",
									 "T3",	"C3",		"C4", "T4",
									 "T5",	"P3",		"P4", "T6",
											"O1",		"O2"};

const std::vector<QString> lbl17noFP {	"F7",	"F3",	"Fz",	"F4",	"F8",
										"T3",	"C3",	"Cz",	"C4",	"T4",
										"T5",	"P3",	"Pz",	"P4",	"T6",
												"O1",			"O2"};

const std::vector<QString> lbl21 {			"Fp1",			"Fp2",
									"F7",	"F3",	"Fz",	"F4",	"F8",
									"T3",	"C3",	"Cz",	"C4",	"T4",
									"T5",	"P3",	"Pz",	"P4",	"T6",
											"O1",			"O2",
											"EOG1",			"EOG2"};




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



namespace egi
{
const int manyChannels = 100; /// remake with geodesicsFlag in edfFile
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

const std::vector<QString> chans128to19mark
{
			" 22 ",			" 9 ",						/// Fp1, Fp2
	" 33 ",	" 24 ",	" 11 ",	" 124 "," 122 ",			/// F7, F3, Fz, F4, F8
	" 45 ",	" 36 ",	" 55 ",	" 104 "," 108 ",			/// T7(T3), C3, CPz, C4, T8(T4)
	" 58 ",	" 52 ",	" 62 ",	" 92 ",	" 96 ",				/// P7(T5), P3, Pz, P4, P8(T6)
			" 70 ",			" 83 ",						/// O1, O2
	" 8 ", " 14 ", " 21 ", " 25 ", " 125 ", " 128 ",	/// EOGs
	"Status",											/// markers
};

const std::vector<QString> chans128to19noeyesMark
{
			" 22 ",			" 9 ",						/// Fp1, Fp2
	" 33 ",	" 24 ",	" 11 ",	" 124 "," 122 ",			/// F7, F3, Fz, F4, F8
	" 45 ",	" 36 ",	" 55 ",	" 104 "," 108 ",			/// T7(T3), C3, CPz, C4, T8(T4)
	" 58 ",	" 52 ",	" 62 ",	" 92 ",	" 96 ",				/// P7(T5), P3, Pz, P4, P8(T6)
			" 70 ",			" 83 ",						/// O1, O2
	"Status",											/// markers
};

const std::vector<QString> chans128groups
{
	"Fp1", "Fp2",
	"F_l", "F7", "F3", "Fz", "F_mid", "F4", "F8", "F_r",
	"FCz", "SMF",
	"C_l", "C3", "C4", "C_r",
	"T_l", "T7", "T8", "T_r",
	"TPO_l", "T5", "T6", "TPO_r",
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

} /// end namespace egi
} /// end namespace coords

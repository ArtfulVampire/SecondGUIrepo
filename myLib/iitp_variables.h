#ifndef IITP_VARIABLES_H
#define IITP_VARIABLES_H
#include <myLib/iitp_consts.h>

namespace iitp
{


/// [numFile] - interestingChannels
/// commented wrists and ankles - letter 7-Nov-17
const std::vector<std::valarray<int>> interestGonios{
	/// 0 eyes closed
	{},
	/// 1 eyes open
	{},
	/// 2 legs
	{/*Ankle_l, Ankle_r,*/ Knee_l, Knee_r}, /// commented Ankles 17.02.18 (Alex2)
	/// 3 legs imaginary
	{},
	/// 4 feet
	{Ankle_l, Ankle_r},
	/// 5 feet imaginary
	{},
	/// 6 wrists
	{Wrist_l, Wrist_r},
	/// 7 wrists imaginary
	{},
	/// 8 feet + wrists
	{Ankle_l, Ankle_r, Wrist_l, Wrist_r},
	/// 9 feet + wrists imaginary
	{},
	/// 10 legs + wrists
	{ /*Ankle_l, Ankle_r, */ Knee_l, Knee_r, Wrist_l, Wrist_r},
	/// 11 legs + wrists imaginary
	{},
	/// 12 static stress
	{},
	/// 13 middle eyes closed
	{},
	/// 14 middle eyes open
	{},
	/// 15 arms
	{Elbow_l, Elbow_r /*, Wrist_l, Wrist_r*/ },
	/// 16 arms imaginary
	{},
	/// 17 arms + legs
	{Elbow_l, Elbow_r, Knee_l, Knee_r /*, Ankle_l, Ankle_r, Wrist_l, Wrist_r*/ },
	/// 18 arms + legs imaginagy
	{},
	/// 19 legs passive
	{Knee_l, Knee_r  /*,Ankle_l, Ankle_r*/ },
	/// 20 arms passive
	{Elbow_l, Elbow_r /*, Wrist_l, Wrist_r*/ },
	/// 21 arms+legs passive
	{Elbow_l, Elbow_r, Knee_l, Knee_r /*, Wrist_l, Wrist_r, Ankle_l, Ankle_r*/ },
	/// 22 final eyes closed
	{},
	/// 23 final eyes open
	{},
	/// 24 weak Ta_l
	{},
	/// 25 weak Ta_r
	{},
	/// 26 weak Fcr_l
	{},
	/// 27 weak Fcr_r
	{},
	/// 28 weak Ecr_l
	{},
	/// 29 weak Ecr_r
	{},

		/// VR
	/// 30 VR + legs slow
	{Knee_l, Knee_r},
	/// 31 VR + legs fast
	{Knee_l, Knee_r},
	/// 32 VR + arms slow
	{Elbow_l, Elbow_r},
	/// 33 VR + arms fast
	{Elbow_l, Elbow_r},
	/// 34 VR + arms, legs slow
	{Knee_l, Knee_r, Elbow_l, Elbow_r},
	/// 35 VR + arms, legs fast
	{Knee_l, Knee_r, Elbow_l, Elbow_r}
};


const std::vector<int> interestEeg{
//	0,	/// Fp1
//	1,	/// Fp2,
	2,	/// F7
	3,	/// F3
	4,	/// Fz
	5,	/// F4 - usually bad
	6,	/// F8
	7,	/// T3
	8,	/// C3
	9,	/// Cz
	10,	/// C4
	11,	/// T4
	12,	/// T5
	13,	/// P3
	14,	/// Pz
	15,	/// P4 - usually bad
	16,	/// T5
	17,	/// O1
	18	/// O2
};

/// all with 12th
const std::valarray<double> fileNums = smLib::range<std::valarray<double>>(0, 35 + 1);
//const std::valarray<double> fileNums{1};


/// added 2, 6, 10 due to letter 03.11.17
const QVector<int> interestingForLegs{0, 1, 2, 4, 6, 8, 10, 13, 14, 22, 23, 24, 25, 30, 31, 34, 35};  /// for maps drawing

/// added due to talk 28.11.17
const QVector<int> interestingForWrists{0, 1, 6, 8, 10, 13, 14, 26, 27, 28, 29};  /// for maps drawing

/// don't apply any processing for these files
const std::map<QString, std::vector<int>> badFiles{
	{"Alex",	{0, 1, 2, 3, 4, 5}},
	{"Boris",	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},
	{"Dima",	{2}},
	{"Egor",	{5}},
	{"Ira",		{11, 13}},
	{"Isakov",	{9}},
	{"Victor",	{9}},
	{"Aliev2",	{16, 31}}
};


} /// end of namespace iitp

#endif /// IITP_VARIABLES_H

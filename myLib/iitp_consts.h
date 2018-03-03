#ifndef IITP_CONSTS_H
#define IITP_CONSTS_H
#include <myLib/signalProcessing.h>
#include <myLib/valar.h>

namespace iitp
{
/// const consts
const std::vector<QString> trialTypesNames
{
	"rest",
	"real",
	"imag",
	"passive",
	"stat"
};

/// class because of simple names
enum trialType {rest	= 0,
				real	= 1,
				imag	= 2,
				passive	= 3,
				stat	= 4};


const std::valarray<int> trialTypes{
	// 0 eyes closed
	trialType::rest,
	// 1 eyes open
	trialType::rest,
	// 2 legs
	trialType::real,
	// 3 legs imaginary
	trialType::imag,
	// 4 feet
	trialType::real,
	// 5 feet imaginary
	trialType::imag,
	// 6 wrists
	trialType::real,
	// 7 wrists imaginary
	trialType::imag,
	// 8 feet + wrists
	trialType::real,
	// 9 feet + wrists imaginary
	trialType::imag,
	// 10 legs + wrists
	trialType::real,
	// 11 legs + wrists imaginary
	trialType::imag,
	// 12 static stress
	trialType::stat,
	// 13 middle eyes closed
	trialType::rest,
	// 14 middle eyes open
	trialType::rest,
	// 15 arms
	trialType::real,
	// 16 arms imaginary
	trialType::imag,
	// 17 arms + legs
	trialType::real,
	// 18 arms + legs imaginagy
	trialType::imag,
	// 19 legs passive
	trialType::passive,
	// 20 arms passive
	trialType::passive,
	// 21 arms+legs passive
	trialType::passive,
	// 22 final eyes closed
	trialType::rest,
	// 23 final eyes open
	trialType::rest,
	// 24 weak Ta_l
	trialType::stat,
	// 25 weak Ta_r
	trialType::stat,
	// 26 weak Fcr_l
	trialType::stat,
	// 27 weak Fcr_r
	trialType::stat,
	// 28 weak Ecr_l
	trialType::stat,
	// 29 weak Ecr_r
	trialType::stat,

			/// VR
	// 30 VR + legs slow
	trialType::real,
	// 31 VR + legs fast
	trialType::real,
	// 32 VR + arms slow
	trialType::real,
	// 33 VR + arms fast
	trialType::real,
	// 34 VR + arms, legs slow
	trialType::real,
	// 34 VR + arms, legs fast
	trialType::real
};

enum gonioChans {Ankle_l	= 0,
				 Ankle_r	= 1,
				 Knee_l		= 2,
				 Knee_r		= 3,
				 Elbow_l	= 4,
				 Elbow_r	= 5,
				 Wrist_l	= 6,
				 Wrist_r	= 7};

const std::vector<QString> gonioNames {
	"Ankle_l",
	"Ankle_r",
	"Knee_l",
	"Knee_r",
	"Elbow_l",
	"Elbow_r",
	"Wrist_l",
	"Wrist_r"};




enum emgChans {Ta_l		= 0,
			   Ta_r		= 1,
			   Bf_l		= 2,
			   Bf_r		= 3,
			   Fcr_l	= 4,
			   Fcr_r	= 5,
			   Ecr_l	= 6,
			   Ecr_r	= 7,
			   Da_l		= 8,
			   Da_r		= 9,
			   Dp_l		= 10,
			   Dp_r		= 11
			  };

const std::vector<QString> emgNamesPrefixes {
	"Ta",
	"Bf",
	"Fcr",
	"Ecr",
	"Da",
	"Dp"
};


const std::vector<QString> emgNames {
	"Ta_l",
	"Ta_r",
	"Bf_l",
	"Bf_r",
	"Fcr_l",
	"Fcr_r",
	"Ecr_l",
	"Ecr_r",
	"Da_l",
	"Da_r",
	"Dp_l",
	"Dp_r"};


/// [numFile] - interestingChannels
/// maybe all EMGs for rest and/or stat
/// 1 May 17 - added Ta_l, Ta_r everywhere for maps drawing
/// 24 Feb 18 - commented all in rest files
const std::vector<std::valarray<int>> interestEmg{
	// 0 eyes closed
	{/*Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r*/},
	// 1 eyes open
	{/*Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r*/},
	// 2 legs
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 3 legs imaginary
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 4 feet
	{Ta_l, Ta_r},
	// 5 feet imaginary
	{Ta_l, Ta_r},
	// 6 wrists
	{Fcr_l, Fcr_r, Ecr_l, Ecr_r,    Ta_l, Ta_r},
	// 7 wrists imaginary
	{Fcr_l, Fcr_r, Ecr_l, Ecr_r,    Ta_l, Ta_r},
	// 8 feet + wrists
	{Ta_l, Ta_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 9 feet + wrists imaginary
	{Ta_l, Ta_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 10 legs + wrists
	{Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 11 legs + wrists imaginary
	{Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r},
	// 12 static stress
	{},
	// 13 middle eyes closed
	{/*Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r*/},
	// 14 middle eyes open
	{/*Ta_l, Ta_r, Bf_l, Bf_r, Fcr_l, Fcr_r, Ecr_l, Ecr_r*/},
	// 15 arms
	{Da_l, Da_r, Dp_l, Dp_r,    Ta_l, Ta_r},
	// 16 arms imaginary
	{Da_l, Da_r, Dp_l, Dp_r,    Ta_l, Ta_r},
	// 17 arms + legs
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 18 arms + legs imaginagy
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 19 legs passive
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 20 arms passive
	{Da_l, Da_r, Dp_l, Dp_r,    Ta_l, Ta_r},
	// 21 arms + legs passive
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 22 final eyes closed
	{/*Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r*/},
	// 23 final eyes open
	{/*Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r*/},
	// 24 weak Ta_l
	{Ta_l,    Ta_r},
	// 25 weak Ta_r
	{Ta_r,    Ta_l},
	// 26 weak Fcr_l
	{Fcr_l,    Ta_l, Ta_r},
	// 27 weak Fcr_r
	{Fcr_r,    Ta_l, Ta_r},
	// 28 weak Ecr_l
	{Ecr_l,    Ta_l, Ta_r},
	// 29 weak Ecr_r
	{Ecr_r,    Ta_l, Ta_r},

		/// VR
	// 30 VR + legs slow
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 31 VR + legs fast
	{Ta_l, Ta_r, Bf_l, Bf_r},
	// 32 VR + arms slow
	{Da_l, Da_r, Dp_l, Dp_r},
	// 33 VR + arms fast
	{Da_l, Da_r, Dp_l, Dp_r},
	// 34 VR + arms, legs slow
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r},
	// 35 VR + arms, legs fast
	{Da_l, Da_r, Dp_l, Dp_r, Ta_l, Ta_r, Bf_l, Bf_r}
};



const std::vector<QString> eegNames{
	"Fp1",
	"Fp2",
	"F7",
	"F3",
	"Fz",
	"F4",
	"F8",
	"T3",
	"C3",
	"Cz",
	"C4",
	"T4",
	"T5",
	"P3",
	"Pz",
	"P4",
	"T6",
	"O1",
	"O2"};



/// for spectra inspection
const double leftFr = 4.;
const double rightFr = 40.;
const myLib::windowName iitpWindow = myLib::windowName::Hamming;
const std::vector<double> interestFrequencies = smLib::range<std::vector<double>>(8, 45 + 1);



/// EXPERIMENTAL forMaps
const std::vector<QString> forMapEmgNames {
	"Ta_l",
	"Ta_r",
	"Fcr_l",
	"Fcr_r",
	"Ecr_l",
	"Ecr_r",

		/// VR
	"Bf_l",
	"Bf_r",
	"Da_l",
	"Da_r",
	"Dp_l",
	"Dp_r"
};

const std::vector<QString> forMapRangeNames {
	"alpha",
	"beta",
	"gamma"
};

} // end namespace iitp
#endif // IITP_CONSTS_H

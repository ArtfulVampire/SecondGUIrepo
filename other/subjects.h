#ifndef SUBJECTS_H
#define SUBJECTS_H

#include <vector>
#include <utility>
#include <QString>


namespace subj
{

const std::vector<std::pair<QString, QString>> guysFBnew {
	std::make_pair("CDV_n", "CDV"),
			std::make_pair("KKS_n", "KKS"),
			std::make_pair("KNL", "KNL"),
			std::make_pair("MSM", "MSM"),
//			std::make_pair("MID", "MID"), /// no third file
			std::make_pair("PDI", "PDI"),
			std::make_pair("PNA", "PNA"),
			std::make_pair("SKI", "SKI"),
			std::make_pair("SRV", "SRV"),
			std::make_pair("TAA", "TAA"),
			std::make_pair("TihAA", "TihAA"),
			std::make_pair("UIA", "UIA")
};

const std::vector<std::pair<QString, QString>> guysFBfinal {
	/// experiment
	std::make_pair("Avdeev", "AKV"),
			std::make_pair("Beketova", "BAM"),
			std::make_pair("Burtcev", "BAV"),
			std::make_pair("Dovbyish", "DEG"),
			std::make_pair("Ilyin", "IAE"),
			std::make_pair("Matasov", "MII"),
			std::make_pair("Medvedeva", "MSO"),
			std::make_pair("Parshikov", "PNU"),
			std::make_pair("Ryizhov", "RMS"),
			/// norm
			std::make_pair("Belousov_ua_n", "BDA"),
			std::make_pair("Broshevitskaya_n", "BND"),
			std::make_pair("Demchishin_ua_n", "DDS"),
			std::make_pair("Evstratov_n", "ENV"),
			std::make_pair("Kartavova_n", "KKE"),
			std::make_pair("Korchun_n", "KAV"),
			std::make_pair("Semyonov_n", "SAV"),
			std::make_pair("Sergeev_n", "SAA"),
			/// other
			std::make_pair("Evstyushin_n", "ENI"),		/// juleek
			std::make_pair("Kravetz_ua_n", "KEA"),		/// low performance
			std::make_pair("Laszov_n", "LMA"),			/// high performance
			std::make_pair("Markina", "MAM"),			/// low performance
			std::make_pair("Nikonenko", "NUA"),			/// low performance
			std::make_pair("Shafikova_s", "SDR"),		/// single data
			std::make_pair("Tankina_s", "TOA")			/// single data

};

const QStringList leest_less = {
	"Berlin-Henis",
	"Bushenkov",
	"CHemerisova",
	"Didkovskaya",
	"Grishina",
	"Ivanova",
	"Krasnobaev",
	"Melnik",
	"Paramonova",
	"Ryibalko",
	"Sarvardinova",
	"SHkarina",
	"Vasina",
//    "Zelenkov"
};
const QStringList leest_more = {
	"Burmistrova",
	"Garmotko",
	"Hanenya",
	"Kalinichenko",
	"Tinyanova"
};

const QStringList leest_non = {
	"tactile",
	"Dasha_GO",
	"Dasha_GZ",
	"Dasha_smell",
	"Dasha_smell_2"
};

const QStringList leest_mri = {
	"Ali-Zade",
	"Atanov",
	"Azieva",
	"Balaev",
	"Gavrilov",
	"Gudovich",
	"Ivashkin",
	"Kabanov",
	"Khasanov",
	"Khokhlov",
	"Levando",
	"Mezhenova",
	"Moskovtsev",
	"Muchanova",
	"Nikolaenko",
	"Novoselova",
	"Shevchenko",
	"Sushinsky",
	"Sushinsky2",
	"Umanskaya",
	"Vorobiev",
	"Zavyalova"
};
} // end namespace subj


#endif // SUBJECTS_H

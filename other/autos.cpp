#include <other/autos.h>

#include <myLib/clustering.h>
#include <myLib/iitp.h>
#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/wavelet.h>
#include <myLib/output.h>
#include <myLib/statistics.h>

 /// for std::defaultfloat
#include <ios>
#include <iostream>
#include <bits/ios_base.h>

using namespace myOut;

namespace autos
{


matrix makeTestData(const QString & outPath)
{
	matrix testSignals(8, 250 * 60 * 3); /// 3 min

	/// signals
	double helpDouble;
	double x, y;

	std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());

	std::uniform_real_distribution<double> distr(0, 1);

	for(int i = 0; i < testSignals.cols(); ++i)
	{
		helpDouble = 2. * pi * i / DEFS.getFreq() * 12.5; /// 12.5 Hz ~ 20 bins per period
		testSignals[0][i] = sin(helpDouble); /// sine

		testSignals[1][i] = (i + 2) % 29;      /// saw

		testSignals[2][i] = (i % 26 >= 13); /// rectangle

		x = distr(gen);
		y = distr(gen);
		testSignals[3][i] = std::sqrt(-2. * std::log(x)) * cos(2. * pi * y); /// gauss?

		testSignals[4][i] = std::abs(i % 22 - 11); /// triangle

		testSignals[5][i] = rand() % 27; /// noise

		x = distr(gen);
		y = distr(gen);
		testSignals[6][i] = std::sqrt(-2. * std::log(x)) * cos(2. * pi * y); /// gauss?

		testSignals[7][i] = std::pow(rand() % 13, 3); /// non-white noise
	}


	double sum1, sum2;
	/// normalize by dispersion = coeff
	double coeff = 10.;
	for(int i = 0; i < testSignals.rows(); ++i)
	{
		sum1 = smLib::mean(testSignals[i]);
		sum2 = smLib::variance(testSignals[i]);

		testSignals[i] -= sum1;
		testSignals[i] /= std::sqrt(sum2);
		testSignals[i] *= coeff;

	}
	matrix pewM(testSignals.rows(),
				testSignals.rows());
	pewM.random(-1., 1.);


	testSignals = pewM * testSignals;

	/// remake with edfs
//	myLib::writePlainData(outPath, testSignals);
//	myLib::writePlainData(outPath + "_", testSignals.resizeCols(2000)); /// edit out time
	return pewM;
}


void clustering()
{
	srand(time(nullptr));

	int numRow = 300;
	int numCol = 18;
	matrix cData(numRow, numCol);

	std::ifstream inStr;
	QString helpString = "/media/Files/Data/Mati/clust.txt";
	inStr.open(helpString.toStdString());
	for(int i = 0; i < numRow; ++i)
	{
		for(int j = 0; j < numCol; ++j)
		{
			inStr >> cData[i][j];
		}
	}


	std::vector<std::vector<double>> distOld;
	distOld.resize(numRow);
	for(int i = 0; i < numRow; ++i)
	{
		distOld[i].resize(numRow);
	}

	std::vector<int> types;
	types.resize(numRow);

	std::vector<std::vector<double>> dists; /// distance, i, j,

	std::vector<double> temp(4);

	std::vector<bool> boundDots;
	std::vector<bool> isolDots;

	boundDots.resize(numRow);
	std::fill(boundDots.begin(), boundDots.end(), false);
	isolDots.resize(numRow);
	std::fill(isolDots.begin(), isolDots.end(), true);

	for(int i = 0; i < numRow; ++i)
	{
		types[i] = i % 3;
		for(int j = i+1; j < numRow; ++j)
		{
			temp[0] = smLib::distance(cData[i], cData[j]);
			temp[1] = i;
			temp[2] = j;
			temp[3] = 0;

			dists.push_back(temp);

			distOld[i][j] = temp[0];
			distOld[j][i] = temp[0];
		}
	}

	clust::sammonProj(distOld, types,
					  "/media/Files/Data/Mati/sammon.jpg");
#if 0
	/// test

	const int N = 15;
	const int dim = 2;

	distOld.clear();
	distOld.resize(N);
	for(int i = 0; i < N; ++i)
	{
		distOld[i].resize(N);
	}

	vector< std::vector<double> > dots;
	dots.resize(N);
	vector<double> ass;
	ass.resize(dim);

	types.clear();
	types.resize(N);

	srand (756);
	for(int i = 0; i < N; ++i)
	{
		types[i] = i % 3;
		for(int j = 0; j < dim; ++j)
		{
			ass[j] =  -5. + 10.*( (rand())%300 ) / 150.;
		}
		dots[i] = ass;
	}

	for(int i = 0; i < N; ++i)
	{
		distOld[i][i] = 0.;
		for(int j = i+1; j < N; ++j)
		{
			distOld[i][j] = distance(dots[i], dots[j], dim);
			distOld[j][i] = distOld[i][j];
		}
	}

	sammonProj(distOld, types,
			   "/media/Files/Data/Mati/sammon.jpg");



exit(1);
return;

#endif


#if 0
	/// smallest tree
	std::sort(dists.begin(), dists.end(), mySort);
	/// make first bound

	boundDots[ dists[0][1] ] = true;
	isolDots[ dists[0][1] ] = false;

	boundDots[ dists[0][2] ] = true;
	isolDots[ dists[0][2] ] = false;

	dists[0][3] = 2;
	newDists.push_back(dists[0]);



	std::vector<std::vector<double> >::iterator itt;
	while (contains(isolDots.begin(), isolDots.end(), true))
	{
		/// adjust dists[i][3]
		for(std::vector<std::vector<double> >::iterator iit = dists.begin();
			iit < dists.end();
			++iit)
		{
			if(boundDots[ (*iit)[1] ])
			{
				(*iit)[3] += 1;
			}
			if(boundDots[ (*iit)[2] ])
			{
				(*iit)[3] += 1;
			}
		}

		/// set new bound ()
		for(itt = dists.begin();
			itt < dists.end();
			++itt)
		{
			if((*itt)[3] == 1) break;
		}

		boundDots[ (*itt)[1] ] = true;
		isolDots[ (*itt)[1] ] = false;

		boundDots[ (*itt)[2] ] = true;
		isolDots[ (*itt)[2] ] = false;

		(*itt)[3] = 2;
		newDists.push_back(*itt);
#if 0
		/// cout something
		for(int j = 0; j < 3; ++j)
		{
			std::cout << (*itt)[j] << '\t';
		}
		std::cout << std::endl;
#endif
	}
	std::sort(newDists.begin(), newDists.end(), mySort);
	std::vector<double> newD;
	for(int i = 0; i < newDists.size(); ++i)
	{
		newD.push_back(newDists[i][0]);
	}
	myLib::kernelEst(smLib::vecToValar(newD)).save("/media/Files/Data/Mati/clust.jpg", nullptr, 100);
	myLib::histogram(smLib::vecToValar(newD), 40).save("/media/Files/Data/Mati/clustH.jpg", nullptr, 100);
#endif
}

} /// end of namespace autos

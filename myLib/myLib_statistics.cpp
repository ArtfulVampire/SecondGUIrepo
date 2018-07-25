#include <myLib/statistics.h>

#include <myLib/dataHandlers.h>
#include <myLib/draws.h>
#include <myLib/small.h>
#include <myLib/output.h>

using namespace myOut;

namespace myLib
{

long long combination(int n, int k)
{
	double ans = 1.;
	for(int i = 0; i < std::min(k, n-k); ++i)
	{
		ans *= (n - i);
		ans /= (i + 1);
	}
	return (long long)(ans);
}


double factorialLog(int n)
{
	return 0.5 * std::log(2 * M_PI * n) + n * (std::log(n) - 1);
}

double combinationLog(int n, int k)
{
	/// by Stirling
	return factorialLog(n) / (factorialLog(k) * factorialLog(n - k));
}

double binomialPvalue(int n, int k, double successRate)
{
	double sum = 0.;
	for(int i = k; i <= n; ++i) /// number of success - not less than k
	{
		double b = std::pow(successRate, i) * std::pow(1. - successRate, n - i);

		const auto lim = std::min(i, n-i); /// C(n, i) = C(n, n-i)
		for(int j = 0; j < lim; ++j)
		{
			b *= (n - j);
			b /= (lim - j);
		}
		sum += b;
	}
	return sum;
}

int binomialLimitOfSignificance(int n, double successRate, double pval)
{
	for(int i = n * successRate; i <= n; ++i)
	{
		if(binomialPvalue(n, i, successRate) < pval) { return i; }
	}
	return n * successRate;
}

double binomialOneTailed(int num1, int num2, int numAll1, int numAll2)
{
	double p = num1 / double(numAll1);
	if(numAll2 == 0) { numAll2 = numAll1; }


	double sum = 0.;
	for(int i = num2; i <= numAll2; ++i)
	{
		/// combination inside, too large values
		long double a = std::pow(1. - p, numAll2 - i) * std::pow(p, i);
		for(int j = 0; j < std::min(i, numAll2 - i); ++j)
		{
			a *= (numAll2 - j);
			a /= (j + 1);
		}
		sum += a;
	}
	return sum;
}

QPixmap histogram(const std::valarray<double> & arr,
				  int numSteps,
				  std::pair<double, double> xMinMax,
				  const QString & color,
				  int valueMax)
{
	std::vector<double> values(numSteps, 0.);

	double xMin, xMax;
	if(xMinMax == decltype(xMinMax)()) /// if(xMinMax.isEmpty())
	{
		xMin = *std::min_element(std::begin(arr),
								 std::end(arr));
		xMax = *std::max_element(std::begin(arr),
								 std::end(arr));
	}
	else
	{
		xMin = xMinMax.first;
		xMax = xMinMax.second;
	}

	/// complex sheet
	const double denom = std::pow(10, -std::floor(std::log10(xMax - xMin)));
	xMin = std::floor(xMin * denom) / denom;
	xMax = std::ceil(xMax * denom) / denom;

	std::for_each(std::begin(arr),
				  std::end(arr),
				  [xMin, xMax, numSteps, &values](double in)
	{
		int a = int(std::floor((in - xMin) / ((xMax-xMin) / numSteps)));
		if(a >= 0 && a < numSteps)
		{
			values[ a ] += 1.;
		}
	});

	if(valueMax <= 0)
	{
		valueMax = *std::max_element(std::begin(values),
									 std::end(values));
	}


	QPixmap pic(numSteps * 15, 400);
	QPainter pnt;
	pic.fill();
	pnt.begin(&pic);

	pnt.setPen("black");
	pnt.setBrush(QBrush("black"));


	pnt.setPen(color);
	pnt.setBrush(QBrush(QColor(color)));
	for(int i = 0; i < numSteps; ++i)
	{
		pnt.drawRect (QRect(QPoint(i * pic.width() / numSteps,
								   pic.height() * 0.9 * ( 1. - values[i] / valueMax)),
							QPoint((i+1) * pic.width() / numSteps,
								   pic.height() * 0.9)
							)
					  );
	}

	pnt.setPen("black");
	pnt.setBrush(QBrush(QColor("black")));
	pnt.drawLine(0,
				 pic.height() * 0.9,
				 pic.width(),
				 pic.height() * 0.9);

	for(double i = xMin; i <= xMax; i += (xMax - xMin) / 20.)
	{
		const double X = pic.width() * (i - xMin) / (xMax - xMin);
		pnt.drawLine(X, pic.height() * 0.9,
					 X, pic.height() * 0.95);
		pnt.drawText(X - 5, pic.height() - 3,
					 nm(i));
	}
	for(double i = 1; i <= valueMax; i += valueMax / 20.)
	{
		const double Y = pic.height() * 0.9 * (1. - double(i) / valueMax);
		pnt.drawLine(0, Y,
					 10, Y);
		pnt.drawText(13, Y + 5,
					 nm(i));
	}
	pnt.end();
	return pic;
}


void kernelEst(QString filePath, QString picPath)
{
	std::valarray<double> arr = readFileInLine(filePath);
	kernelEst(arr, picPath);
}

void kernelEst(const std::valarray<double> & arr, QString picPath)
{
	double sigma = 0.;
	int length = arr.size();

	sigma = smLib::variance(arr);
	sigma = std::sqrt(sigma);
	double h = 1.06 * sigma * std::pow(length, -0.2);

	QPixmap pic(1000, 400);
	QPainter pnt;
	pic.fill();
	pnt.begin(&pic);
	pnt.setPen("black");

	std::vector<double> values(pic.width(), 0.);

	double xMin, xMax;

	xMin = *std::min_element(std::begin(arr),
							 std::end(arr));
	xMax = *std::max_element(std::begin(arr),
							 std::end(arr));

	xMin = std::floor(xMin);
	xMax = std::ceil(xMax);

	///    sigma = (xMax - xMin);
	///    xMin -= 0.1 * sigma;
	///    xMax += 0.1 * sigma;

	///    /// generality
	///    xMin = -20;
	///    xMax = 20;

	///    xMin = 65;
	///    xMax = 100;


	for(int i = 0; i < pic.width(); ++i)
	{
		for(int j = 0; j < length; ++j)
		{
			values[i] += 1 / (length * h)
						 * smLib::gaussian((xMin + (xMax - xMin) / double(pic.width()) * i - arr[j]) / h);
		}
	}

	double valueMax;
	valueMax = *std::max_element(std::begin(values),
								 std::end(values));

	for(int i = 0; i < pic.width() - 1; ++i)
	{
		pnt.drawLine(i,
					 pic.height() * 0.9 * ( 1. - values[i] / valueMax),
					 i + 1,
					 pic.height() * 0.9 * (1. - values[i + 1] / valueMax));

	}
	pnt.drawLine(0,
				 pic.height() * 0.9,
				 pic.width(),
				 pic.height() * 0.9);

	/// x markers - 10 items
	for(int i = xMin; i <= xMax; i += (xMax - xMin) / 10)
	{
		pnt.drawLine((i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height() * 0.9,
					 (i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height());
		pnt.drawText((i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height() * 0.95,
					 nm(i));
	}
	pic.save(picPath, 0, 100);
}


QPixmap kernelEst(const std::valarray<double> & arr)
{
	int length = arr.size();

	const double sigma = smLib::sigma(arr);
	const double h = 1.06 * sigma * std::pow(length, -0.2);

	QPixmap pic(1000, 400);
	QPainter pnt;
	pic.fill();
	pnt.begin(&pic);
	pnt.setPen("black");

	double xMin = *std::min_element(std::begin(arr),
									std::end(arr));
	double xMax = *std::max_element(std::begin(arr),
									std::end(arr));

	xMin = std::floor(xMin);
	xMax = std::ceil(xMax);

	///    sigma = (xMax - xMin);
	///    xMin -= 0.1 * sigma;
	///    xMax += 0.1 * sigma;

	///    /// generality
	///    xMin = -20;
	///    xMax = 20;

	///    xMin = 65;
	///    xMax = 100;


	std::vector<double> values(pic.width(), 0.);
	for(int i = 0; i < pic.width(); ++i)
	{
		for(int j = 0; j < length; ++j)
		{
			values[i] += 1. / (length * h)
						 * smLib::gaussian((xMin + (xMax - xMin) / double(pic.width()) * i - arr[j]) / h);
		}
	}

	double valueMax = *std::max_element(std::begin(values),
										std::end(values));

	for(int i = 0; i < pic.width() - 1; ++i)
	{
		pnt.drawLine(i,
					 pic.height() * 0.9 * ( 1. - values[i] / valueMax),
					 i + 1,
					 pic.height() * 0.9 * (1. - values[i + 1] / valueMax));

	}
	pnt.drawLine(0,
				 pic.height() * 0.9,
				 pic.width(),
				 pic.height() * 0.9);

	/// x markers - 10 items
	for(double i = xMin; i <= xMax; i += (xMax - xMin) / 10.)
	{
		pnt.drawLine((i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height() * 0.9,
					 (i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height());
		pnt.drawText((i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height() * 0.95,
					 nm(i));
	}
	return pic;
}

#if 0
/// tests on normality
bool gaussApproval(double * arr, int length) /// kobzar page 239
{
	double z;
	int m = int(length/2);
	double a[m+1];
	double disp = variance(arr, length) * length;
	double B = 0.;
	double W;

	a[0] = 0.899 / std::pow(length-2.4, 0.4162) - 0.02;
	for(int j = 1; j <= m; ++j)
	{
		z = (length - 2*j + 1.) / (length - 0.5);
		a[j] = a[0] * (z + 1483 / std::pow(3.-z, 10.845) + std::pow(71.61, -10.) / std::pow(1.1-z, 8.26));
		B += a[j] * (arr[length-j+1] - arr[j-1]); /// or without +1
	}
	B *= B;
	W = (1 - 0.6695 / std::pow(length, 0.6518)) * disp / B;

	if(W < 1.) return true;
	return false;
}


bool gaussApproval(const QString & filePath)
{
	std::valarray<double> arr = readFileInLine(filePath);
	return gaussApproval(arr.data(), arr.size());
}

bool gaussApproval2(double * arr, int length) /// kobzar page 238
{
	double W = 0.;
	double disp = variance(arr, length) * length;
	double c[length+1];
	double sum = 0.;
	for(int i = 1; i <= length; ++i)
	{
		sum += std::pow(rankit(i, length), 2.);
	}
	sum = std::sqrt(sum);
	for(int j = 1; j < length; ++j)
	{
		c[j] = rankit(length - j + 1, length) / sum;
		W += c[j] * (arr[length - j] - arr[j - 1]);
	}
	W /= disp;

	/// NOT READY
	return false;


}
#endif


double quantile(double alpha) /// zValue(1 - pValue)
{
	/// Kobzar page 27, approx 15
	return (4.91 * (std::pow(alpha, 0.14) - std::pow(1. - alpha, 0.14)));
}

double normalCumulative(double zVal) /// (1 - pValue)(zValue)
{
#if 01
	/// Kobzar page 27, approx 8
	return 1. - 0.852 * std::exp(-std::pow((zVal + 1.5774) / 2.0637, 2.34));
#else
	/// Kobzar page 28, approx 18
	return 1 - 0.5 * std::exp(-0.717 * zVal - 0.416 * zVal * zVal);
#endif

}

double rankit(int i, int length, double k)
{
	return quantile( (i-k) / (length + 1. - 2. * k) );
}



void countRCP(QString filePath, QString picPath, double * outMean, double * outSigma)
{
	std::valarray<double> arr = readFileInLine(filePath);

	(*outMean) = smLib::mean(arr);
	(*outSigma) = smLib::sigma(arr);

	if(!picPath.isEmpty())
	{
		kernelEst(arr, picPath);
	}
}


void drawRCP(const std::valarray<double> & values, const QString & picPath)
{
	QPixmap pic(1000, 400);
	QPainter pnt;
	pic.fill();
	pnt.begin(&pic);
	pnt.setPen("black");
	int length = values.size();

	int numOfDisp = 4;

	std::valarray<double> line(pic.width());
	for(int i = 0; i < pic.width(); ++i)
	{
		line[i] = smLib::gaussian( (i - pic.width()/2) / (pic.width()/2.) * numOfDisp );
	}

	line /= line.max();

	for(int i = 0; i < pic.width() - 1; ++i)
	{
		pnt.drawLine(i,
					 pic.height() * 0.9 * (1. - line[i]),
					 i+1,
					 pic.height() * 0.9 * (1. - line[i+1]));
	}
	pnt.drawLine(0,
				 pic.height() * 0.9,
				 pic.width(),
				 pic.height() * 0.9);


	for(int i = 0; i < length; ++i) /// draw the values
	{
		int coordinate = pic.width()/2. * (1. + values[i] / numOfDisp);
		if(i%2 == 0) /// raw data
		{
			pnt.setPen("blue");
		}
		else /// ica data
		{
			pnt.setPen("red");
		}
		pnt.drawLine(coordinate,
					 pic.height() * 0.9 * ( 1. - line[coordinate]),
					 coordinate,
					 pic.height() * 0.9 * ( 1. - line[coordinate]) - 50);
	}
	pic.save(picPath, 0, 100);
}


/// Kobzar page 454?
std::pair<double, whichGreater> MannWhitney(const std::valarray<double> & arr1,
											const std::valarray<double> & arr2)
{
	std::vector<std::pair <double, int>> arr; /// composed row

	/// add first array
	std::for_each(std::begin(arr1),
				  std::end(arr1),
				  [&arr](double in)
	{ arr.push_back(std::make_pair(in, 1)); });

	/// add second array
	std::for_each(std::begin(arr2),
				  std::end(arr2),
				  [&arr](double in)
	{ arr.push_back(std::make_pair(in, 2)); });

	std::sort(std::begin(arr),
			  std::end(arr),
			  [](std::pair<double, int> i,
			  std::pair<double, int> j) { return i.first < j.first; });

	const int N1 = arr1.size();
	const int N2 = arr2.size();

	/// count sums
	int sum1 = 0;
	for(uint i = 0; i < arr.size(); ++i)
	{
		if(arr[i].second == 1)
		{
			sum1 += i + 1;
		}
	}
	double U = sum1 - N1 * (N1 + 1) / 2.;

	const double average = N1 * N2 / 2.;
	const double sigma = std::sqrt(N1 * N2 * ( N1 + N2 + 1 ) / 12.);
	const double zValue = std::abs(U - average) / sigma;


	whichGreater res2{};
	if(U > average)
	{
		res2 = whichGreater::first;
	}
	else
	{
		res2 = whichGreater::second;
	}

	/// zValue -> pValue
	return std::make_pair(1. - myLib::normalCumulative(zValue), res2);
}

int MannWhitney(const std::valarray<double> & arr1,
				const std::valarray<double> & arr2,
				const double p)
{
	/// check output!
	/// 0 - not different
	/// 1 - arr1 > arr2
	/// 2 - arr2 > arr1

	const std::pair<double, whichGreater> res = MannWhitney(arr1, arr2);

	if(res.first > p)							{ return 0; }	/// not different
	else if(res.second == whichGreater::first)	{ return 1; }	/// arr1 > arr2
	else if(res.second == whichGreater::second)	{ return 2; }	/// arr2 > arr1
	else return 0;												/// never get here
}

template <typename Typ>
void writeMannWhitney(const trivector<Typ> & MW,
					  const QString & outPath,
					  const QString & separator)
{
	std::ofstream fil;
	fil.open(outPath.toStdString());

	if(typeid(Typ) == typeid(double))
	{
		fil << std::fixed;
		fil.precision(5);
	}

	const int numChans = MW[0][1].size() / DEFS.spLength();

	/// 0-1, 0-2, 0-3, ... 0-N, 1-2, 1-3, 1-4, ... 1-N, ... (N-1)-N
	for(uint i = 0; i < MW.size(); ++i)
	{
		for(uint j = i + 1; j < MW.size(); ++j)
		{
#if 1
			/// each channel in a row
			for(int ch = 0; ch < numChans; ++ch)
			{
				for(int sp = 0; sp < DEFS.spLength(); ++sp)
				{
					fil << MW[i][j - i][sp + ch * DEFS.spLength()] << separator;
				}
				fil << std::endl;
			}
#else
			/// all channels in a row
			fil << MW[i][j - i];
#endif
			fil << std::endl;
		}
	}
	fil.flush();
	fil.close();
}
template void writeMannWhitney(const trivector<int> & MW, const QString & outPath, const QString & separator);
template void writeMannWhitney(const trivector<double> & MW, const QString & outPath, const QString & separator);

trivector<int> countMannWhitney(const QString & spectraPath,
								matrix * averageSpectraOut,
								matrix * distancesOut)
{

	trivector<int> res; /// [class1][class2][NetLength]


	/// 0 - Spatial, 1 - Verbal, 2 - Rest
	std::vector<matrix> spectra = myLib::readSpectraDir(spectraPath);
	const int NetLength = spectra[0][0].size();

	for(matrix & in : spectra) { in.transpose(); }

	matrix averageSpectra(spectra.size(), NetLength, 0);
	matrix distances(spectra.size(), spectra.size(), 0);

	res.resize(spectra.size());
	for(uint i = 0; i < spectra.size(); ++i) /// first class
	{
		res[i].resize(spectra.size());
		for(uint j = i + 1; j < spectra.size(); ++j) /// second class
		{
			res[i][j - i].resize(NetLength);
			int dist1 = 0;
			for(int k = 0; k < NetLength; ++k)
			{
				res[i][j - i][k] = MannWhitney(spectra[i][k],
											   spectra[j][k],
											   0.05);
				if(res[i][j - i][k] != 0)
				{
					++dist1;
				}
			}
			distances[i][j - i] = dist1 / double(NetLength);
		}
	}

	if(averageSpectraOut != nullptr)
	{
		(*averageSpectraOut) = std::move(averageSpectra);
	}
	if(distancesOut != nullptr)
	{
		(*distancesOut) = std::move(distances);
	}
	return res;
}

trivector<double> countMannWhitneyD(const QString & spectraPath)
{
	trivector<double> res;

	/// 0 - Spatial, 1 - Verbal, 2 - Rest
	std::vector<matrix> spectra = myLib::readSpectraDir(spectraPath);
	const int NetLength = spectra[0][0].size();

	for(matrix & in : spectra) { in.transpose(); }

	res.resize(spectra.size());
	for(uint i = 0; i < spectra.size(); ++i) /// first class
	{
		res[i].resize(spectra.size());
		for(uint j = i + 1; j < spectra.size(); ++j) /// second class
		{
			res[i][j - i].resize(NetLength);
			for(int k = 0; k < NetLength; ++k)
			{
				res[i][j - i][k] = MannWhitney(spectra[i][k],
											   spectra[j][k]).first;
			}
		}
	}

	return res;
}

void MannWhitneyFromMakepa(const QString & spectraDir, const QString & outPicPath)
{
	matrix inSpectraAv;
	matrix dists;
	trivector<int> MW;

	MW = countMannWhitney(spectraDir,
						  &inSpectraAv,
						  &dists);

	/// make drw
	drawTemplate(outPicPath);
	drawArrays(outPicPath,
			   inSpectraAv);
	drawMannWitney(outPicPath,
				   MW);

	QString helpString = DEFS.dirPath() + "/results.txt";
	std::ofstream outStr;
	outStr.open(helpString.toStdString(), std::ios_base::app);
	if(!outStr.good())
	{
		std::cout << "can't open log file: " << helpString << std::endl;
	}
	else
	{
		for(int i = 0; i < DEFS.numOfClasses(); ++i)
		{
			for(int j = i + 1; j < DEFS.numOfClasses(); ++j)
			{
				outStr << "dist " << i << " - " << j << '\t' << dists[i][j - i] << '\n';
			}
		}
	}
	outStr.flush();
	outStr.close();
}



template <typename Typ>
double mean(const Typ & arr, int length, int shift)
{
	return std::accumulate(std::begin(arr) + shift,
						   std::end(arr) + shift + length,
						   0.)
			/ length;
}
template double mean(const std::vector<double> &, int, int);
template double mean(const std::valarray<double> &, int, int);

/// needed for fractal dimension
template <typename Typ>
double covariance(const Typ &arr1, const Typ &arr2, int length, int shift, bool fromZero)
{
	double res = 0.;
	double m1 = 0.;
	double m2 = 0.;
	if(!fromZero)
	{
		m1 = mean(arr1, length, shift);
		m2 = mean(arr2, length, shift);
	}
	for(int i = 0; i < length; ++i)
	{
		res += (arr1[i + shift] - m1) *
				(arr2[i + shift] - m2);
	}
	return res;
}
template double covariance(const std::vector<double> &arr1, const std::vector<double> &arr2, int length, int shift, bool fromZero);
template double covariance(const std::valarray<double> &arr1, const std::valarray<double> &arr2, int length, int shift, bool fromZero);

} /// namespace myLib

#include <chrono>

#include <myLib/wavelet.h>
#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <myLib/valar.h>
#include <other/defs.h>

#include <QPixmap>
#include <QPainter>

using namespace myOut;

namespace wvlt
{

#if WAVELET_MATLAB
bool isInit = false;

int initMtlb()
{
	if(isInit) { return 1; }
	auto t0 = std::chrono::high_resolution_clock::now();
	std::cout << "matlab lib load starts" << std::endl;
	if (!libcwt_rInitializeWithHandlers(nullptr, nullptr)){
		std::cout << "Could not initialize the application." << std::endl;
		return -1;
	}
	if (!libcwt_rInitialize()){
		std::cout << "Could not initialize the library." << std::endl;
		return -2;
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << "matlab lib load ends" << std::endl
		 << "time elapsed = "
		 << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count() / 1000.
		 << " sec" << std::endl;
	isInit = true;
	return 1;
}

int termMtlb()
{
	if(!isInit) { return 1; }
	try
	{
		libcwt_rTerminate();
	}
	catch(...)
	{
		return -1;
	}
	isInit = false;
	return 1;
}

matrix cwt(const std::valarray<double> & signal, double srate)
{
	if(!isInit)
	{
		initMtlb();
	}
	const int numFreqs = 19; /// number of frequencies in cwt.m (2:1:20)

	mxArray * srate_ = mxCreateDoubleScalar(srate);
	mxArray * inData = mxCreateDoubleMatrix(1, signal.size(), mxREAL);
	double * it = mxGetPr(inData);
	for(auto datum : signal)
	{
		*it = datum;
		++it;
	}
	mxArray * res = mxCreateDoubleMatrix(numFreqs, signal.size(), mxCOMPLEX);

	mlfCWT(1, &res, inData, srate_);
	double * itt = mxGetPr(res);

	matrix result(numFreqs, signal.size());

	for(int col = 0; col < result.cols(); ++col)
	{
		for(int row = 0; row < result.rows(); ++row)
		{
			result[row][col] = *itt;
			++itt;
		}

	}
	mxDestroyArray(res);
	mxDestroyArray(inData);
	mxDestroyArray(srate_);

	return result;
}

void drawWaveletMtlb(const matrix & inData, const QString & picPath)
{
	/// height and length in pixels
	const int barH = 30;
	const int barL = 1;

	QPixmap pic(inData.cols() * barL,
				inData.rows() * barH);
	pic.fill();
	QPainter painter;
	painter.begin(&pic);

	/// normalization to [0, 1]
	matrix drawData = inData;
	drawData += drawData.minVal();
	drawData *= wvlt::range / drawData.maxVal();

	for(int i = 0; i < inData.rows(); ++i)
	{
		for(int j = 0; j < inData.cols(); ++j)
		{
			painter.setBrush(QBrush(wvlt::hueJet(wvlt::range, drawData[i][j])));
			painter.setPen(wvlt::hueJet(wvlt::range, drawData[i][j]));

			painter.drawRect( j * barL,
							  pic.height() - (i + 1) * barH, /// +1 'cause left UPPER
							  barL,
							  barH);
		}
	}
	painter.setPen("black");

	painter.setFont(QFont("Helvetica", 28, -1, -1));

	painter.setPen(Qt::DashLine);
	for(int i = 0; i < inData.rows(); ++i)
	{
		painter.drawLine(0,
						 i * barH,
						 pic.width(),
						 i * barH);
		painter.drawText(0,
						 pic.height() - (i + 0.5) * barH,
						 nm(i + 2)); /// +2 from scales [2:1:20]
	}

	painter.setPen(Qt::SolidLine);
	for(int i = 0; i < std::floor(inData.cols() / DEFS.getFreq()); ++i)
	{
		painter.drawLine(i * DEFS.getFreq() * barL,
						 pic.height(),
						 i * DEFS.getFreq() * barL,
						 pic.height() - 20);
		painter.drawText(i * DEFS.getFreq() * barL - 8,
						 pic.height() - 2,
						 nm(i));

	}
	pic.save(picPath, nullptr, 100);
	painter.end();
}
#endif




double morletCosNew(double const freq1, /// Hz
					const double timeShift,
					const double time)
{
	double freq = freq1 * 2. * pi;
	double res =  std::sqrt(2. * freq / pi_sqrt / morletFall)
				  * cos(freq * (time - timeShift) / DEFS.getFreq())
				  * exp(-0.5 * std::pow(freq / morletFall * (time - timeShift) / DEFS.getFreq(), 2));
	return res;
}

double morletSinNew(double const freq1,
					const double timeShift,
					const double time)
{
	double freq = freq1 * 2. * pi;
	double res =  std::sqrt(2. * freq / pi_sqrt / morletFall)
				  * sin(freq * (time - timeShift) / DEFS.getFreq())
				  * exp(-0.5 * std::pow(freq / morletFall * (time - timeShift) / DEFS.getFreq(), 2));
	return res;
}


const int timeStep = 5; /// 0.1 sec (the same results up to ~50)
int halfSize(double freq)
{
	return 3 * morletFall * DEFS.getFreq() / freq;
}

std::valarray<double> morletSinGen(double fr)
{
	const double freq = fr * 2. * pi;
	std::valarray<double> res(2 * halfSize(fr));
	res[halfSize(fr)] = 0.;
	for(int i = 0; i < halfSize(fr); ++i)
	{
		res[halfSize(fr) + i] = std::sqrt(2. * freq / pi_sqrt / morletFall)
								* std::sin(freq * i / DEFS.getFreq())
								* std::exp(-0.5 * std::pow(freq / morletFall * i / DEFS.getFreq(), 2));
		res[halfSize(fr) - i] = -res[halfSize(fr) + i];
	}
	return res;
}

std::valarray<double> morletCosGen(double fr)
{
	const double freq = fr * 2. * pi;
	std::valarray<double> res(2 * halfSize(fr));
	res[halfSize(fr)] = 1.;
	for(int i = 0; i < halfSize(fr); ++i)
	{
		res[halfSize(fr) + i] = std::sqrt(2. * freq / pi_sqrt / morletFall)
								* std::cos(freq * i / DEFS.getFreq())
								* std::exp(-0.5 * std::pow(freq / morletFall * i / DEFS.getFreq(), 2));
		res[halfSize(fr) - i] = res[halfSize(fr) + i];
	}
	return res;
}


std::vector<matrix> myCWT(const matrix & fileData,
						  const std::vector<double> & freqs)
{
	/// what if fileData < 2*halfSize ???
	std::vector<matrix> res(fileData.rows(),
							matrix(freqs.size(),
								   std::ceil(fileData.cols() / static_cast<double>(wvlt::timeStep)),
								   0.));

	for(int frNum = 0; frNum < freqs.size(); ++frNum)
	{
		const double freq = freqs[frNum];

		const std::valarray<double> morletSin{morletSinGen(freq)};
		const std::valarray<double> morletCos{morletCosGen(freq)};

		for(int sigNum = 0; sigNum < fileData.rows(); ++sigNum)
		{
			const std::valarray<double> & input = fileData[sigNum];

			int sliceNum = 0;
			for(int mid = 0;
				mid < fileData.cols();
				mid += wvlt::timeStep, ++sliceNum)
			{
				std::valarray<double> in(2 * halfSize(freq));

				if(mid - halfSize(freq) < 0)
				{
					const int len = mid + halfSize(freq);
					in[std::slice(in.size() - len, len, 1)] = input[std::slice(0, len, 1)]
							/ len * (2 * halfSize(freq))
							;
				}
				else if(mid + halfSize(freq) > input.size())
				{
					const int len = input.size() - (mid - halfSize(freq));
					in[std::slice(0, len, 1)] = input[std::slice(input.size() - len, len, 1)]
							/ len * (2 * halfSize(freq))
							;
				}
				else
				{
					in[std::slice(0, 2 * halfSize(freq), 1)]
							= input[std::slice(mid - halfSize(freq), 2 * halfSize(freq), 1)];
				}
				/// square abs
				res[sigNum][frNum][sliceNum] = std::pow(smLib::prod(morletSin, in), 2)
											   + std::pow(smLib::prod(morletCos, in), 2);
			}
		}
	}
	return res;
}

matrix myCWT(const std::valarray<double> & inSignal, const std::vector<double> & freqs)
{
	matrix res(freqs.size(),
			   std::ceil(inSignal.size() / static_cast<double>(wvlt::timeStep)),
			   0.);

	for(int frNum = 0; frNum < freqs.size(); ++frNum)
	{
		const double freq = freqs[frNum];

		const std::valarray<double> morletSin{morletSinGen(freq)};
		const std::valarray<double> morletCos{morletCosGen(freq)};

		int sliceNum = 0;
		for(int mid = 0;
			mid < inSignal.size();
			mid += wvlt::timeStep, ++sliceNum)
		{
			std::valarray<double> in(2 * halfSize(freq));

			if(mid - halfSize(freq) < 0)
			{
				const int len = mid + halfSize(freq);
				in[std::slice(in.size() - len, len, 1)] = inSignal[std::slice(0, len, 1)]
						* std::pow( (2 * halfSize(freq)) / len, 0.5 )
						;
			}
			else if(mid + halfSize(freq) > inSignal.size())
			{
				const int len = inSignal.size() - (mid - halfSize(freq));
				in[std::slice(0, len, 1)] = inSignal[std::slice(inSignal.size() - len, len, 1)]
						* std::pow( (2 * halfSize(freq)) / len, 0.5 )
						;
			}
			else
			{
				in[std::slice(0, 2 * halfSize(freq), 1)]
						= inSignal[std::slice(mid - halfSize(freq), 2 * halfSize(freq), 1)];
			}
			/// square abs
			res[frNum][sliceNum] = std::pow(smLib::prod(morletSin, in), 2)
								   + std::pow(smLib::prod(morletCos, in), 2);
		}
	}
	return res;
}


/// look myLib_draw and _drw
double red(int range, double j, double V, double S)
{
	double part = j / static_cast<double>(range);
	/// matlab
	if    (0. <= part && part <= wvlt::colDots[0])			{ return V*(1.-S); }
	if(wvlt::colDots[0] < part && part <= wvlt::colDots[1])	{ return V*(1.-S); }
	if(wvlt::colDots[1] < part && part <= wvlt::colDots[2])	{ return V*(1.-S) + V*S*(part-wvlt::colDots[1])/(wvlt::colDots[2] - wvlt::colDots[1]); }
	if(wvlt::colDots[2] < part && part <= wvlt::colDots[3])	{ return V; }
	if(wvlt::colDots[3] < part && part <= 1.)				{ return V - V*S*(part-wvlt::colDots[3])/(1 - wvlt::colDots[3])/2.; }
	return 0.0;
#if 0
	/// old
	if    (0.000 <= part && part <= 0.167)	{ return V*(1.-S); /// 2. - V*S/2. + V*S*(part)*3.; }
	if(0.167 < part && part <= 0.400)		{ return V*(1.-S); }
	if(0.400 < part && part <= 0.500)		{ return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.; }
	if(0.500 < part && part <= 0.600)		{ return V*(1.-S) + V*S*(part-0.400)/(0.500-0.400)/2.; }
	if(0.600 < part && part <= 0.833)		{ return V; }
	if(0.833 < part && part <= 1.000)		{ return V - V*S*(part-0.833)/(1.000-0.833)/2.; }
	return 0.0;
#endif
}
double green(int range, double j, double V, double S)
{
	double part = j / static_cast<double>(range);
	/// matlab
	if    (0.0 <= part && part <= wvlt::colDots[0])				{ return V*(1.-S); }
	if(wvlt::colDots[0] < part && part <= wvlt::colDots[1])	{ return V*(1.-S) + V*S*(part-wvlt::colDots[0])/(wvlt::colDots[1] - wvlt::colDots[0]); }
	if(wvlt::colDots[1] < part && part <= wvlt::colDots[2])	{ return V; }
	if(wvlt::colDots[2] < part && part <= wvlt::colDots[3])	{ return V - V*S*(part-wvlt::colDots[2])/(wvlt::colDots[3] - wvlt::colDots[2]); }
	if(wvlt::colDots[3] < part && part <= 1.)				{ return V*(1.-S); }
	return 0.0;
#if 0
	/// old
	double hlp = 1.0;
	if    (0.000 <= part && part <= 0.167)	{ return V*(1.-S); }
	if(0.167 < part && part <= 0.400)		{ return V*(1.-S) + V*S*hlp*(part-0.167)/(0.400-0.167); }
	if(0.400 < part && part <= 0.500)		{ return V-V*S*(1.-hlp); }
	if(0.500 < part && part <= 0.600)		{ return V-V*S*(1.-hlp); }
	if(0.600 < part && part <= 0.833)		{ return V-V*S*(1.-hlp) - V*S*hlp*(part-0.600)/(0.833-0.600); }
	if(0.833 < part && part <= 1.000)		{ return V*(1.-S); }
	return 0.0;
#endif
}

double blue(int range, double j, double V, double S)
{
	double part = j / static_cast<double>(range);
	if    (0.0 <= part && part <= wvlt::colDots[0])			{ return V -V*S/2. + V*S*(part)/(wvlt::colDots[0] - 0.0)/2.; }
	if(wvlt::colDots[0] < part && part <= wvlt::colDots[1])	{ return V; }
	if(wvlt::colDots[1] < part && part <= wvlt::colDots[2])	{ return V - V*S*(part-wvlt::colDots[1])/(wvlt::colDots[2] - wvlt::colDots[1]); }
	if(wvlt::colDots[2] < part && part <= wvlt::colDots[3])	{ return V*(1.-S); }
	if(wvlt::colDots[3] < part && part <= 1.)				{ return V*(1.-S); }
	return 0.0;
#if 0
	/// old
	if    (0.000 <= part && part <= 0.167)	{ return V -V*S/2. + V*S*(part)/(0.167-0.000)/2.; }
	if(0.167 < part && part <= 0.400)		{ return V; }
	if(0.400 < part && part <= 0.500)		{ return V - V*S*(part-0.400)/(0.500-0.400)/2.; }
	if(0.500 < part && part <= 0.600)		{ return V - V*S*(part-0.400)/(0.500-0.400)/2.; }
	if(0.600 < part && part <= 0.833)		{ return V*(1.-S); }
	if(0.833 < part && part <= 1.000)		{ return V*(1.-S); }
	return 0.0;
#endif
}

QColor hueJet(int range, double j)
{
	if(j > range)	{ j = range; }	/// bicycle for no black colour
	if(j < 0)		{ j = 0; }		/// bicycle for no black colour
	//    return QColor(255.*red1(range,j), 255.*green1(range,j), 255.*blue1(range,j));
	return QColor(255. * red(range, j),
				  255. * green(range, j),
				  255. * blue(range, j));
}

QPixmap drawWavelet(const matrix & cwtMatrix)
{
	 /// magic consts in pixels
	const int wid = wvlt::timeStep;
	const int hei = 800 / cwtMatrix.rows();

	QPixmap pic(cwtMatrix.cols() * wid, cwtMatrix.rows() * hei);
	pic.fill();
	QPainter painter;
	painter.begin(&pic);

	const double mx = cwtMatrix.maxVal();
	const double mn = cwtMatrix.minVal();

	for(int i = 0; i < cwtMatrix.rows(); ++i)
	{
		for(int j = 0; j < cwtMatrix.cols(); ++j)
		{
			double numb = static_cast<double>(cwtMatrix[i][j] - mn) / (mx - mn) * wvlt::range;
			painter.setBrush(QBrush(wvlt::hueJet(wvlt::range, numb)));
			painter.setPen(wvlt::hueJet(wvlt::range, numb));

			painter.drawRect(j * wid,
							 pic.height() - hei - i * hei, /// legs upwards
							 wid, hei);
		}
	}

	/// freqs edges
	painter.setPen("black");
	painter.setPen(Qt::DashLine);
	for(int i = 0; i < cwtMatrix.rows(); ++i)
	{
		painter.drawLine(0,
						 i * hei,
						 pic.width(),
						 i * hei);
	}

	/// seconds marks
	painter.setFont(QFont("Helvetica", 18, -1, -1));
	painter.setPen(Qt::SolidLine);
	for(int i = 0; i < cwtMatrix.cols() * wvlt::timeStep / DEFS.getFreq() + 1; ++i)
	{
		painter.drawLine(i * DEFS.getFreq(),
						 pic.height(),
						 i * DEFS.getFreq(),
						 pic.height() - 20);

		painter.drawText(i * DEFS.getFreq() - 8,
						 pic.height() - 2,
						 nm(i));

	}
	return pic;
}

} /// end of namespace wvlt

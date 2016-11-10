#include "library.h"


namespace wvlt
{

bool isInit = false;



int initMtlb()
{
	if(isInit) return 1;
	auto t0 = std::chrono::high_resolution_clock::now();
	std::cout << "matlab lib load starts" << std::endl;
	if (!libcwt_rInitializeWithHandlers(NULL, 0)){
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
	if(!isInit) return 1;
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

matrix cwt(const lineType & signal, double freq)
{
	if(!isInit)
	{
		initMtlb();
	}
	const int numFreqs = 19; /// number of frequencies in cwt.m (2:1:20)

	mxArray * srate = mxCreateDoubleScalar(freq);
	mxArray * inData = mxCreateDoubleMatrix(1, signal.size(), mxREAL);
	double * it = mxGetPr(inData);
	for(auto datum : signal)
	{
		*it = datum;
		++it;
	}
	mxArray * res = mxCreateDoubleMatrix(numFreqs, signal.size(), mxCOMPLEX);

	mlfCWT(1, &res, inData, srate);
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
	mxDestroyArray(srate);

	return result;
}

void drawWaveletMtlb(const matrix & inData, QString picPath)
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
			painter.setBrush(QBrush(myLib::hueJet(wvlt::range, drawData[i][j])));
			painter.setPen(myLib::hueJet(wvlt::range, drawData[i][j]));

			painter.drawRect( j * barL,
							  pic.height() - (i + 1) * barH, // +1 'cause left UPPER
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
						 QString::number(i + 2)); /// +2 from scales [2:1:20]
	}

	painter.setPen(Qt::SolidLine);
	for(int i = 0; i < int(inData.cols() / def::freq); ++i)
	{
		painter.drawLine(i * def::freq * barL,
						 pic.height(),
						 i * def::freq * barL,
						 pic.height() - 20);
		painter.drawText(i * def::freq * barL - 8,
						 pic.height() - 2,
						 QString::number(i));

	}
	pic.save(picPath, 0, 100);
	painter.end();
}

}

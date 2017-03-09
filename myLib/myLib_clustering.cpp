#include <myLib/clustering.h>


namespace clust
{

struct clustDot
{
    int number = -1;
	std::vector<double> initCoords;
	std::vector<double> initDists;
	std::vector<double> newCoords;
	std::vector<double> newDists;
    int clustNum = -1;
};

struct cluster
{
    int number = -1;
	std::vector<int> dotNums;
	std::vector<double> centroid;
	std::vector<double> sigma;
};

struct clustering
{
    int numDots = -1;
    int dimDots = -1;
	std::vector<cluster> clusts;
	std::vector<clustDot> dots;
    mat dists;
	std::vector<bool> boundDots;
	std::vector<bool> isolDots;

    void readFile(QString filePath);
};



double countAngle(double initX, double initY)
{
	if(initX == 0.)
	{
		return ((initY > 0.) ? (M_PI/2.) : (-M_PI/2));
	}

	if(initX > 0.)
	{
		return atan(initY/initX);
	}
	else
	{
		return atan(initY/initX) + M_PI;
	}
}
/*
void clustering::readFile(QString filePath)
{

	std::vector< std::vector<double> > cData;

    ifstream inStr;
    inStr.open(filePath.toStdString().c_str());
    if(!inStr.good())
    {
		std::cout << "readFile: can't read file" << std::endl;
        return;
    }

    inStr.ignore(64, ' ');
    inStr >> numDots;

    inStr.ignore(64, ' ');
    inStr >> dimDots;

    for(int i = 0; i < numDots; ++i)
    {
        for(int j = 0; j < dimDots; ++j)
        {
            inStr >> cData[i][j];
        }
    }
	std::vector<double> temp;
    temp.resize(4);

    boundDots.resize(numDots);
    std::fill(boundDots.begin(), boundDots.end(), false);

    isolDots.resize(numDots);
    std::fill(isolDots.begin(), isolDots.end(), true);

    for(int i = 0; i < numDots; ++i)
    {
        for(int j = i+1; j < numDots; ++j)
        {
            temp[0] = distance(cData[i].data(), cData[j].data(), dimDots);
            temp[1] = i;
            temp[2] = j;
            temp[3] = 0;
            dists.push_back(temp);
        }
    }
    std::sort(dists.begin(), dists.end(), mySort);

    // make first bound
	std::vector<int>::iterator it;

    boundDots[ dists[0][1] ] = true;
    isolDots[ dists[0][1] ] = false;

    boundDots[ dists[0][2] ] = true;
    isolDots[ dists[0][2] ] = false;

    dists[0][3] = 2;
    newDists.push_back(dists[0]);

    for(int j = 0; j < 3; ++j)
    {
		std::cout << dists[0][j] << '\t';
    }
	std::cout << std::endl;


	std::vector<std::vector<double> >::iterator itt;
    while (contains(isolDots.begin(), isolDots.end(), true))
    {
        //adjust dists[i][3]
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
//            if((*iit)[3] >= 2)
//            {
//                dists.erase(iit);
////                --iit; // :-)
//            }
        }

        // set new bound ()
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

        for(int j = 0; j < 3; ++j)
        {
			std::cout << (*itt)[j] << '\t';
        }
		std::cout << std::endl;
    }
    std::sort(newDists.begin(), newDists.end(), mySort);
	std::vector<double> newD;
    for(int i = 0; i < newDists.size(); ++i)
    {
        newD.push_back(newDists[i][0]);
    }
	std::cout << newD << std::endl;

}

*/




#if 0


void refreshDist(mat & dist,
const std::vector<std::pair <double, double> > & testCoords,
const int input)
{

    // numRow * (numRow + 1) / 2 = distSize
    int size = testCoords.size();

    double helpDist = 0;
    for(int i = 0; i < size; ++i)
    {
        for(int j = i+1; j < size; ++j)
        {
            if(i != input && j != input) continue;

            helpDist = pow( pow(testCoords[i].first - testCoords[j].first, 2)
                            + pow(testCoords[i].second - testCoords[j].second, 2), 0.5);
            dist[i][j] = helpDist;
            dist[j][i] = helpDist; // unneeded?
        }
    }
}

void refreshDistAll(mat & distNew,
					const coordType & plainCoords)
{
    // numRow * (numRow + 1) / 2 = distSize

    int numRow = plainCoords.size();
	//    std::cout << "numRow = " << numRow << std::endl;



    double helpDist = 0.;
    for(int i = 0; i < numRow; ++i)
    {
        for(int j = i+1; j < numRow; ++j)
        {
            helpDist = pow( pow(plainCoords[i].first - plainCoords[j].first, 2)
                            + pow(plainCoords[i].second - plainCoords[j].second, 2), 0.5);
            distNew[i][j] = helpDist;
            distNew[j][i] = helpDist;  // unneeded?
        }
    }
}

void countGradient(const coordType & plainCoords,
				   const std::vector<std::vector<double> >  & distOld,
				   std::vector<std::vector<double> > & distNew,
				   std::vector<double> & gradient)
{
    const int size = plainCoords.size();
    const double delta = 0.1;
	std::vector<std::pair <double, double> > tempCoords = plainCoords;

    for(int i = 0; i < size; ++i)
    {
        tempCoords[i].first += delta/2.;
        refreshDist(distNew, tempCoords, i);
        gradient[2 * i] = errorSammon(distOld, distNew);

        tempCoords[i].first -= delta;
        refreshDist(distNew, tempCoords, i);
        gradient[2 * i] -= errorSammon(distOld, distNew);
        gradient[2 * i] /= delta;
        tempCoords[i].first += delta/2.;

		if(std::abs(tempCoords[i].first - plainCoords[i].first) > 1e-5)
        {
			std::cout << "coords1 changed " << plainCoords[i].first - tempCoords[i].first << std::endl;
        }

        tempCoords[i].second += delta/2.;
        refreshDist(distNew, tempCoords, i);
        gradient[2 * i + 1] = errorSammon(distOld, distNew);

        tempCoords[i].second -= delta;
        refreshDist(distNew, tempCoords, i);
        gradient[2 * i + 1] -= errorSammon(distOld, distNew);
        gradient[2 * i + 1] /= delta;
        tempCoords[i].second += delta/2.;

		if(std::abs(tempCoords[i].second - plainCoords[i].second) > 1e-5)
        {
			std::cout << "coords2 changed " << plainCoords[i].second - tempCoords[i].second << std::endl;
        }

        refreshDist(distNew, tempCoords, i);
    }
}

void moveCoordsGradient(coordType & plainCoords,
                        const mat & distOld,
                        mat & distNew)
{
    int size = plainCoords.size();

	std::vector<double> gradient;
    gradient.resize(size * 2);
    double lambda = 0.1;

    double errorBackup;

    countGradient(plainCoords, distOld, distNew, gradient);
    int numSteps = 0;
    while(1)
    {
        errorBackup = errorSammon(distOld, distNew);
        // a bit move coords
        for(int i = 0; i < size; ++i)
        {
            plainCoords[i].first -= gradient[2 * i] * lambda;
            plainCoords[i].second -= gradient[2 * i+1] * lambda;
        }
        // count new dists
        refreshDistAll(distNew, plainCoords);

        // if became worse - go back
        if(errorBackup < errorSammon(distOld, distNew))
        {
            for(int i = 0; i < size; ++i)
            {
                plainCoords[i].first += gradient[2 * i] * lambda;
                plainCoords[i].second += gradient[2 * i+1] * lambda;
            }
            refreshDistAll(distNew, plainCoords);
            break;
        }
        ++numSteps;
        if(numSteps % 5 == 4) lambda *= 2;
    }
	//    std::cout << "gradient steps = " << numSteps  << std::endl;
}

double errorSammon(const mat & distOld,
                   const mat & distNew) // square matrices
{
    int size = distOld.size();
	//    std::cout << "errorSammon size = " << size << std::endl;
    double res = 0.;
    for(int i = 0; i < size; ++i)
    {
        for(int j = i+1; j < size; ++j)
        {
            res += pow(distOld[i][j] - distNew[i][j], 2.) / pow(distOld[i][j], 2.);
        }
    }
    return res;
}
#endif


double errorSammonAdd(const mat & distOld,
                      const mat & distNew,
					  const std::vector<int> & placedDots) // square matrices
{
    double res = 0.;
    for(unsigned int i = 0; i < placedDots.size(); ++i)
    {
        for(unsigned int j = i+1; j < placedDots.size(); ++j)
        {
            res += pow(distOld[i][j] - distNew[i][j], 2.) / pow(distOld[i][j], 2.);
        }
    }
    return res;
}

void countInvHessianAddDot(const mat & distOld,
                           const mat & distNew,
						   const coordType & crds,
						   const std::vector<int> & placedDots,
                           mat & invHessian)
{
    invHessian[0][0] = 0.;
    invHessian[0][1] = 0.;
    invHessian[1][0] = 0.;
    invHessian[1][1] = 0.;

	const int b = placedDots.back();
    for(unsigned int j = 0; j < placedDots.size() - 1; ++j)
    {
		const int i = placedDots[j];
        //dydy
        invHessian[0][0] +=
                2. * (distOld[i][b] *
                      (pow(distNew[i][b], -3.) *
                       pow(crds.back().second - crds[j].second, 2.) -
                       pow(distNew[i][b], -1.)
                       )
                      + 1.)
                * pow(distOld[i][b], -2.);
        //dxdx
        invHessian[1][1] +=
                2. * (distOld[i][b] *
                      (pow(distNew[i][b], -3.) *
                       pow(crds.back().first - crds[j].first, 2.) -
                       pow(distNew[i][b], -1.)
                       )
                      + 1.)
                * pow(distOld[i][b], -2.);
        invHessian[0][1] +=
                -2. * distOld[i][b] *
                pow(distNew[i][b], -3.) *
                (crds.back().first - crds[j].first) *
                (crds.back().second - crds[j].second)
                * pow(distOld[i][b], -2.);
    }
    invHessian[1][0] = invHessian[0][1];

    double det = invHessian[1][1] * invHessian[0][0] - invHessian[1][0] * invHessian[0][1];
	//    std::cout << "det = " << det << std::endl;
    invHessian[0][0] /= det;
    invHessian[0][1] /= det;
    invHessian[1][0] /= det;
    invHessian[1][1] /= det;


}


void countGradientAddDot(const mat & distOld,
                         const mat & distNew,
						 const coordType & crds,
						 const std::vector<int> & placedDots,
						 std::vector<double>  & gradient) // gradient for one dot
{
	const int b = placedDots.back();
    gradient[0] = 0.;
    gradient[1] = 0.;
    for(unsigned int j = 0; j < placedDots.size() - 1; ++j)
    {

		const int i = placedDots[j];
        gradient[0] +=
                2. * (1. - distOld[i][b] /
                      distNew[i][b]) *
                (crds.back().first - crds[j].first)
                * pow(distOld[i][b], -2.);
        gradient[1] +=
                2. * (1. - distOld[i][b] /
                      distNew[i][b]) *
                (crds.back().second - crds[j].second)
                * pow(distOld[i][b], -2.);
    }

}

void countDistNewAdd(mat & distNew, // change only last coloumn
					 const coordType & crds,
					 const std::vector<int> & placedDots)
{
	const int b = placedDots.back(); // placedDots[placedDots.size() - 1];
    for(unsigned int i = 0; i < placedDots.size() - 1; ++i)
    {
		const int a = placedDots[i];
        distNew[a][b] = pow(pow(crds[i].first  - crds.back().first , 2) +
                            pow(crds[i].second - crds.back().second, 2),
                            0.5);
        distNew[b][a] = distNew[a][b];
    }

}

void sammonAddDot(const mat & distOld,
                  mat & distNew, // change only last coloumn
				  coordType & plainCoords,
				  const std::vector<int> & placedDots)
{
    const int addNum = placedDots.size() - 1;
    // set initial place
    double helpX = 0.;
    double helpY = 0.;
    double sumW = 0.;
    double currW = 0.;
    for(int i = 0; i < addNum; ++i)
    {
        currW = pow(distOld[placedDots[i]][placedDots.back()], -2.);
        sumW += currW;
        helpX += plainCoords[i].first  * currW;
        helpY += plainCoords[i].second * currW;
    }
    helpX /= sumW;
    helpY /= sumW;

	plainCoords.push_back(std::make_pair(helpX, helpY));

	//std::cout all the dots



    // gradien descent
	std::vector<double> gradient;
    gradient.resize(2);
    mat invHessian; // matrix 2x2
    invHessian.resize(2);
    invHessian[0].resize(2);
    invHessian[1].resize(2);

    double tmpError1 = 0.;
    double tmpError2 = 0.;

    // count initial error
    countDistNewAdd(distNew,
                    plainCoords,
                    placedDots);
    tmpError2 = errorSammonAdd(distOld,
                               distNew,
                               placedDots);

    double lambda = 0.2;
    int iterationsCount = 0;
    double deltaX = 0.;
    double deltaY = 0.;





    while(1)
    {
        tmpError1 = tmpError2;
        countGradientAddDot(distOld,
                            distNew,
                            plainCoords,
                            placedDots,
                            gradient);
        if(1)
        {
            countInvHessianAddDot(distOld,
                                  distNew,
                                  plainCoords,
                                  placedDots,
                                  invHessian);
        }
        deltaX = lambda * (invHessian[0][0] * gradient[0] + invHessian[0][1] * gradient[1]);
        deltaY = lambda * (invHessian[1][0] * gradient[0] + invHessian[1][1] * gradient[1]);

        plainCoords[addNum].first  -= deltaX;
        plainCoords[addNum].second -= deltaY;

        countDistNewAdd(distNew,
                        plainCoords,
                        placedDots);

        tmpError2 = errorSammonAdd(distOld,
                                   distNew,
                                   placedDots);

        ++iterationsCount;

        if(tmpError2 < 1e-10
		   || (std::abs(tmpError1 - tmpError2) / tmpError1) < 1e-6
           || iterationsCount > 100) break;
    }
	//    std::cout << "NewDot = " << plainCoords[addNum].first << '\t' << plainCoords[addNum].second << std::endl;
	//    std::cout << "NumOfIterations addDot = " << iterationsCount << " error = " << tmpError2 << std::endl;

    //    if(addNum == 4) exit(1);
}

void sammonProj(const mat & distOld,
				const std::vector<int> & types,
                const QString & picPath)
{
    srand(time(NULL));
    int size = distOld.size();

    mat distNew; // use only higher triangle
    distNew.resize(size);
    for(int i = 0; i < size; ++i)
    {
        distNew[i].resize(size);
    }

	std::vector< std::pair <double, double> > plainCoords;
    //    plainCoords.resize(size);

    // find three most distant points
    // precise
    int num1 = -1;
    int num2 = -1;
    int num3 = -1;
	std::vector<int> placedDots;
    double maxDist = 0.;

    for(int i = 0; i < size; ++i)
    {
        for(int j = i+1; j < size; ++j)
        {
            if(distOld[i][j] > maxDist)
            {
                maxDist = distOld[i][j];
                num1 = i;
                num2 = j;
            }
        }
    }
	//    std::cout << "maxDist = " << maxDist << std::endl;
	plainCoords.push_back(std::make_pair(0., 0.));
	plainCoords.push_back(std::make_pair(maxDist, 0.));
    maxDist = 0.;

    for(int i = 0; i < size; ++i)
    {
        if(i == num1 || i == num2) continue;
        if(fmin(distOld[i][num1], distOld[i][num2]) > maxDist)
        {
            maxDist = fmin(distOld[i][num1], distOld[i][num2]);
            num3 = i;
        }
    }
	//    std::cout << "maxDist = " << maxDist << std::endl;
    //count third dot coords
    double tm = (pow(distOld[num1][num3], 2.) +
                 pow(distOld[num1][num1], 2.) -
                 pow(distOld[num2][num3], 2.)
                 ) * 0.5 / distOld[num1][num2];
	//    std::cout << "tm = " << tm << std::endl;
	plainCoords.push_back(std::make_pair(tm,
                                    pow( pow(distOld[num1][num3], 2.) -
                                         pow(tm, 2.),
                                         0.5)
                                    )
                          );
    placedDots.push_back(num1);
    placedDots.push_back(num2);
    placedDots.push_back(num3);

    for(int i = 0; i < 3; ++i)
    {
        distNew[placedDots[i]][placedDots[(i+1)%3]] = distOld[placedDots[i]][placedDots[(i+1)%3]];
        distNew[placedDots[(i+1)%3]][placedDots[i]] = distNew[placedDots[i]][placedDots[(i+1)%3]];
    }

	//    std::cout << distNew << std::endl;


    double helpDist = 0.;
    for(int addNum = 3; addNum < size; ++addNum)
    {
        maxDist = 0.;
        //search max distant dot
        for(int i = 0; i < size; ++i)
        {
            if(std::find(placedDots.begin(),
                         placedDots.end(),
                         i) != placedDots.end()) {continue;}

            helpDist = distOld[placedDots[0]][placedDots[1]];
            for(uint j = 0; j > placedDots.size(); ++j)
            {
                helpDist = fmin(helpDist, distOld[i][placedDots[j]]);
            }

            if(helpDist > maxDist)
            {
                maxDist = helpDist;
                num3 = i;
            }
        }
		//        std::cout << "newDotNum = " << num3 << std::endl;

        //place this dot
        placedDots.push_back(num3);
        sammonAddDot(distOld, distNew, plainCoords, placedDots);
        if(addNum >= int(sqrt(size)))
        {
            ////TODO
            //            adjustSkeletonDots(distOld, distNew, plainCoords, placedDots);
        }
    }
	//    std::cout << distNew << std::endl;

    for(int i = 0; i < size; ++i)
    {
		std::cout << "dot[" << i << "] = "
				  << plainCoords[i].first << '\t' << plainCoords[i].second << std::endl;
    }
	std::cout << std::endl;


    drawSammon(plainCoords, types, picPath);
    QString helpString = picPath;
    helpString.replace(".jpg", "_.jpg");
    drawShepard(distOld, distNew, helpString);
}


void drawSammon(const coordType & plainCoords,
				const std::vector<int> & types,
				const QString & picPath) //uses coords array
{

	const int NumberOfVectors = plainCoords.size();
	//draw the points
	QPixmap pic(1200, 1200);
	pic.fill();
	QPainter painter;
	painter.begin(&pic);

	painter.setPen("black");
	painter.drawLine(QPointF(0, pic.height()/2.),
					 QPointF(pic.width(), pic.height()/2.));
	painter.drawLine(QPointF(pic.width()/2., 0),
					 QPointF(pic.width()/2, pic.height()));

	double minX = 0., minY = 0., maxX = 0., maxY = 0., avX, avY, rangeX, rangeY;
	const double rectSize = 4;

	double sum1 = 0., sum2 = 0.;

	minX = plainCoords.front().first;
	minY = plainCoords.front().second;
	for(int i = 0; i < NumberOfVectors; ++i)
	{
		maxX = fmax(maxX, plainCoords[i].first);
		maxY = fmax(maxY, plainCoords[i].second);

		minX = fmin(minX, plainCoords[i].first);
		minY = fmin(minY, plainCoords[i].second);
	}
	avX = (minX + maxX)/2.;
	avY = (minY + maxY)/2.;

	rangeX = (maxX - minX)/2.;
	rangeY = (maxY - minY)/2.;

	rangeX *= 1.02;
	rangeY *= 1.02;

	double range = fmax(rangeX, rangeY);

	double initX = 0.;
	double initY = 0.;
	double leng = 0.;
	double angle = 0.;
	double drawX = 0.;
	double drawY = 0.;

	// count half of points for right angle
	double sumAngle1 = 0.;
	double sumAngle2 = 0.;
	double maxLeng = 0;
	for(int i = 0; i < NumberOfVectors; ++i)
	{
		sum1 = plainCoords[i].first;
		sum2 = plainCoords[i].second;

		// relative coordinates (centroid)
		initX = sum1 - avX;
		initY = sum2 - avY;

		leng = pow(pow(initX, 2.) + pow(initY, 2.), 0.5);
		maxLeng = fmax(leng, maxLeng);

		angle = countAngle(initX, initY);

		if(i < NumberOfVectors/2)
		{
			sumAngle1 += angle;
		}
		if(i >= NumberOfVectors/4 && i < NumberOfVectors*3/4)
		{
			sumAngle2 += angle;
		}
	}
	sumAngle1 /= (NumberOfVectors/2);
	sumAngle2 /= (NumberOfVectors/2);
	range = maxLeng * 1.02;

	int mirror = 1;
	if(cos(sumAngle1) * sin(sumAngle2) - cos(sumAngle2) * sin(sumAngle1) < 0.)
	{
		mirror = -1;
	}

	int pew = 0;
	for(int i = 0; i < NumberOfVectors; ++i)
	{
		sum1 = plainCoords[i].first;
		sum2 = plainCoords[i].second;

		if( i<3 )
		{
			painter.setBrush(QBrush("red"));
			painter.setPen("red");

		}
		else if(0)
		{
			switch(types[i])
			{
			case 0:
			{
				painter.setBrush(QBrush("blue"));
				painter.setPen("blue");
				break;
			}
			case 1:
			{
				painter.setBrush(QBrush("red"));
				painter.setPen("red");
				break;
			}
			case 2:
			{
				painter.setBrush(QBrush("green"));
				painter.setPen("green");
				break;
			}
			default:
			{
				painter.setBrush(QBrush("black"));
				painter.setPen("black");
				break;
			}
			}
		}
		else
		{
			//            painter.setBrush(QBrush("red"));
			//            painter.setPen("red");

			pew = int(255.*i/NumberOfVectors);
			painter.setBrush(QBrush(QColor(pew,0,pew)));
			painter.setPen(QColor(pew,0,pew));
		}





		initX = sum1 - avX;
		initY = -(sum2 - avY);

		leng = pow(pow(initX, 2.) + pow(initY, 2.), 0.5);
		angle = countAngle(initX, initY);

		angle -= sumAngle1;
		//        initX = leng * cos(angle);
		//        initY = leng * sin(angle);
		mirror = 1;


		drawX = pic.width()  * 0.5 * (1. + (initX / range));
		drawY = pic.height() * 0.5 * (1. + (initY / range) * mirror);

		//        std::cout << drawX << '\t' << drawY << std::endl;

		painter.drawRect(QRectF(QPointF(drawX - rectSize,
										drawY - rectSize),
								QPointF(drawX + rectSize,
										drawY + rectSize)
								)
						 );

	}
	pic.save(picPath, 0, 100);
	painter.end();
	std::cout << "Sammon projection done" << std::endl;
}

void drawShepard(const mat & distOld,
				 const mat & distNew,
				 const QString & picPath)
{
	//    std::cout << distNew << std::endl;
	const int num = distOld.size();
	//draw the points
	QPixmap pic(1200, 1200);
	pic.fill();
	QPainter painter;
	painter.begin(&pic);

	painter.setPen("black");
	painter.setBrush(QBrush("black"));
	painter.drawLine(QPointF(pic.width() * 0.1, pic.height() * 0.9),
					 QPointF(pic.width() * 1.0, pic.height() * 0.9)
					 );
	painter.drawLine(QPointF(pic.width() * 0.1, pic.height() * 0.9),
					 QPointF(pic.width() * 0.1, pic.height() * 0.0)
					 );
	painter.drawLine(QPointF(pic.width() * 0.1, pic.height() * 0.9),
					 QPointF(pic.width() * 1.0, pic.height() * 0.0)
					 );

	double drawX = 0.;
	double drawY = 0.;
	double maxDistOld = 0.;
	double maxDistNew = 0.;
	for(int i = 0; i < num; ++i)
	{
		for(int j = i+1; j < num; ++j)
		{
			maxDistOld = fmax(distOld[i][j], maxDistOld);
			maxDistNew = fmax(distNew[i][j], maxDistNew);
		}
	}
	maxDistNew *= 1.02;
	maxDistOld *= 1.02;

	const int rectSize = 4;
	for(int i = 0; i < num; ++i)
	{
		for(int j = i+1; j < num; ++j)
		{
			drawX = pic.width()  * (0.1 + 0.9 * distOld[i][j] / maxDistOld);
			drawY = pic.height() * (0.9 - 0.9 * distNew[i][j] / maxDistNew);

			painter.drawRect(QRectF(QPoint(drawX - rectSize, drawY - rectSize),
									QPoint(drawX + rectSize, drawY + rectSize)
									)
							 );
		}
	}
	painter.end();
	pic.save(picPath, 0, 100);

}

}

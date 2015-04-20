#ifndef CLUSTERING_H
#define CLUSTERING_H

#include "library.h"

struct clustDot
{
    int number = -1;
    vector < double > initCoords;
    vector < double > initDists;
    vector < double > newCoords;
    vector < double > newDists;
    int clustNum = -1;
};

struct cluster
{
    int number = -1;
//    vector <clustDot> dots;
    vector <int> dotNums;
    vector <double> centroid;
    vector <double> sigma;
};

struct clustering
{
    int numDots = -1;
    int dimDots = -1;
    vector <cluster> clusts;
    vector <clustDot> dots;
    vector < vector<double> > dists;
    vector <bool> boundDots;
    vector <bool> isolDots;

    void readFile(QString filePath);
};

#endif // CLUSTERING_H

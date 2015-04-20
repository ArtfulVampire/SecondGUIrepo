#include "clustering.h"

/*
void clustering::readFile(QString filePath)
{

    vector < vector <double> > cData;

    ifstream inStr;
    inStr.open(filePath.toStdString().c_str());
    if(!inStr.good())
    {
        cout << "readFile: can't read file" << endl;
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
    vector <double> temp;
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
        cout << dists[0][j] << '\t';
    }
    cout << endl;


    vector<vector<double> >::iterator itt;
    while (contains(isolDots.begin(), isolDots.end(), true))
    {
        //adjust dists[i][3]
        for(vector<vector<double> >::iterator iit = dists.begin();
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
            cout << (*itt)[j] << '\t';
        }
        cout << endl;
    }
    std::sort(newDists.begin(), newDists.end(), mySort);
    vector <double> newD;
    for(int i = 0; i < newDists.size(); ++i)
    {
        newD.push_back(newDists[i][0]);
    }
    cout << newD << endl;

}

*/

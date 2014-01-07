#include <draw.h>

void draw(QPaintDevice * dev, int numOfVectors, int * spL, double ** arr, int spLength, double spStep, QString savePath, double uiScaling)
{
    QPainter * paint = new QPainter();
    QSvgGenerator svgGen;
    QPixmap pic;
    double norm;
    int helpInt;
    QString helpString;

    cout<<"devType = "<<dev->devType()<<endl;

    if(1)
    {
        svgGen.setSize(QSize(1600, 1600));
        svgGen.setViewBox(QRect(QPoint(0,0), svgGen.size()));
        svgGen.setFileName(savePath);
        paint->begin(&svgGen);
        paint->setBrush(QBrush("white"));
        paint->drawRect(QRect(QPoint(0,0), svgGen.size()));
    }
    else
    {
        pic = QPixmap(1600,1600);
        pic.fill();
        paint->begin(&pic);
    }

    //finding maximum magnitude
    norm = 0.;
    for(int j=0; j<numOfVectors; ++j)
    {
        for(int c2=0; c2<19; ++c2)
        {
            for(int k=0; k<250-1; ++k)
            {
                norm = max(norm, arr[j][c2][int(k*spL[j]/250.)]);
            }
        }
    }
    cout<<"max magnitude = "<<norm<<endl;
    norm = 250./norm; //250 - pixels per graph, generality
//    cout<<"norm = "<<norm<<endl;
    norm *= uiScaling;


//    cout<<"prep spectra draw"<<endl;
    for(int c2=0; c2<19; ++c2)  //exept markers channel
    {
        //draw spectra
        for(int k=0; k<250-1; ++k)
        {
            for(int j=0; j<numOfVectors; ++j)
            {
                if(j==0) paint->setPen(QPen(QBrush("blue"), 2));
                if(j==1) paint->setPen(QPen(QBrush("red"), 2));
                if(j==2) paint->setPen(QPen(QBrush("green"), 2));
                paint->drawLine(QPointF(coords::x[c2]+k, coords::y[c2] - arr[j][c2][int(k*spL[j]/250.)]*norm), QPointF(coords::x[c2]+k+1, coords::y[c2] - arr[j][c2][int((k+1)*spL[j]/250.)]*norm));
            }
        }

        //draw axes
        paint->setPen("black");
        paint->drawLine(QPointF(coords::x[c2], coords::y[c2]), QPointF(coords::x[c2], coords::y[c2]-250));
        paint->drawLine(QPointF(coords::x[c2], coords::y[c2]), QPointF(coords::x[c2]+250, coords::y[c2]));

        //draw Herzes
        paint->setFont(QFont("Helvitica", 8));
        for(int k=0; k<250-1; ++k) //for every Hz generality
        {
            if( (left + k*(spLength)/250.)*spStep - floor((left + k*(spLength)/250.)*spStep) < spLength/250.*spStep/2. || ceil((left + k*(spLength)/250.)*spStep) - (left + k*(spLength)/250.)*spStep < spLength/250.*spStep/2.)  //why spLength - generality
            {
                paint->drawLine(QPointF(coords::x[c2] + k, coords::y[c2]), QPointF(coords::x[c2] + k, coords::y[c2]+5));

                helpInt = int((left + k*(spLength)/250.)*spStep + 0.5);
                helpString.setNum(helpInt);
                if(helpInt<10)
                {
                    paint->drawText(QPointF(coords::x[c2] + k-3, coords::y[c2]+15), helpString);  //-3 getFont->size
                }
                else
                {
                    paint->drawText(QPointF(coords::x[c2] + k-5, coords::y[c2]+15), helpString);  //-5 getFont->size
                }
            }
        }

    }
//    cout<<"spectra drawn"<<endl;

    //write channels labels
    paint->setFont(QFont("Helvetica", 24, -1, false));
    for(int c2=0; c2<19; ++c2)  //exept markers channel
    {
        paint->drawText(QPointF((coords::x[c2]-20), (coords::y[c2]-252)), QString(coords::lbl[c2]));
    }

    //draw scale
    paint->drawLine(QPointF(coords::x[6], coords::y[1]), QPointF(coords::x[6], coords::y[1] - 250.));  //250 - graph height generality

    //returning norm = max magnitude
    norm /= uiScaling;
    norm = 250. / norm;
    norm = int(norm*10.)/10.;

    if(0)
    {
        helpString.setNum(norm);
        helpString.append(" mcV^2/Hz");
        paint->drawText(QPointF(coords::x[6]+5., coords::y[1] - 250./2.), helpString);
    }

    if(!1)
    {
        pic.save(savePath, 0, 100);
    }
    paint->end();



    delete paint;
}

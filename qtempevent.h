#ifndef QTEMPEVENT_H
#define QTEMPEVENT_H

#include <QEvent>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <QStringList>
#include <QString>

class QTempEvent : public QEvent
{
public:
    QTempEvent() : QEvent((Type)1200)
    {
    }

    QString info()
    {
        return "Temperature Event";
    }

    double value()
    {
        char * a = new char [50];
        int heat1, heat2;
        QStringList lst;
        QString helpString;
//        system("acpi -t");
//        helpString = QString(getline(cin))

        FILE * file = fopen("/media/Files/Data/temp.txt", "w");
        fclose(file);
        helpString = "acpi -t >> /media/Files/Data/temp.txt";
        system(helpString.toStdString().c_str());

        file = fopen("/media/Files/Data/temp.txt", "r");

        fgets(a, 50, file);
        helpString = QString(a);
        lst = helpString.split(" ");
        for(int j=0; j<lst.length(); ++j)
        {
            if(lst[j]=="degrees") heat1 = lst[j-1].toDouble();
        }

        fgets(a, 50, file);
        helpString = QString(a);
        lst = helpString.split(" ");
        for(int j=0; j<lst.length(); ++j)
        {
            if(lst[j]=="degrees") heat2 = lst[j-1].toDouble();
        }
        temp = fmax(heat1,heat2);


        delete []a;
        return fmax(heat1,heat2);
    }

private:
    double temp;
};

#endif // QTEMPEVENT_H

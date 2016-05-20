#ifndef TEMPTHREAD_H
#define TEMPTHREAD_H

#include <QThread>
#include <QTimer>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <QStringList>
#include <QString>
#include <iostream>

class TempThread : public QThread
{
    Q_OBJECT
public:
    explicit TempThread(QObject *parent = 0);
    void run()
    {
        QTimer timer;
        connect(&timer, SIGNAL(timeout()), SLOT(checkTemp()));
        timer.start(10000);
        exec();
    }

signals:


public slots:
    void checkTemp()
    {
        char * a = new char [50];
        int heat1, heat2;
        QStringList lst;
        QString helpString;
        double temp;
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

        if(temp>=85.)
        {
            std::cout << "CRITICAL temp = " << temp << std::endl;
            sleep(3);
            checkTemp();
        }
        else
        {
            std::cout << "current temp = " << temp << std::endl;
        }
    }

};
#endif // TEMPTHREAD_H

#include <QtGui>
#include <QApplication>
#include <widgets/mainwindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	DEFS.setUser(username::MichaelA); /// insert your username:: and/or add DEFS.setDir(pathToEdfDir)

    MainWindow w;
    w.show();

    return a.exec();
}

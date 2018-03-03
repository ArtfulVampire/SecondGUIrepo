#include <QtGui>
#include <QApplication>
#include <widgets/mainwindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	/// set your user
	DEFS.setUser(username::MichaelA);
	DEFS.setAutosUser(autosUser::Galya);

    MainWindow w;
    w.show();

    return a.exec();
}

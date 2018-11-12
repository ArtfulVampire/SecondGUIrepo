#include <QtGui>
#include <QApplication>
#include <widgets/mainwindow.h>
#include <other/defs.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	/// set your user
	DEFS.setUser(username::MichaelB);
	if(argc > 1) { DEFS.setDir(QString(argv[1])); }
//	DEFS.setDir("c:/EEG");

	MainWindow w;
	w.show();

	return a.exec();
}

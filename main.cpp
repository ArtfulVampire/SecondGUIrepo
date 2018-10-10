#include <QtGui>
#include <QApplication>
#include <widgets/mainwindow.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	/// set your user
//	DEFS.setUser(username::IITP);
//	DEFS.setUser(username::ElenaC);
	DEFS.setUser(username::MichaelA);
	DEFS.setAutosUser(autosUser::Galya);

//	QFontDatabase::addApplicationFont("Ubuntu-R.ttf");
//	qApp->setFont(QFont("Ubuntu", 12, QFont::Normal, false));

	MainWindow w;
	w.show();

	return a.exec();
}

#ifndef SPECTRE_H
#define SPECTRE_H

#include <other/matrix.h>

#include <QWidget>
#include <QString>

#define SPECTRA_EXP_NAME_SPECIFICITY 0

namespace Ui {
    class Spectre;
}

class Spectre : public QWidget
{
    Q_OBJECT

public:
    explicit Spectre();
    ~Spectre();
    void setSmooth(int);
    bool eventFilter(QObject *obj, QEvent *event);
    void defaultState();
    void setPow(double);
    void setFftLength(int);

	void writeSpectra(const std::vector<int> & chanList,
					  const double leftFreq,
					  const double rightFreq);

	std::vector<int> countSpectra(std::vector<int> chanList = std::vector<int>{});

    void cleanSpectra();


public slots:
    void countSpectraSlot();
    void inputDirSlot();
    void outputDirSlot();
    void setFftLengthSlot();

    void setLeft();
    void setRight();
    void drawWavelets();
    void compare();
    void psaSlot();
	void integrate();	/// to deprecate
	void center();		/// to deprecate - already in ClassifierData::z_transform()

private:
    Ui::Spectre * ui;

	QString backupDirPath{};
	QString defaultInPath{};
	QString defaultOutPath{};
	QString defaultInPathW{};
	QString defaultOutPathW{};

	std::vector<std::pair<int, int>> rangeLimits{};

	std::vector<matrix> dataFFT{};
	std::vector<QString> fileNames{};
};

#endif /// SPECTRE_H

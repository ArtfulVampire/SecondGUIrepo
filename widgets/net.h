#ifndef NET_H
#define NET_H

#include <iostream>
#include <cmath>
#include <fstream>
#include <ios>
#include <vector>

#include <classifier/classifier.h>
#include <classifier/classifierdata.h>
#include <other/feedback.h>

#include <QFileDialog>
#include <QButtonGroup>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QTime>
#include <QWidget>
#include <QMessageBox>



namespace Ui {
    class Net;
}

class Net : public QWidget
{
    Q_OBJECT

private:
    Ui::Net * ui;

    /// ui things
    std::vector<QButtonGroup *> myButtonGroup;
    bool stopFlag = false;
    bool autoFlag = false;

    /// ui private methods
    void aaDefaultSettings();

    /// data
    enum class myMode {N_fold, k_fold, train_test,  half_half};
    enum class source {winds, reals, pca, bayes};
    myMode Mode = myMode::N_fold;
    source Source = source::reals;

    /// classification
	Classifier * myModel = nullptr;
	ClassifierData myClassifierData{};

	/// succesiive
	std::valarray<int> passed{};
    matrix pcaMat{};
	void successiveLearning(const std::valarray<double> & newSpectre,
							const uint newType,
							const QString & newFileName);
	void successiveLearningFinal(const matrix & newSpectra,
								 const int newType);


    /// data
    std::pair<std::vector<uint>, std::vector<uint>> makeIndicesSetsCross(
            const std::vector<std::vector<uint> > & arr,
            const int numOfFold);

    QString filesPath;

	/// class - deprecated, moved to Classifier
    void crossClassification();
    void leaveOneOutClassification();
    void halfHalfClassification();
    void trainTestClassification(const QString & trainTemplate = "_train",
                                 const QString & testTemplate = "_test");
    void cycleParams(std::vector<std::vector<double>> & in); /// for customF

public:
    explicit Net();
    ~Net();


    void customF();

    /// classification
	Classifier::avType autoClassification(const QString & spectraDir = QString());

    /// NEED CHECK
    void successiveProcessing();
	Classifier::avType successiveByEDF(const QString & edfPath1, const QString & ansPath1,
									   const QString & edfPath2, const QString & ansPath2);
	Classifier::avType successiveByEDFfinal(const QString & edfPath1,
											const QString & ansPath1,
											const QString & edfPath2,
											const QString & ansPath2);
	void successivePreclean(const QString & spectraPath,
							const QStringList & filters = {"*_train*"});

    /// setsgets
	void setErrCrit(double in);
    void setLrate(double in);
	double getLrate() const;
    void setNumOfPairs(int num);
    void setFold(int in);
    void setAutoProcessingFlag(bool);
	void setMode(const QString & in = "N-fold"); /// make myMode
	void setSource(const QString & in = "reals"); /// make Source
    void setClassifier(const QString &);
	void setClassifier(ModelType);
	const Classifier & getClassifier() const { return *myModel; }
	const ClassifierData & getClassifierData() const { return myClassifierData; }

	/// data
    void loadData(const QString & spectraPath = DEFS.dirPath()
												+ "/SpectraSmooth",
				  const QStringList & filters = {"*.psd"});
	void loadDataUCI(const QString & setName);
	void loadDataXenia(const QString & filesPath = DEFS.dirPath() + "/Xenia_tables",
					   const QString & type = "bd_new");


    /// wts
    void readWtsByName(const QString & fileName,
                       twovector<std::valarray<double>> * wtsMatrix = nullptr);
    void writeWts(const QString & wtsPath = QString());
    void drawWts(QString wtsPath = QString(),
				 QString picPath = QString()) const;

public slots:
	void loadDataSlot();
    void pca();

	void autoClassificationSimple();			/// on SpectraSmooth + Source

    void setSourceSlot(QAbstractButton*);
    void setModeSlot(QAbstractButton*, bool);
    void setClassifier(QAbstractButton*, bool);


	/// ANN
    void setDimensionalitySlot();

	void readWtsSlot();
    void drawWtsSlot();
    void writeWtsSlot();
};

#endif // NET_H

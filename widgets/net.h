#ifndef NET_H
#define NET_H

#include <iostream>
#include <cmath>
#include <vector>

#include <classifier/classifier.h>
#include <classifier/classifierdata.h>
#include <other/feedback.h>
#include <other/defs.h>

#include <QWidget>
#include <QPixmap>
#include <QAbstractButton>
#include <QButtonGroup>

namespace Ui {
    class Net;
}

class Net : public QWidget
{
    Q_OBJECT

public:
    explicit Net();
    ~Net();


    void customF();

    /// classification
	Classifier::avType autoClassification(const QString & spectraDir = QString());

    /// NEED CHECK
    void successiveProcessing();
	/// to compare FeedbackNew and FeedbackFinal
	Classifier::avType successiveByEDFnew(const fb::FBedf & file1,
										  const fb::FBedf & file2);
	Classifier::avType successiveByEDFnew(const QString & edfPath,
										  const QString & ansPath1,
										  const QString & edfPath2,
										  const QString & ansPath2);
	Classifier::avType successiveByEDFfinal(const fb::FBedf & file1,
											const fb::FBedf & file2);

	Classifier::avType successiveByEDFfinal(const QString & edfPath1,
											const QString & ansPath1,
											const QString & edfPath2,
											const QString & ansPath2);

	Classifier::avType notSuccessive(const fb::FBedf & file1,
									 const fb::FBedf & file2);

	using sucAllType = std::tuple<Classifier::avType, Classifier::avType, Classifier::avType>;
	sucAllType successiveByEDFall(const fb::FBedf & file1,
								  const fb::FBedf & file2);


	void successivePreclean(const QString & spectraPath,
							const QStringList & filters = {"*_train*"}); /////////////////
	void innerClassHistogram(const fb::FBedf & file1, fb::taskType typ, fb::ansType howSolved);

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
	void loadData(const QString & spectraPath = DEFS.dirPath() + "/SpectraSmooth",
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


private:
	Ui::Net * ui;

	/// ui things
	std::vector<QButtonGroup*> myButtonGroup;
	bool stopFlag = false;
	bool autoFlag = false; /// to deprecate

	/// ui private methods
	void aaDefaultSettings();
	void cycleParams(std::vector<std::vector<double>> & in); /// to deprecate

	/// data
	enum class myMode {N_fold, k_fold, train_test,  half_half, people};
	enum class source {winds, reals, pca, bayes};
	myMode Mode{myMode::N_fold};
	source Source{source::reals};

	/// classification
	Classifier * myModel = nullptr;
	ClassifierData myClassifierData{};

	/// succesiive
	std::valarray<int> passed{};
	matrix pcaMat{}; /// ???
	void successiveLearning(const std::valarray<double> & newSpectre,
							const uint newType,
							const QString & newFileName);
	void successiveLearningFinal(const matrix & newSpectra,
								 const int newType,
								 const QString & currExpName);


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

};

#endif /// NET_H

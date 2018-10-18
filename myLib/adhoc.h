#ifndef ADHOC_H
#define ADHOC_H

#include <vector>

#include <QString>

namespace myLib
{

/// some useful functions mainly for mainwindow_customF.cpp
std::vector<QString> readBurdenkoLog(const QString & logPath);

std::vector<std::pair<int, int>> handleBurdenkoAnnots(const std::vector<QString> & annotations,
													  double srate);
void burdenkoAnnotsSimpler();
void burdenkoStuff();
void burdenkoReactionTime();
void burdenkoReadEdfPlus();
void burdenkoReadEdf();
void burdenkoEdfPlusToEdf();


std::vector<QString> composeMarkersList(const QString & workPath); /// really unused


void elenaCalculation(const QString & realsPath,
					  const QString & outSpectraPath,
					  const QString & outTableDir);

void XeniaPretable();
void SettingsXenia();
void XeniaFinalest();
void XeniaFinal();

void SettingsGalya();
void GalyaProcessing(const QString & addPath);

void feedbackFinal();

void BaklushevHistograms();
void matiConcat();
void testEgiChns128();

void checkFeedbackFiles();
void binomialToFile();
void binomialThresholds();
void drawFftWindows();
void drawEnvelope();
void HiguchiTests();

} /// end namespace myLib

#endif // ADHOC_H

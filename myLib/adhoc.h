#ifndef ADHOC_H
#define ADHOC_H

#include <vector>

#include <QString>

namespace myLib
{

/// some useful functions mainly for mainwindow_customF.cpp
std::vector<QString> readBurdenkoLog(const QString & logPath);

std::vector<std::pair<int, int>> handleAnnots(const std::vector<QString> & annotations, double srate);


std::vector<QString> composeMarkersList(const QString workPath); /// really unused

void XeniaPretable();
void XeniaFinalest();
void GalyaProcessing();

void BaklushevHistograms();
void matiConcat();
void testEgiChns128();
void burdenkoStuff();

void checkFeedbackFiles();
void binomialToFile();
void binomialThresholds();
void drawFftWindows();
void drawEnvelope();
void HiguchiTests();

} /// end namespace myLib

#endif // ADHOC_H

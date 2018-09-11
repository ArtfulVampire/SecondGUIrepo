#ifndef ADHOC_H
#define ADHOC_H

#include <vector>

#include <QString>

namespace myLib
{

/// some useful functions mainly for mainwindow_customF.cpp
std::vector<QString> readBurdenkoLog(const QString & logPath);

std::vector<std::pair<int, int>> handleAnnots(const std::vector<QString> & annotations, double srate);

} /// end namespace myLib

#endif // ADHOC_H

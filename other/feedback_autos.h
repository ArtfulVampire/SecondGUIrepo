#ifndef FEEDBACK_AUTOS_H
#define FEEDBACK_AUTOS_H

#include <map>
#include <vector>

#include <other/feedback.h>

#include <QString>

namespace fb
{
/// copy from net.h
using sucAllType = std::tuple<Classifier::avType, Classifier::avType, Classifier::avType>;

std::map<QString, QString> coutAllFeatures(const QString & guysPath,
					 const std::vector<std::pair<QString, QString>> & guysList,
					 const QString & postfix);
void calculateICA(const QString & guysPath,
				  const std::vector<std::pair<QString, QString>> & guysList,
				  const QString & postfix);

std::vector<sucAllType> calculateSuccessiveBoth(const QString & guysPath,
												const std::vector<std::pair<QString, QString>> & guysList,
												const QString & postfix);

void createAnsFiles(const QString & guyPath, const QString & guyName);
void checkMarkFBfinal(const QString & filePath);
void repairMarkersInNewFB(QString edfPath, int numSession);
void successiveNetPrecleanWinds(const QString & windsPath);

} /// end namespace fb

#endif // FEEDBACK_AUTOS_H

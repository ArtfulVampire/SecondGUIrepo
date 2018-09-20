#ifndef FEEDBACK_AUTOS_H
#define FEEDBACK_AUTOS_H
#include <map>
#include <vector>
#include <QString>

#include <other/feedback.h>
#include <widgets/net.h>

namespace fb
{

std::map<QString, QString> coutAllFeatures(const QString & dear,
					 const std::vector<std::pair<QString, QString>> & guysList,
					 const QString & postfix);
void calculateICA(const QString & dear,
				  const std::vector<std::pair<QString, QString>> & guysList,
				  const QString & postfix);

std::vector<Net::sucAllType> calculateSuccessiveBoth(const QString & dear,
													 const std::vector<std::pair<QString, QString>> & guysList,
													 const QString & postfix);

void createAnsFiles(const QString & guyPath, const QString & guyName);
void checkMarkFBfinal(const QString & filePath);
void repairMarkersInNewFB(QString edfPath, int numSession);
void successiveNetPrecleanWinds(const QString & windsPath);

} /// end namespace fb

#endif // FEEDBACK_AUTOS_H

#ifndef COORDS_H
#define COORDS_H

#include <vector>
#include <map>

#include <QString>

namespace coords
{
extern const double scale;
extern const int numOfChan;
extern const std::vector<double> x;
extern const std::vector<double> y;
extern const std::vector<QString> lbl_A1;
extern const std::vector<QString> lbl_A2;
extern const std::vector<QString> lbl19_with_ears;
extern const std::vector<QString> lbl19;
extern const std::vector<QString> lbl16noz;
extern const std::vector<QString> lbl17noFP;
extern const std::vector<QString> lbl21;
extern const std::vector<QString> lbl31_less;
extern const std::vector<QString> lbl31_more_withEOG;
extern const std::vector<QString> lbl31_more;
extern const std::vector<QString> lbl31_good;
extern const std::vector<QString> lbl32;
extern const std::vector<QString> lbl_all;
namespace egi
{
extern const int manyChannels;
extern const std::vector<int> chans128to20;
extern const std::vector<QString> chans128to19mark;
extern const std::vector<QString> chans128to19noeyesMark;
extern const std::vector<QString> chans128groups;
extern const std::map<QString, std::vector<QString>> chans128;
} /// end namespace egi
} /// end namespace coords

#endif // COORDS_H

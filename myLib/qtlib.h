#ifndef QTLIB_H
#define QTLIB_H

#include <QtWidgets>

namespace qtLib
{
void hideLayout(QLayout * lay);
template <class C> std::vector<C*> widgetsOfLayout(QLayout *);
}

#endif // QTLIB_H

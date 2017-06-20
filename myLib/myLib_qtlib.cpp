#include <myLib/qtlib.h>

namespace qtLib
{

void hideLayout(QLayout * lay)
{
	for(int i = 0; i < lay->count(); ++i)
	{
		auto * a = lay->itemAt(i)->widget();
		if(a) { a->hide(); }
	}
}

}

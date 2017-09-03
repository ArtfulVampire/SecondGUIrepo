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

template <class C> std::vector<C*> widgetsOfLayout(QLayout * lay)
{
	std::vector<C*> res;
	for(int i = 0; i < lay->count(); ++i)
	{
		if(auto * a = dynamic_cast<C*>(lay->itemAt(i)->widget())) { res.push_back(a); }
	}
	return res;
}
template std::vector<QDoubleSpinBox*> widgetsOfLayout(QLayout * lay);
template std::vector<QSpinBox*> widgetsOfLayout(QLayout * lay);
template std::vector<QLineEdit*> widgetsOfLayout(QLayout * lay);
template std::vector<QPushButton*> widgetsOfLayout(QLayout * lay);
template std::vector<QCheckBox*> widgetsOfLayout(QLayout * lay);

} // end namespace qtLib

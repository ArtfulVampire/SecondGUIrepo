#include <myLib/temp.h>

std::ostream & myWrite2 (std::ostream & os)
{
	os << std::endl;
	return os;
}

template<class input, class... inputs>
std::ostream & myWrite2 (std::ostream & os, input in, inputs... ins)
{
	os << in << "\t";
	myWrite2(os, ins...);
	return os;
}

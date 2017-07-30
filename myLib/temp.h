#ifndef TEMP_H
#define TEMP_H

#include <iostream>

std::ostream & myWrite2 (std::ostream & os);

template<class input, class... inputs>
std::ostream & myWrite2 (std::ostream & os, input in, inputs... ins);

#endif // TEMP_H

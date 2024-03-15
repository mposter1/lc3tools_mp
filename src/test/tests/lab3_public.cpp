#include "lab3.h"

void setup(Tester &tester)
{
    std::mt19937 mt(0);
    gen_random(40, mt, tester);
}
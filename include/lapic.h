#include <quark.h>

void lapicinit(u8 spurvec);

void timerirqf();
void timerinit(u8 vec);
void timerstart(int hz, int refhz);
u32 timerticks();

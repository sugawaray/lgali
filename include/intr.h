#include <quark.h>

struct Idtrec {
	u16	offl;
	u16	ssel;
	u16	attr;
	u16	offh;
};

#define IDTRTRAP	0xF
#define IDTRINTR	0xE

void idtrsetad(struct Idtrec *o, void (*f)());
void idtrsettype(struct Idtrec *o, u8 t);
void idtrsetssel(struct Idtrec *o, u8 v);
void idtrsetvalid(struct Idtrec *o, int isvalid);

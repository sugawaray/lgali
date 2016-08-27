#include "intrim.h"
#include <intr.h>
#include <lapic.h>
#include <quark.h>
#include <pit8254.h>
#include <tick.h>

enum {
	Irqstimer	= 0x20,
	Irqtimer	= 0x31,
	Irqspurious	= 0x3f,

	HZ	= 100,
	Lhz	= 50
};

static struct Idtrec idt[0x100];

static
void
initidt()
{
	int i, c;
	char tp[6];
	struct Idtrec *rec;

	c = sizeof idt / sizeof idt[0];
	for (i = 0; i < c; ++i) {
		rec = &idt[i];
		idtrsetssel(rec, 0x02);
		idtrsettype(rec, IDTRINTR);
		switch (i) {
		case Irqstimer:
			idtrsetad(rec, tickirqf);
			break;
		case Irqtimer:
			idtrsetad(rec, timerirqf);
			break;
		case Irqspurious:
			idtrsetad(rec, intrdef);
			break;
		default:
			if (i >= Irqstimer)
				idtrsetad(rec, intrdef);
			else
				idtrsetvalid(rec, 0);
			break;
		}
	}

	cli();
	lapicinit(Irqspurious);
	packt((const char *)idt, sizeof idt / sizeof idt[0], tp);
	lidt(tp);
	sti();
}

static
void
initic8259()
{
	cli();
	ic8259init(Irqstimer);
	ic8259setmsk(0xfe, 0xff);
	sti();
}

int
init()
{
	initidt();
	initic8259();
	stimerstart(HZ);
	timerinit(Irqtimer);
	timerstart(Lhz, HZ);
	ic8259setmsk(0xff, 0xff);
	return 0;
}

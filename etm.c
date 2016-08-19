#include "intrim.h"
#include <intr.h>
#include <lapic.h>
#include <led.h>
#include <quark.h>
#include <pit8254.h>
#include <ser.h>
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
#if 1
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
#else
	char tp[6];
	struct Idtrec *rec;

	sidt(tp);
	mmemcpy(idt, *(u32 *)(tp + 2), *(u16 *)tp + 1);
	rec = &idt[Irqstimer];
	idtrsetssel(rec, 0x02);
	idtrsettype(rec, IDTRINTR);
	idtrsetad(rec, tickirqf);

	rec = &idt[Irqtimer];
	idtrsetssel(rec, 0x02);
	idtrsettype(rec, IDTRINTR);
	idtrsetad(rec, timerirqf);

	rec = &idt[Irqspurious];
	idtrsetssel(rec, 0x02);
	idtrsettype(rec, IDTRINTR);
	idtrsetad(rec, intrdef);
#endif
}

static
void
#if 1
initic8259()
#else
initintr()
#endif
{
#if 1
	cli();
	ic8259init(Irqstimer);
	ic8259setmsk(0xFE, 0xFF);
	sti();
#else
	char tp[6];

	cli();
	ic8259init(Irqstimer);
	ic8259setmsk(0xFE, 0xFF);
	lapicinit(Irqspurious);
	packt((const char *)idt, sizeof idt / sizeof idt[0], tp);
	lidt(tp);
	sti();
#endif
}

int
main()
{
	int r, q;
	u32 t;

	wait();
	ledminit();
	r = serinit();
	if (r != 0)
		ledmemit(5, 100);
	else {
		initidt();
#if 1
		initic8259();
#else
		initintr();
#endif
		stimerstart(HZ);
		timerinit(Irqtimer);
		timerstart(Lhz, HZ);
		ic8259setmsk(0xff, 0xff);
	}
	q = 0;
	for (;;) {
		t = timerticks();
		if ((q != t / Lhz) && (t % Lhz == 0)) {
			q = t / Lhz;
			ledmemit(1, 1);
		}
	}
	return 0;
}

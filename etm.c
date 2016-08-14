#include "intrim.h"
#include "tmim.h"
#include <intr.h>
#include <led.h>
#include <quark.h>
#include <pit8254.h>
#include <ser.h>

enum {
	Irqstimer	= 0x20,

	HZ	= 100
};

static struct Idtrec idt[0x100];

static
void
initintr()
{
	char tp[6];
	struct Idtrec *rec;

	sidt(tp);
	mmemcpy(idt, *(u32 *)(tp + 2), *(u16 *)tp + 1);
	rec = &idt[Irqstimer];
	idtrsetssel(rec, 0x02);
	idtrsettype(rec, IDTRINTR);
	idtrsetad(rec, tickirqf);

	cli();
	ic8259init(Irqstimer);
	ic8259setmsk(0xFE, 0xFF);
	packt((const char *)idt, sizeof idt / sizeof idt[0], tp);
	lidt(tp);
	sti();
}

int
main()
{
	int r;

	wait();
	ledminit();
	r = serinit();
	if (r != 0)
		ledmemit(5, 100);
	else {
		initintr();
		stimerstart(HZ);
		/* initialize LAPIC timer */
		/* stop i8254 system timer */
	}
	for (;;)
		;
	return 0;
}

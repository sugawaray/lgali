#include "intrim.h"
#include <intr.h>
#include <led.h>
#include <quark.h>
#include <ser.h>

enum {
	Irqstimer	= 0x20
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
	idtrsetad(rec, ontimer);

	cli();
	ic8259_init(Irqstimer);
	ic8259_setmsk(0xFE, 0xFF);
	packt((const char *)idt, sizeof idt / sizeof idt[0], tp);
	lidt(tp);
	sti();
}

static
void
timerinit()
{
	outb(0x43, 0x36);
	outb(0x40, 0xFF);
	outb(0x40, 0xFF);
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
		timerinit();
		/* initialize i8254 */
		/* initialize LAPIC timer */
		/* stop i8254 system timer */
	}
	for (;;)
		;
	return 0;
}

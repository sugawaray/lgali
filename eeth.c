#include <quark.h>
#include <intr.h>
#include <led.h>
#include <lib.h>
#include <ser.h>

#define IDDEV	20
#define IDFUN	6

enum {
	VEC	= 0x30
};

static struct Idtrec idt[0x100];

static
void
intrinit()
{
	struct Idtrec *rec;
	char tp[6];

	rec = &idt[VEC];
	sidt(tp);
	mmemcpy(idt, *(u32 *)(tp + 2), *(u16 *)tp + 1);
}

int
main()
{
	int r;

	wait();
	ledminit();
	r = serinit();
	if (r != 0) {
		ledmemit(5, 100);
		for (;;)
			;
	}
	serout("serial port initialization has done.\r\n");
	serout("ethernet pci configuration registers.\r\n");
	pcicsetdev(IDDEV);
	pcicsetfn(IDFUN);
	pcicpr();
	intrinit();

	for (;;)
		;
	return 0;
}

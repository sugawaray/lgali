#include <fat.h>
#include <nomagicc.h>
#include "th.h"

#define AEQ(e, x)	do { if ((e) != (x)) { testerror(testmsg("0x%08x != 0x%08x", (e), (x))); } } while (0)

static char buf[512];

static
void
rdbin()
{
	TANY fd;
	char *p;

	fd = fopen("./fat0sect", "rb");
	if (fd == 0) {
		testerror("test error rdbin");
		return;
	}
	p = buf;
	do {
		p += fread(p, 1, sizeof buf - (p - buf), fd);
	} while (!feof(fd) && (sizeof buf - (p - buf)) > 0);
	fclose(fd);
}

static
void
rdbsbpb_test()
{
	struct BsBpb o;

	rdbin();
	memset(&o, 0, sizeof o);
	o.BPB_RootEntCnt = 0xff;
	o.BPB_TotSec16 = 0xff;
	o.BPB_FATSz16 = 0xff;
	o.BPB_HiddSec = 0xffffffff;
	rdbsbpb(&o, buf);
	AEQ(0xeb, o.BS_jmpBoot[0]);
	AEQ(0x58, o.BS_jmpBoot[1]);
	AEQ(0x90, o.BS_jmpBoot[2]);

	AEQ(0x6d, o.BS_OEMName[0]);
	AEQ(0x6b, o.BS_OEMName[1]);
	AEQ(0x66, o.BS_OEMName[2]);
	AEQ(0x73, o.BS_OEMName[3]);
	AEQ(0x2e, o.BS_OEMName[4]);
	AEQ(0x66, o.BS_OEMName[5]);
	AEQ(0x61, o.BS_OEMName[6]);
	AEQ(0x74, o.BS_OEMName[7]);

	AEQ(512, o.BPB_BytsPerSec);

	AEQ(0x08, o.BPB_SecPerClus);

	AEQ(0x20, o.BPB_RsvdSecCnt);

	AEQ(0x02, o.BPB_NumFATs);

	AEQ(0x00, o.BPB_RootEntCnt);

	AEQ(0x00, o.BPB_TotSec16);

	AEQ(0xf8, o.BPB_Media);

	AEQ(0x00, o.BPB_FATSz16);

	AEQ(0x10, o.BPB_SecPerTrk);

	AEQ(0x04, o.BPB_NumHeads);

	AEQ(0x00000000, o.BPB_HiddSec);

	AEQ(0x00ed3800, o.BPB_TotSec32);
}

int
main()
{
	testrun("rdbsbpb", rdbsbpb_test);
	return 0;
}

#include <fat.h>
#include <nomagicc.h>
#include "th.h"

#define AEQ(e, x)	do { if ((e) != (x)) { testerror(testmsg("0x%08x != 0x%08x", (e), (x))); } } while (0)
#define AEQS(e, x)	do { if (strcmp((e), (x)) != 0) { testerror(testmsg("[%s] != [%s]", (e), (x))); } } while (0)

static char buf[512];
static char *buf1;
static int buf1sz;

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
rdbin1()
{
	int bsz;
	TANY f;
	char *p, *t;

	buf1 = 0;
	buf1sz = 0;
	f = fopen("./fat", "rb");
	if (f == 0) {
		testerror("test error rdbin1");
		return;
	}

	bsz = 512;
	buf1 = malloc(bsz);
	if (!buf1) {
		testerror("test error rdbin1: memory");
		fclose(f);
		return;
	}
	p = buf1;
	while (!feof(f)) {
		if (bsz - (p - buf1) <= 0) {
			bsz *= 2;
			t = realloc(buf1, bsz);
			if (!t) {
				testerror("test error rdbin1: memory");
				break;
			}
			p = t + (p - buf1);
			buf1 = t;
		}
		p += fread(p, 1, bsz - (p - buf1), f);
	}
	if (!feof(f)) {
		free(buf1);
		buf1 = 0;
	} else {
		buf1sz = (int)ftell(f);
	}
	fclose(f);
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

static
void
rdbsbpb32_test()
{
	struct BsBpb32 o;

	rdbin();
	memset(&o, 0, sizeof o);
	o.BPB_ExtFlags = 0xffff;
	o.BPB_FSVer = 0xffff;
	rdbsbpb32(&o, buf);

	AEQ(0x00003b31, o.BPB_FATSz32);
	AEQ(0x0000, o.BPB_ExtFlags);
	AEQ(0x0000, o.BPB_FSVer);
	AEQ(0x00000002, o.BPB_RootClus);
	AEQ(0x0001, o.BPB_FSInfo);
	AEQ(0x0006, o.BPB_BkBootSec);
	AEQ(0x80, o.BS_DrvNum);
	AEQ(0x29, o.BS_BootSig);
	AEQ(0x238c6189, o.BS_VolID);
	AEQ(0x4e, o.BS_VolLab[0]);
	AEQ(0x4d, o.BS_VolLab[5]);
	AEQ(0x20, o.BS_VolLab[10]);
}

static
void
rddir_root_test()
{
	struct BsBpb b1;
	struct BsBpb32 b2;
	struct Dir o;

	rdbin1();
	rdbsbpb(&b1, buf1);
	rdbsbpb32(&b2, buf1);
	rddir(&o, buf1, &b1, &b2);
	AEQS(o.DIR_Name, "EMPTY   TXT");
}

int
main()
{
	testrun("rdbsbpb", rdbsbpb_test);
	testrun("rdbsbpb32", rdbsbpb32_test);
	testrun("rddir root", rddir_root_test);
	return 0;
}

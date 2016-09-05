#include <fat.h>

enum {
	OBsOEMName	= 0x03,
	OBpbBytsPerSec	= 0x0b,
	OBpbSecPerClus	= 0x0d,
	OBpbRsvdSecCnt	= 0x0e,
	OBpbNumFATs = 0x10,
	OBpbRootEntCnt	= 0x11,
	OBpbTotSec16	= 0x13,
	OBpbMedia	= 0x15,
	OBpbFATSz16	= 0x16,
	OBpbSecPerTrk	= 0x18,
	OBpbNumHeads	= 0x1a,
	OBpbHiddSec	= 0x1c,
	OBpbTotSec32	= 0x20,
	OBpbFATSz32	= 0x24,
	OBpbExtFlags	= 0x28,
	OBpbFSVer	= 0x2a,
	OBpbRootClus	= 0x2c,
	OBpbFSInfo	= 0x30,
	OBpbBkBootSec	= 0x32,
	OBsDrvNum	= 0x40,
	OBsBootSig	= 0x42,
	OBsVolID	= 0x43,
	OBsVolLab	= 0x47
};

#define M16(o)	(*(u16 *)(o))
#define M32(o)	(*(u32 *)(o))

void
rdbsbpb(struct BsBpb *o, const char *dat)
{
	int i;
	for (i = 0; i < 3; ++i)
		o->BS_jmpBoot[i] = dat[i];
	for (i = 0; i < 8; ++i)
		o->BS_OEMName[i] = dat[OBsOEMName + i];
	o->BPB_BytsPerSec = M16(dat + OBpbBytsPerSec);
	o->BPB_SecPerClus = dat[OBpbSecPerClus];
	o->BPB_RsvdSecCnt = M16(dat + OBpbRsvdSecCnt);
	o->BPB_NumFATs = dat[OBpbNumFATs];
	o->BPB_RootEntCnt = M16(dat + OBpbRootEntCnt);
	o->BPB_TotSec16 = M16(dat + OBpbTotSec16);
	o->BPB_Media = dat[OBpbMedia];
	o->BPB_FATSz16 = M16(dat + OBpbFATSz16);
	o->BPB_SecPerTrk = M16(dat + OBpbSecPerTrk);
	o->BPB_NumHeads = M16(dat + OBpbNumHeads);
	o->BPB_HiddSec = M32(dat + OBpbHiddSec);
	o->BPB_TotSec32 = M32(dat + OBpbTotSec32);
}

void
rdbsbpb32(struct BsBpb32 *o, const char *dat)
{
	int i;
	o->BPB_FATSz32 = M32(dat + OBpbFATSz32);
	o->BPB_ExtFlags = M16(dat + OBpbExtFlags);
	o->BPB_FSVer = M16(dat + OBpbFSVer);
	o->BPB_RootClus = M32(dat + OBpbRootClus);
	o->BPB_FSInfo = M16(dat + OBpbFSInfo);
	o->BPB_BkBootSec = M16(dat + OBpbBkBootSec);
	o->BS_DrvNum = dat[OBsDrvNum];
	o->BS_BootSig = dat[OBsBootSig];
	o->BS_VolID = M32(dat + OBsVolID);
	for (i = 0; i < 11; ++i)
		o->BS_VolLab[i] = dat[OBsVolLab + i];
}

#include "ethim.h"

enum {
	Msarc	= 0x70000000,
	Sarc	= 0x20000000,
	Mjd	= 0x00400000,
	Jdng	= 0x00400000,
	Mfes	= 0x00004000,
	Fes10	= 0x00000000,
	Mdm	= 0x00000800,
	Dmfull	= 0x00000800,
	Mte	= 0x00000008,
	Te	= 0x00000008,
	Mre	= 0x00000004,
	Re	= 0x00000004
/* #define AE	0x80000000 */
};

u32
ethdefmcnf(u32 orig)
{
	orig &= ~Msarc;
	orig |= Sarc;

	orig &= ~Mjd;
	orig |= Jdng;

	orig &= ~Mfes;
	orig |= Fes10;

	orig &= ~Mdm;
	orig |= Dmfull;

	orig &= ~Mte;
	orig |= Te;

	orig &= ~Mre;
	orig |= Re;

	return orig;
}

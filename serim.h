#define IDBUS	0
#define IDDEV	20
#define IDFUN	5
#define OFFBAR0	0x10

static int detmmsz();
static void detmmap();

struct Ruart {
	u32 rbr_thr_dll;
	u32 ier_dlh;
	u32 iir_fcr;
	u32 lcr;
	u32 _rsv1;
	u32 lsr;
};

extern volatile struct Ruart *ruart;

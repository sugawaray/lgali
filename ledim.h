#define IDBUS	0
#define IDDEV	31
#define IDFUN	0

#define OFFGPIOBASE	0x44

#define PCICADDR_GPIOBASE	PCICADDR(IDBUS, IDDEV, IDFUN, OFFGPIOBASE)

#define OFFRGEN	0x20
#define OFFRGIO	0x24
#define OFFRGLVL	0x28

u32 getgba();
void prepled(u32 gba, u16 ledn);
void toggleled(u32 gba, u16 ledn);


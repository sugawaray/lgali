#ifndef QUARK_H
#define QUARK_H	1

#define PCIC_ADDR_PORT	0xCF8
#define PCIC_DATA_PORT	0xCFC

#define PCICADDR(b, d, f, o)	\
	(0x80000000 | ((b) << 16) | ((d) << 11) | ((f) << 8) | (o))

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned	u32;

void outb(u16 port, u8 value);
void outw(u16 port, u16 value);
void outl(u16 port, u32 value);
void inb(u16 port, u8 *buf);
void inw(u16 port, u16 *buf);
void inl(u16 port, u32 *buf);

void nop();

void sgdt(char *d);
void lidt(const char *src);
void sidt(char *d);
void ic8259_init(int off);
void cli();
void sti();

void wait();

void pcicsetdev(int dev);
void pcicsetfn(int fn);
void pcicpr();
void pcicw32(u32 off, u32 v);
void pcicw16(u32 off, u16 v);
void pcicr32(u32 off, u32 *v);
void pcicr16(u32 off, u16 *v);

#endif

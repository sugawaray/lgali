#include "impl.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <nomagicc.h>
#include <string.h>

#define S(v)	#v
#define A(v)	do { if (!(v)) { testerror(S(v)); } } while (0)

void
t()
{
	int i;
	char b[256];

	memset(b, -1, sizeof b);
	readfile("tbin", b);
	for (i = 0; i < 128; ++i)
		A(b[i] == i);
	for (; i < 256; ++i)
		A(b[i] == -1);
}

static
void
t1()
{
	int r;
	struct sockaddr_ll ad;

	r = addrinit(2, "01:23:45:67:89:AB", &ad);
	A(ad.sll_family == AF_PACKET);
	A(ad.sll_ifindex == 2);
	A(ad.sll_halen == 6);
	A(ad.sll_addr[0] == 0x01);
	A(ad.sll_addr[1] == 0x23);
	A(ad.sll_addr[2] == 0x45);
	A(ad.sll_addr[3] == 0x67);
	A(ad.sll_addr[4] == 0x89);
	A(ad.sll_addr[5] == 0xAB);
	A(r == 0);
}

int
main()
{
	testrun("readfile", t);
	testrun("addr", t1);
}

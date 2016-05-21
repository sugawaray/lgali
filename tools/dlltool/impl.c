#include "impl.h"
#include <sys/socket.h>
#include <linux/if_arp.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <stdio.h>
#include <string.h>

int
readfile(const char *pa, char *b)
{
	char *p;
	FILE *f;

	f = fopen(pa, "r");
	if (f == 0)
		return -1;
	p = b;
	do {
		p += fread(p, 1, 256 - (p - b), f);
	} while (!feof(f) && 256 - (p - b));
	fclose(f);
	return 0;
}

static const char hex[] = "0123456789ABCDEF";
int
addrinit(int ind, const char *mac, struct sockaddr_ll *o)
{
	int i;
	const char *p, *q;
	unsigned char *s;

	memset(o, 0, sizeof *o);
	o->sll_family = AF_PACKET;
	o->sll_ifindex = ind;
	o->sll_halen = 6;
	s = (unsigned char *)o->sll_addr;
	i = 0;
	q = mac - 1;
	do {
		p = q + 1;
		q = p + strspn(p, hex);
		s[i] = (strchr(hex, p[0]) - hex) << 4;
		s[i++] += strchr(hex, p[1]) - hex;
	} while (*q && i < 6);
	return 0;
}

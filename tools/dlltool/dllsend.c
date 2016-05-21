#include "impl.h"
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static char buf[256];

void
sendbin(int f, const char *p, int sz)
{
	const char *q;
	int t;

	errno = 0;
	q = p;
	do {
		t = write(f, q, sz - (q - p));
		if (t < 0) {
			perror("write");
			return;
		}
		q += t;
	} while (sz - (q - p) > 0);
}

int
main(int argc, char **argv)
{
	int f, r;
	struct sockaddr_ll ad;

	if (argc < 4) {
		fprintf(stderr, "me filepath ifindex mac\n");
		exit(1);
	}
	if (readfile(argv[1], buf) != 0) {
		fprintf(stderr, "reading the input\n");
		exit(1);
	}
	errno = 0;
	f = socket(AF_PACKET, SOCK_RAW, ntohs(ETH_P_ALL));
	if (f == -1) {
		perror("socket creation");
		exit(1);
	}
	r = addrinit((int)strtol(argv[2], 0, 10), argv[3], &ad);
	if (r != 0) {
		fprintf(stderr, "address initialization\n");
		goto end;
	}
	r = bind(f, (struct sockaddr *)&ad, sizeof ad);
	if (r != 0) {
		perror("socket binding\n");
		goto end;
	}
	sendbin(f, buf, 0x40);
end:
	close(f);
}

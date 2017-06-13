#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <net/bpf.h>
#include <net/if.h>

#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *ifname;
uint8_t macaddr[6];

#define PKTBUFSIZE	2048
#define QMAX		100

struct pktqueue {
	struct pktbuf *first;
	struct pktbuf *last;
	unsigned int len;
};

struct pktbuf {
	struct pktbuf *next;
	unsigned char buf[PKTBUFSIZE];
	unsigned int size;
};

struct pktqueue inq = { NULL, NULL };
struct pktqueue outq = { NULL, NULL };

/* asm for "(ether dst 1:2:3:4:5:6) or (ether multicast)" */
static struct bpf_insn bpf_insns[] = {
	{ 0x20, 0, 0, 0x00000002 },
	{ 0x15, 0, 2, 0x03040506 },
	{ 0x28, 0, 0, 0x00000000 },
	{ 0x15, 2, 0, 0x00000102 },
	{ 0x30, 0, 0, 0x00000000 },
	{ 0x45, 0, 1, 0x00000001 },
	{ 0x6, 0, 0, 0x00040000 },
	{ 0x6, 0, 0, 0x00000000 },
};


static int bpf_open(const char *ifname, uint8_t *macaddr);

static int pktqueue_full(struct pktqueue *);

static void macaddress_str2bin(const char *str, uint8_t *bin);
static void usage(void);


static int
bpf_open(const char *ifname, uint8_t *macaddr)
{
	struct bpf_program bpf_prog;
	struct ifreq ifr;
	uint32_t n;
	int fd, i, k;
	char filename[20];

	for (fd = -1, i = 0; i < 10; i++) {
		snprintf(filename, sizeof(filename), "/dev/bpf%u", i);
		fd = open(filename, O_RDWR);
		if (fd >= 0)
			break;
		if (errno != EBUSY)
			err(1, "open bpf");
	}
	if (fd == -1)
		errx(1, "no bpf device available");

	k = PKTBUFSIZE;
	if (ioctl(fd, BIOCSBLEN, &k) == -1)
		err(1, "ioctl(BIOCSBLEN)");

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(fd, BIOCSETIF, &ifr) == -1)
		err(1, "ioctl(BIOCSETIF)");

	k = 1;
	if (ioctl(fd, BIOCIMMEDIATE, &k) == -1)
		err(1, "ioctl(BIOCIMMEDIATE)");

	k = 1;
	if (ioctl(fd, BIOCSHDRCMPLT, &k) == -1)
		err(1, "ioctl(BIOCSHDRCMPLT)");

	/* don't want loopback */
	k = 0;
	if (ioctl(fd, BIOCGSEESENT, &k) == -1)
		err(1, "ioctl(BIOCGSEESENT)");

	k = 1;
	if (ioctl(fd, BIOCPROMISC, &k) == -1)
		err(1, "ioctl(BIOCPROMISC)");

	/* replace macaddr */
	n = ((uint32_t)macaddr[2] << 24) + ((uint32_t)macaddr[3] << 16) +
	    ((uint32_t)macaddr[4] <<  8) + macaddr[5];
	bpf_insns[1].k = n;
	n = ((uint32_t)macaddr[0] << 8) + macaddr[1];
	bpf_insns[3].k = n;

	bpf_prog.bf_len = sizeof(bpf_insns) / sizeof(bpf_insns[0]);
	bpf_prog.bf_insns = bpf_insns;
	if (ioctl(fd, BIOCSETFNR, &bpf_prog) == -1)
		err(1, "ioctl(BIOCSETFNR)");

	return fd;
}

struct pktbuf *
pktbuf_new(void)
{
	struct pktbuf *p;

	p = malloc(sizeof(struct pktbuf));
	p->next = NULL;
	p->size = 0;
	return p;
}

void
pktbuf_free(struct pktbuf *p)
{
	if (p->next != NULL)
		warnx("pktbuf->next is not free");
	free(p);
}

void
pktqueue_init(struct pktqueue *q)
{
	q->first = q->last = NULL;
	q->len = 0;
}

int
pktqueue_empty(struct pktqueue *q)
{
	return q->len == 0;
}

struct pktbuf *
pktqueue_dequeue(struct pktqueue *q)
{
	struct pktbuf *pkt;

	if (q->len == 0)
		return NULL;
	pkt = q->first;
	if (pkt->next != NULL)
		q->first = pkt->next;
	else
		q->first = q->last = NULL;
	q->len--;
	pkt->next = NULL;
	return pkt;
}

void
pktqueue_enqueue(struct pktqueue *q, struct pktbuf *pkt)
{
	if (pktqueue_full(q)) {
		warnx("queue full");
		return;
	}
	pkt->next = NULL;
	if (q->first == NULL)
		q->first = q->last = pkt;
	else {
		q->last->next = pkt;
		q->last = pkt;
	}
	q->len++;
}

static int
pktqueue_full(struct pktqueue *q)
{
	return q->len >= QMAX;
}


static void
macaddress_str2bin(const char *str, uint8_t *bin)
{
	unsigned int oct[6];
	int i;

	sscanf(str, "%x:%x:%x:%x:%x:%x",
	       &oct[0], &oct[1], &oct[2], &oct[3], &oct[4], &oct[5]);
	for (i = 0; i < 6; i++)
		bin[i] = oct[i];
}

void
usage(void)
{
	printf("usage: ifcat <ifname> <macaddr>\n");
}

int
main(int argc, char **argv)
{
	struct pktbuf *pkt;
	struct sockaddr_un sun;
	fd_set rfds, wfds;
	socklen_t salen;
	int bpf, ch, i, n, lsock, sock;

	while ((ch = getopt(argc, argv, "")) != -1) {
		switch (ch) {
		case '?':
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 2) {
		usage();
		exit(0);
	}

	ifname = argv[0];
	macaddress_str2bin(argv[1], macaddr);

	bpf = bpf_open(ifname, macaddr);

	lsock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (lsock == -1)
		err(1, "socket");

	(void)unlink(ifname);

	memset(&sun, 0, sizeof(sun));
	sun.sun_family = AF_UNIX;
	strncpy(sun.sun_path, ifname, sizeof(sun.sun_path));
	if (bind(lsock, (struct sockaddr *)&sun, sizeof(sun)) == -1)
		err(1, "bind");
	(void)chmod(sun.sun_path, 0775);

	if (listen(lsock, 1) == -1)
		err(1, "listen");

	sock = accept(lsock, (struct sockaddr *)&sun, &salen);
	if (sock == -1)
		err(1, "accept");

	pktqueue_init(&inq);
	pktqueue_init(&outq);

	while (1) {
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		if (!pktqueue_empty(&inq))
			FD_SET(sock, &wfds);
		if (!pktqueue_full(&inq))
			FD_SET(bpf, &rfds);
		if (!pktqueue_empty(&outq))
			FD_SET(bpf, &wfds);
		if (!pktqueue_full(&outq))
			FD_SET(sock, &rfds);

		n = select(sock + 1, &rfds, &wfds, NULL, NULL);
		if (n == -1) {
			if (errno == EINTR)
				continue;
			err(1, "poll");
		}

		if (FD_ISSET(sock, &wfds)) {
			pkt = pktqueue_dequeue(&inq);
			i = write(sock, &pkt->size, sizeof(pkt->size));
			if (i == -1)
				err(1, "write to stdout");
			i = write(sock, pkt->buf, pkt->size);
			if (i == -1)
				err(1, "write to stdout");
			pktbuf_free(pkt);
		}

		if (FD_ISSET(bpf, &wfds)) {
			pkt = pktqueue_dequeue(&outq);
			i = write(bpf, pkt->buf, pkt->size);
			if (i == -1)
				err(1, "send");
			pktbuf_free(pkt);
		}

		if (FD_ISSET(bpf, &rfds)) {
			pkt = pktbuf_new();
			i = read(bpf, pkt->buf, sizeof(pkt->buf));
			if (i == -1)
				err(1, "read");
			pkt->size = i;
			pktqueue_enqueue(&inq, pkt);
		}

		if (FD_ISSET(sock, &rfds)) {
			pkt = pktbuf_new();
			i = read(sock, &pkt->size, sizeof(pkt->size));
			if (i == -1)
				err(1, "read from stdin");
			i = read(sock, pkt->buf, pkt->size);
			if (i == -1)
				err(1, "read from stdin");
			pktqueue_enqueue(&inq, pkt);
		}
	}

	exit(0);
}

/*
 * pcap.c
 */

#include "pcap.h"

ScmObj test_pcap(void)
{
	return SCM_MAKE_STR("pcap is working");
}

static void pcap_print(ScmObj obj, ScmPort* port, ScmWriteContext* ctx);

SCM_DEFINE_BUILTIN_CLASS_SIMPLE(Scm_PCAPClass, pcap_print);

static void pcap_print(ScmObj obj, ScmPort* port, ScmWriteContext* ctx)
{
	ScmPCAP* t = SCM_PCAP(obj);
	Scm_Printf(port, "#<PCAP>");
}

/*
 * Module initialization function.
 */
extern void Scm_Init_pcaplib(ScmModule*);

void Scm_Init_pcap(void)
{
	ScmModule *mod;

	/* Register this DSO to Gauche */
	SCM_INIT_EXTENSION(pcap);

	/* Create the module if it doesn't exist yet. */
	mod = SCM_MODULE(SCM_FIND_MODULE("pcap", TRUE));

	/* Register stub-generated procedures */
	Scm_Init_pcaplib(mod);

	Scm_InitStaticClass(&Scm_PCAPClass, "<pcap>", mod, NULL, 0);
}

pcap_t *gpcap;
/* XXX */

// XXX: interface must be configurable
ScmObj Scm_MakePCAP(void)
{
	ScmPCAP *p = SCM_NEW_ATOMIC(ScmPCAP);
	int ret;
	char errbuf[PCAP_ERRBUF_SIZE];

	SCM_SET_CLASS(p, SCM_CLASS_PCAP);
	p->pcap = pcap_create("en0", errbuf);
	if (p->pcap == NULL)
		Scm_Error("pcap_create: %s", errbuf);
        ret = pcap_set_snaplen(p->pcap, 1518);
	if (ret != 0)
		Scm_Error("pcap_set_snaplen: %s", pcap_statustostr(ret));
        ret = pcap_set_timeout(p->pcap, 1000);
	if (ret != 0)
		Scm_Error("pcap_set_timeout: %s", pcap_statustostr(ret));
gpcap = p->pcap;
	ret = pcap_activate(p->pcap);
	if (ret != 0)
		Scm_Error("pcap_activate: %d", ret);

	//Scm_RegisterFinalizer(SCM_OBJ(p), pcap_finalize, NULL);
	return SCM_OBJ(p);
}

unsigned int Scm_pcap_next(ScmObj buf)
{
	struct pcap_pkthdr hdr;
	int bufsize;
	unsigned char *bufptr;
	const unsigned char *cp;

	if (!SCM_UVECTORP(buf))
		Scm_TypeError("pcap frame", "uniform vector", buf);

	bufsize = Scm_UVectorSizeInBytes(SCM_UVECTOR(buf));
        bufptr = SCM_UVECTOR_ELEMENTS(buf);

	cp = pcap_next(gpcap, &hdr);
	if (cp == NULL)
		Scm_Error("pcap_next => NULL");

	/* XXX: size */
	memcpy(bufptr, cp, hdr.len);
	return hdr.len;
}

int Scm_pcap_inject(ScmObj buf)
{
	struct pcap_pkthdr hdr;
	int bufsize, ret;
	unsigned char *bufptr;
	const unsigned char *cp;

	if (!SCM_UVECTORP(buf))
		Scm_TypeError("pcap frame", "uniform vector", buf);

	bufsize = Scm_UVectorSizeInBytes(SCM_UVECTOR(buf));
        bufptr = SCM_UVECTOR_ELEMENTS(buf);
	return pcap_inject(gpcap, bufptr, bufsize);
}

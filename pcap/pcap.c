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

ScmObj Scm_MakePCAP(void)
{
	ScmPCAP *p = SCM_NEW(ScmPCAP);
	SCM_SET_CLASS(p, SCM_CLASS_PCAP);
	//Scm_RegisterFinalizer(SCM_OBJ(p), pcap_finalize, NULL);
	return SCM_OBJ(p);
}

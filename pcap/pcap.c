/*
 * pcap.c
 */

#include "pcap.h"

/*
 * The following function is a dummy one; replace it for
 * your C function definitions.
 */

ScmObj test_pcap(void)
{
    return SCM_MAKE_STR("pcap is working");
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
}

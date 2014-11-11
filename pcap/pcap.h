/*
 * pcap.h
 */

/* Prologue */
#ifndef GAUCHE_PCAP_H
#define GAUCHE_PCAP_H

#include <gauche.h>
#include <gauche/extend.h>

#include <pcap/pcap.h>

SCM_DECL_BEGIN

typedef struct {
	SCM_HEADER;
	pcap_t *pcap;
} ScmPCAP;

SCM_CLASS_DECL(Scm_PCAPClass);

#define SCM_CLASS_PCAP   (&Scm_PCAPClass)
#define SCM_PCAP(obj)    ((ScmPCAP*)obj)
#define SCM_PCAPP(obj)   SCM_XTYPEP(obj, SCM_CLASS_PCAP)

extern ScmObj test_pcap(void);
extern ScmObj Scm_MakePCAP(void);

SCM_DECL_END

#endif  /* GAUCHE_PCAP_H */

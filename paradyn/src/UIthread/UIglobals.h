/* $Log: UIglobals.h,v $
/* Revision 1.5  1994/05/07 23:26:29  karavan
/* added short explanation feature to SHG.
/*
 * Revision 1.4  1994/05/05  19:53:28  karavan
 * added structure defn
 *
 * Revision 1.3  1994/05/05  02:13:28  karavan
 * moved CmdTabEntry definition from paradyn.tcl.C to UIglobals.h
 *
 * Revision 1.2  1994/04/06  17:39:00  karavan
 * changed interp to global
 *
 * Revision 1.1  1994/04/05  04:42:34  karavan
 * initial version of UI thread code and tcl paradyn command
 * */

/* UIglobals.h 
     definitions used by UI thread */

#ifndef _ui_globals_h
#define _ui_globals_h

#include "dataManager.CLNT.h"
#include "performanceConsultant.CLNT.h"
#include "UI.SRVR.h"
extern "C" {
 #include "tk.h"
}
#define UIMBUFFSIZE 256


struct cmdTabEntry 
{
  char *cmdname;
  int (*func)(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);
};

typedef struct UIMReplyRec {
  void *cb;
  thread_t tid;
} UIMReplyRec;


extern resource                  *uim_rootRes;
extern int                       uim_eid;
extern List<metricInstance*>     uim_enabled;
extern performanceStream         *uim_defaultStream;
extern UIM                       *uim_server;
extern int UIMMsgTokenID;
extern Tcl_HashTable UIMMsgReplyTbl;
extern Tcl_HashTable shgNamesTbl;
extern Tcl_Interp *interp;

#endif

#ifndef ORVuCTRL_API_H
 #define ORVuCTRL_API_H

#define ORVuCTRL_API_VERSION_MAJOR 0
#define ORVuCTRL_API_VERSION_MINOR 1

#ifdef ORVuCTRL_API_EXPORTS
 #define ORVuCTRL_API __declspec(dllexport)
#else
 #define ORVuCTRL_API __declspec(dllimport)
#endif

typedef size_t ORVuHNDL; // ORVuCtrl handle

enum ORVuVERTYPE { ORVuVER_DLL = 0, ORVuVER_REMOTE };

// Not needed at this level
//#ifndef ORV_CTRL_STATE_H
// #include "ORVuCtrl\CtrlState.h"
//#endif

struct ORVuCtrldata_s {
  unsigned short idx;
  float red, grn, blu, lvl;
};
#ifndef ORVu_CTRL_H
  typedef ORVuCtrldata_s ORVuCtrldata;
#endif

#ifdef __cplusplus
  extern "C" {
#endif

// IPaddrs may be a list of IP addresses separated by semi-colon
// Connect will attempt to reach all addresses in the list
// If any succeed, then Connect succeeds skipping the unconnected hubs
ORVuCTRL_API ORVuHNDL ORVuCtrl_Connect     (const char *IPaddrs);
ORVuCTRL_API bool   ORVuCtrl_Disconnect  (ORVuHNDL orvucHndl); 
ORVuCTRL_API bool   ORVuCtrl_IsValid     (ORVuHNDL orvucHndl);
ORVuCTRL_API bool   ORVuCtrl_Ping        (ORVuHNDL orvucHndl);
ORVuCTRL_API int    ORVuCtrl_GetLastError(ORVuHNDL orvucHndl);
ORVuCTRL_API bool   ORVuCtrl_GetStatus   (ORVuHNDL orvucHndl, char *rspbuf, int bufsiz);
ORVuCTRL_API float  ORVuCtrl_Version     (ORVuHNDL orvucHndl, int orvuc_vertype);

ORVuCTRL_API int    ORVuCtrl_GetNumLEDs  (ORVuHNDL orvucHndl); 
ORVuCTRL_API bool   ORVuCtrl_GetLEDConfig(ORVuHNDL orvucHndl, int *rspbuf, int bufsiz); 
ORVuCTRL_API bool   ORVuCtrl_SetLEDConfig(ORVuHNDL orvucHndl, int *rspbuf, int bufsiz); 
ORVuCTRL_API float  ORVuCtrl_GetNowTime  (ORVuHNDL orvucHndl); 

ORVuCTRL_API bool   ORVuCtrl_ExchangeState(ORVuHNDL orvucHndl, unsigned long out, unsigned long *in);


#ifdef __cplusplus
  }
#endif

#endif  // ndef ORVuCTRL_API_H

// NeoLEDArrayAPI.cpp: Entry point for the DLL application.

#include "ORVuCtrl.h"
#include "ORVuCtrlAPI.h"
#include "version.h"

ORVuHNDL
ORVuCtrl_Connect(const char *IPaddrs)
{
  ORVuCtrl *orvuc = 0;
  orvuc = ORVuCtrl::CreateORVuCtrl(IPaddrs);  // use default port

  return ORVuHNDL(orvuc);
}


bool
ORVuCtrl_Disconnect(ORVuHNDL orvucHndl)
{
  if (!orvucHndl) return false;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;
  delete orvuc;

  return true;
}


bool
ORVuCtrl_IsValid(ORVuHNDL orvucHndl)
{
  if (orvucHndl < 0x0100000) return false;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  return orvuc->IsValid();
}


bool
ORVuCtrl_Ping(ORVuHNDL orvucHndl)
{
  if (orvucHndl < 0x0100000) return false;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  return orvuc->Ping();
}


int
ORVuCtrl_GetLastError(ORVuHNDL orvucHndl)
{
  if (orvucHndl < 0x0100000) return -1;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  int err = orvuc->GetLastErr();
  if (err >= 0) return err;

  return -1;
}


bool
ORVuCtrl_GetStatus(ORVuHNDL orvucHndl, char *rspbuf, int bufsiz)
{
  if (orvucHndl < 0x0100000) return false;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  return orvuc->GetStatus(rspbuf, bufsiz);
}



float
ORVuCtrl_Version(ORVuHNDL orvucHndl, int nl_vertype)
{
  if (nl_vertype == ORVuVER_DLL) return float(VERSION);

  float ver = 0;
  if (orvucHndl < 0x0100000) return ver;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  switch (nl_vertype) {
    case ORVuVER_DLL:     ver = float(VERSION);  break;
    case ORVuVER_REMOTE:  ver = orvuc->Version();  break;
  }

  return ver;
}

int    
ORVuCtrl_GetNumLEDs(ORVuHNDL orvucHndl)
{
  if (orvucHndl < 0x0100000) return false;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  return orvuc->GetNumLEDs();
}

bool   
ORVuCtrl_GetLEDConfig(ORVuHNDL orvucHndl, int *rspbuf, int bufsiz)
{
  if (orvucHndl < 0x0100000) return false;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  return orvuc->GetLEDConfig(rspbuf,bufsiz);
}

bool   
ORVuCtrl_SetLEDConfig(ORVuHNDL orvucHndl, int *rspbuf, int bufsiz)
{
  if (orvucHndl < 0x0100000) return false;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  return orvuc->SetLEDConfig(rspbuf,bufsiz);
}

 
float  
ORVuCtrl_GetNowTime(ORVuHNDL orvucHndl)
{
  if (orvucHndl < 0x0100000) return false;

  ORVuCtrl *orvuc = (ORVuCtrl *)orvucHndl;

  return orvuc->GetNowTime();
}

 


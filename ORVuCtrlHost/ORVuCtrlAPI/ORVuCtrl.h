#ifndef ORVu_CTRL_H
 #define ORVu_CTRL_H
#pragma once

#ifndef ORV_CTRL_STATE_H
 #include "ORVuCtrl\CtrlState.h"
#endif

class ORVuCtrl
{
  public:
    ORVuCtrl();
    virtual ~ORVuCtrl();

    static ORVuCtrl* CreateORVuCtrl(const char *IPaddrs, int port = 50555);
    static  bool   GetErrString(int errcode, char *rspbuf, int bufsiz);

    virtual bool   IsValid() const = 0;  // Connectivity test
    virtual bool   Ping() const = 0;     // Round-trip validation
    virtual int    GetLastErr() const = 0;
    virtual bool   GetStatus(char *rspbuf, int bufsiz) const = 0;
    virtual float  Version() const = 0;

    virtual int    GetNumLEDs  () const = 0; 
    virtual bool   GetLEDConfig(int *rspbuf, int bufsiz) = 0; 
    virtual bool   SetLEDConfig(const int *rspbuf, int bufsiz) = 0; 
    virtual float  GetNowTime() = 0; 

    virtual bool   ExchangeState(const ORVCtrlState &out_state, ORVCtrlState &in_state) const = 0;

};

#endif  // ndef ORVu_CTRL_H

// get winsock2

#ifdef _MSC_VER
 #ifndef WIN32_LEAN_AND_MEAN
 // #pragma message("Win32 Lean & Mean")
  #define WIN32_LEAN_AND_MEAN
 #endif
 #if (_MSC_VER > 1400) && defined(_WIN64)
  #include <Windows.h>
 #else
#if 1
  #include <Windows.h>
#else
  #if !defined(_X86_) && !defined(_IA64_)
   #define _X86_
  #endif
  #ifndef _WIN32_WINNT
   #define _WIN32_WINNT 0x0502
  #endif
  #ifndef WINVER
   #define WINVER  _WIN32_WINNT
  #endif
  #ifndef _INC_STDARG
   #include <stdarg.h>
  #endif
  #ifndef _WINDEF_
   #include <windef.h>
  #endif
  #ifndef _WINBASE_
   #include <winbase.h>
  #endif
#endif
 #endif
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>  // inet_pton, InetPton

#include <string>

#include "ORVuCtrl.h"


// =============================================
// ORVuCtrlImp hidden class implementor
// =============================================

class ORVuCtrlImp : public ORVuCtrl
{
    // public interface
  public:
    ORVuCtrlImp();
    virtual ~ORVuCtrlImp();

    bool           Connect(const char *IPaddrs, int port) const;
    virtual bool   IsValid() const;
    virtual bool   Ping() const;
    virtual int    GetLastErr() const;
    virtual bool   GetStatus(char *rspbuf, int bufsiz) const;
    virtual float  Version() const;

    virtual int    GetNumLEDs() const;
    virtual bool   GetLEDConfig(int *rspbuf, int bufsiz);
    virtual bool   SetLEDConfig(const int *rspbuf, int bufsiz);
    virtual float  GetNowTime();

    virtual bool   ExchangeState(const ORVCtrlState &out_state, ORVCtrlState &in_state) const;

  // private methods
  private:
    bool connectToServer() const;

    // data
  private:
    const int BUFLEN = 8192;
    const int SNDBUFLEN = 256;
    SOCKET  m_socket;
    WSADATA m_wsa_data;
    char *  p_str_buffer;
};


// ==========================================================================
// ORVuCtrl base class implementation
// ==========================================================================

ORVuCtrl::ORVuCtrl()
{
}


ORVuCtrl::~ORVuCtrl()
{
}


ORVuCtrl*
ORVuCtrl::CreateORVuCtrl(const char *IPaddrs, int port)
{
  ORVuCtrlImp* orvuc = new ORVuCtrlImp;
  if (orvuc && orvuc->Connect(IPaddrs, port))
    return (ORVuCtrl*)orvuc;

  delete orvuc;
  return 0;
}

bool
ORVuCtrl::GetErrString(int errcode, char *rspbuf, int bufsiz)
{
  // Most errors come from WSAGetLastError()
  return false;
}


// ==========================================================================
// ORVuCtrlImp hidden class implementor Implementation
// =========================================================================

ORVuCtrlImp::ORVuCtrlImp()
  : m_socket(INVALID_SOCKET)
  , m_wsa_data()
  , p_str_buffer(0)
{
  int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsa_data);
  if (iResult == NO_ERROR) {
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  }
  // create a big receive buffer that every method will leverage
  p_str_buffer = new char[BUFLEN];
}


ORVuCtrlImp::~ORVuCtrlImp()
{
  WSACleanup();
  delete p_str_buffer;
}


bool
ORVuCtrlImp::Connect(const char *IPaddrs, int port) const
{
  if (!IPaddrs || !(*IPaddrs))
    IPaddrs = "127.0.0.1";
  if (port < 1)
    port = 1632;  // default

  // make sure your socket is valid
  if (m_socket == INVALID_SOCKET)
    return false;


  // Trap on "localhost" and convert to "127.0.0.1" if necessary
  // inet_addr() does not properly recognize "localhost"
  // We should probably checkout inet_pton()

  sockaddr_in clientService;

  // Address family, IP address, and port to connect to
  clientService.sin_family = AF_INET;
//  if (InetPton(AF_INET, IPaddrs, &clientService.sin_addr.s_addr) < 1)
//    return false;
  if (inet_pton(AF_INET, IPaddrs, &clientService.sin_addr.s_addr) < 1)
    return false;
  clientService.sin_port = htons(port);

  // Attempt server connection.
  int iResult = connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService));

  return (iResult != SOCKET_ERROR);
}


bool
ORVuCtrlImp::IsValid() const
{
  // Initialize vars
//  fd_set readfds;    // not used
  fd_set writefds;
  fd_set exceptfds;
  struct timeval timeout;
  char buf1[BUFSIZ] = "";  // , buf2[BUFSIZ];

  // Clear sockets
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);

  // Add descriptors
  FD_SET(m_socket, &writefds);
  FD_SET(m_socket, &exceptfds);

  int n = int(m_socket + 1); // socket descriptor
  timeout.tv_sec = 10; // timeout values (second and milliseconds)
  timeout.tv_usec = 500000;
  int numSocketsReady = select(n, NULL, &writefds, &exceptfds, &timeout);

  // Typical errors: (numSocketsReady == SOCKET_ERROR || numSocketsReady == -1)
  // GetLastError reports the problem
  // number of valid sockets matches connected sockets (just 1 in this case)
  if ( (numSocketsReady == 1)
      &&  FD_ISSET(m_socket, &writefds)
      && !FD_ISSET(m_socket, &exceptfds) )
    return true;

  return false;
}


bool
ORVuCtrlImp::Ping() const
{
  const int bufsiz = BUFSIZ;
  char rspbuf[BUFSIZ] = "";
  return GetStatus(rspbuf, bufsiz);
}


int
ORVuCtrlImp::GetLastErr() const
{
  return WSAGetLastError();
}


bool
ORVuCtrlImp::GetStatus(char *rspbuf, int bufsiz) const
{
  int bytesRecv = SOCKET_ERROR;
  int bytesSent = send(m_socket, "GS", int(strlen("GS")), 0);
  bytesRecv = recv(m_socket, rspbuf, bufsiz, 0);
  return (bytesRecv > 0);   //  bytesRecv != SOCKET_ERROR)
}


/*
bool
ORVuCtrlImp::SendCommand(const char *cmdstr, char *rspbuf, int bufsiz) const {
  int bytesRecv = SOCKET_ERROR;
  int bytesSent = send(m_socket, cmdstr, BUFSIZ, 0);
  bytesRecv = recv(m_socket, rspbuf, bufsiz, 0);
  if (bytesRecv > 0 && bytesRecv != SOCKET_ERROR)
    rspbuf[bytesRecv] = '\0'; // Add null terminating char
    return true;
  return false;
}
*/


float
ORVuCtrlImp::Version() const
{
  char respbuf[BUFSIZ] = "";
  if (GetStatus(respbuf, BUFSIZ)) {
    // Status string includes version of Arduino server
    // ORVuCtrl v1.234
    float fver = 0;
    if (sscanf_s(respbuf, "ORVuCtrl v%f", &fver) > 0) {
      return (fver);
    }
  }

  return -1;
}

int
ORVuCtrlImp::GetNumLEDs() const
{
  return 0;
}


bool
ORVuCtrlImp::GetLEDConfig(int *rspbuf, int bufsiz)
{
  return false;
}


bool
ORVuCtrlImp::SetLEDConfig(const int *rspbuf, int bufsiz)
{
  return false;
}

float
ORVuCtrlImp::GetNowTime()
{
  return false;
}


bool
ORVuCtrlImp::ExchangeState(const ORVCtrlState &out_state, ORVCtrlState &in_state) const
{
  // Out header = X$
  // In  header = x#
  int bytesRecv = SOCKET_ERROR;
  ORVCtrlUnion cu;
  cu.bit = out_state;
  sprintf_s(p_str_buffer, BUFLEN, "%s%08X", "X$", cu.val);
  int bytesSent = send(m_socket, p_str_buffer, int(strlen(p_str_buffer)), 0);
  bytesRecv = recv(m_socket, p_str_buffer, BUFLEN, 0);
  if (bytesRecv > 0) {
    if ( (bytesRecv == 10) && (p_str_buffer[0] == 'x') && (p_str_buffer[1] == '#') ) {
      int ns = sscanf_s(&p_str_buffer[2], "%x", &cu.val);
      if (ns == 1) {
        in_state = cu.bit;
        return true;
      }
    }
  }
  return false;
}

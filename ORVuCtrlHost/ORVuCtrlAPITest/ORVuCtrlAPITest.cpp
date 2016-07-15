// ORVuCtrlAPITest.cpp : Defines the entry point for the console application.
//

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "..\\ORVuCtrlAPI.h"
//#include "Version.h"

static void print_options();

int
main(int argc, char *argv[])
{
  //printf("\n%s v%s\n\tusing ORVuCtrl DLL v%5.3f", PRODUCT_NAME, STRING_VERSION, ORVuCtrl_Version()*0.001f);

  const char *IPaddrs = "127.0.0.1";
  if (argc > 1)
    IPaddrs = argv[1];

  printf("\nIP address set to: %s\n", IPaddrs);

  // Attempt connection
  ORVuHNDL orvucHndl = ORVuCtrl_Connect(IPaddrs);
  if (!orvucHndl) {
    printf("\a\n\nConnection failed.\n\t ");
    return -1;
  }

  printf("\nBegin testing. \nEnter the [F2] key for a list of testing options.\n");

  // Initialize vars for switch-case loop
  const int BUFLEN = 2048;
  bool testing = true;
  int outp = 5;   // Test output
  int inp = 2;    // Test input
  float control = 0.750;
  int inp_resp = -1;
  float fl_resp = -1;
  int which = 0;  // Version() selector
  char buff[BUFLEN] = "";
  char cmdstr[BUFSIZ] = "";
  char *pb = cmdstr;
  int i = 0;
  bool success = false;
  int err_code = 0;
  int key = 0;

  // Begin testing
  while (testing) {
    char buff[BUFLEN] = ""; // Reset response buffer after each new instruction

    // Grab and display char on console
    printf("\n");
    int key = _getch();
    _putch(key);

    switch (key) {
      // capture extended keys
      //   Function keys are prefaced with 0
      //   All other extended keys are prefaced with 224
#if 0
    case 0: case 224:
    {
      key = _getch();
      switch (key) {
        case 60: // Print testing options (F2)
          print_options();
          break;
        default:
          printf("\nUnrecognized extended key %d.", key);
          break;
      }
      key = _getch();
      break;
    }
#endif
    case 60: // Print testing options (F2)
      print_options();
      break;

    case 27: // [Escape] key
      testing = false;
      break;

    case 'v':  case 'V':   // Get version
      printf("\nLocal Version: %f\n", ORVuCtrl_Version(orvucHndl, ORVuVER_DLL));
      printf("Remote Version: %.4f\n", ORVuCtrl_Version(orvucHndl, ORVuVER_REMOTE));
      break;

    case 'e':  case 'E':   // Get last error
      inp_resp = ORVuCtrl_GetLastError(orvucHndl);
      if (inp_resp < 0) printf("\nUnable to retrieve last error.\n");
      else              printf("\nLast error: %i\n", inp_resp);
      break;

    case '1':   // Ping
      printf("\nPinging ORVuCtrl...%s.\n", (ORVuCtrl_Ping(orvucHndl) ? "SUCCESS" : "FAILED"));
      break;

    case '2':   // Check for valid connection
      // Default error code of 0 most likely indicates timeout
      // Look at WSA error codes for further details
      printf("\nTesting for valid connection...\n");
      if (ORVuCtrl_IsValid(orvucHndl))
        printf("SUCCESS.\n");
      else
        printf("FAILED. (Error code: %i)\n", err_code);
      break;

    case '4':   // Get status report
      success = ORVuCtrl_GetStatus(orvucHndl, buff, BUFLEN);
      if (success)  printf("\n%s\n", buff);
      else          printf("\nUnable to retrieve status.\n");
      break;

    default:  // Invalid command
      printf("\nKey [%d] not recognized by any command.\n"
             "Enter the [F2] key for a list of testing options.\n",key);
      break;
    }
  }
  ORVuCtrl_Disconnect(orvucHndl);
  return 0;
}

static
void
print_options()
{
  printf(
    "\n ------------------------------------"
    "\n Enter one of the following keys:\n"
    "\n ------------------------------------"
  "\n\n   [h] Help"
    "\n   [Esc] Quit\n"
    "\n   [v] Get local and remote ORVuCtrl version"
    "\n   [e] Get last error"
    "\n   [1] Ping ORVuCtrl"
    "\n   [2] Check for valid connection"
    "\n   [4] Get status report"
    "\n   [9] Set ORVuCtrl command\n"
    );
  return;
}




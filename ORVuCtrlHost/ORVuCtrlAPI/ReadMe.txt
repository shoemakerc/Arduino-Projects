========================================================================
    DYNAMIC LINK LIBRARY : PeaveyNetAPI Project Overview
========================================================================

Notes to Nayely

  Emulator
    main
      Declare local variables as close to where they are used as possible.
        By limiting scope of a variable, you limit bugs.
        In old C, variables had to be declared at the top of their scope ... no choice.
        To limit the scope, you could use { } around code.
        In C++, variables have scope from their point of declaration to where they lose scope.
        "total_bytes_received" does not need to be declared until you enter your while loop,
          right next to "reset_connecion".
      map object
        This can be cleaned-up significantly.  
          First, create a typedef for "map <string, inputAndControls> foo"
               typedef map <string, inputAndControls> MapOutToIn;
            Then you can handily use that definition in your code.
          Next you want to use the program arguments to set the emulator to 
            Dome Room or Distance Hall.
          Then according to DR or DH, you need to completely populate your map
            using a for-loop and sprintf_s to create your strings.
          Finally, choose a name distinctively from "foo".
            "foo" and "foobar" are notorious bogus names meant to be temporary code
            that is destined to be deleted.
      Variable name strategy
        At AuSIM, we try for consistency in name format
          MixedCaseName is a publically accessible function, method, or variable.
          _lowercase is a global variable, usually reserved for c-library
          x_lowercase is used for private class variables, where the 'x' indicates
            the basic type of variable.  
          ALLCAPS_NAME is a macro
          lowerUPPER_NAME is typically an enumeration
        So when I see 'm_socket' in a main, I know you did not mean it to be within our 
          naming convention.  'm_' would indicate it was a member of something, and thus
          not accessible in a main().
          For a variable with function scope like this, you could say "my_socket"
          When I pass a variable into a function, I prefer to change the name inside the function.
            This is just a personal preference so I do not confuse code between the calling
            scope and the called scope.
    initWinsock
      Inside this function returning void, you trap on the result of WSAStartup, 
      but cannot pass the event back to the caller with a void return.
      If WSAStartup fails, you probably do not want to proceed in the main() function after
      you return.
      What would make this function useful as an independent function wrapping WSAStartup, would be to provide a verbose mode where contents of WSADATA was displayed or tested,
        as well as to return a bool.
    bindSocket
      bind_socket appears to be the same value as m_socket.  
      If bind() fails, you want to return a false and trap on it in your main so you do
      not try to continue on an unbound socket.
      There are some old functions in the std-c-library that return a value that is passed as
        a parameter.  But those are frowned on and no longer replicated in new interfaces.
    getStatus
      Returns a pointer to a local address.
          Such an address is not guarranteed to have persistence beyond the return.  It might
          in one compiler, but not in the next.
          The solution options are:
             * declare the internal buffer static so it has persistence
             * pass in a buffer to be copied into
      Keys Dome Room from Distance Hall by IPAddress
        No good way to test the DR vs DH
        One solution option:
          Make a command line argument to choose DR or DH at emulator launch

AppWizard has created this PeaveyNetAPI DLL for you.

This file contains a summary of what you will find in each of the files that
make up your PeaveyNetAPI application.


PeaveyNetAPI.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

PeaveyNetAPI.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

PeaveyNetAPI.cpp
    This is the main DLL source file.

	When created, this DLL does not export any symbols. As a result, it
	will not produce a .lib file when it is built. If you wish this project
	to be a project dependency of some other project, you will either need to
	add code to export some symbols from the DLL so that an export library
	will be produced, or you can set the Ignore Input Library property to Yes
	on the General propert page of the Linker folder in the project's Property
	Pages dialog box.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named PeaveyNetAPI.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////

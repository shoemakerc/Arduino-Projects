#ifndef __VERSION_H__
#define __VERSION_H__

#ifndef ORVUCTRL_API_H
 #include "..\\ORVuCtrlAPI.h"  // device enum
#endif

#define PRODUCT_NAME     "ORVuCtrlAPI"
#define MAJOR_VERSION    ORVuCTRL_API_VERSION_MAJOR
#define MINOR_VERSION    ORVuCTRL_API_VERSION_MINOR
#define SUBMINOR_VERSION 00
#define BUILD_NUMBER     12320
// Perforce $Change: 12326 $

#define xstr_ver(major,minor,subminor) str_ver(major,minor,subminor)
#define str_ver(major,minor,subminor) #major "." #minor #subminor
#define STRING_VERSION xstr_ver(MAJOR_VERSION, MINOR_VERSION, SUBMINOR_VERSION)

#define VERSION (MAJOR_VERSION + MINOR_VERSION*0.1 + SUBMINOR_VERSION*0.001)



#endif //__VERSION_H__

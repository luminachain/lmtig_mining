





























#ifndef ADL_SDK_H_
#define ADL_SDK_H_

#include "adl_structures.h"

#if defined (LINUX)
#define __stdcall
#endif 


typedef void* ( __stdcall *ADL_MAIN_MALLOC_CALLBACK )( int );


#endif 





#ifndef HWLOC_CONFIG_H
#define HWLOC_CONFIG_H

#define HWLOC_VERSION "2.11.2"
#define HWLOC_VERSION_MAJOR 2
#define HWLOC_VERSION_MINOR 11
#define HWLOC_VERSION_RELEASE 2
#define HWLOC_VERSION_GREEK ""

#define __hwloc_restrict
#define __hwloc_inline __inline

#define __hwloc_attribute_unused
#define __hwloc_attribute_malloc
#define __hwloc_attribute_const
#define __hwloc_attribute_pure
#define __hwloc_attribute_deprecated
#define __hwloc_attribute_may_alias
#define __hwloc_attribute_warn_unused_result


#define HWLOC_HAVE_WINDOWS_H 1
#define hwloc_pid_t HANDLE
#define hwloc_thread_t HANDLE

#include <windows.h>
#include <BaseTsd.h>
typedef DWORDLONG hwloc_uint64_t;

#if defined( _USRDLL ) 
#if defined( DECLSPEC_EXPORTS )
#define HWLOC_DECLSPEC __declspec(dllexport)
#else
#define HWLOC_DECLSPEC __declspec(dllimport)
#endif
#else 
#define HWLOC_DECLSPEC
#endif


#define HWLOC_SYM_TRANSFORM 0


#define HWLOC_SYM_PREFIX hwloc_


#define HWLOC_SYM_PREFIX_CAPS HWLOC_

#endif 

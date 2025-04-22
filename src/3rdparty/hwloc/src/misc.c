

#include "private/autogen/config.h"
#include "private/private.h"
#include "private/misc.h"

#include <stdarg.h>
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#ifdef HAVE_PROGRAM_INVOCATION_NAME
#include <errno.h>
extern char *program_invocation_name;
#endif
#ifdef HAVE___PROGNAME
extern char *__progname;
#endif

#ifndef HWLOC_HAVE_CORRECT_SNPRINTF
int hwloc_snprintf(char *str, size_t size, const char *format, ...)
{
  int ret;
  va_list ap;
  static char bin;
  size_t fakesize;
  char *fakestr;

  
  if (!size) {
    str = &bin;
    size = 1;
  }

  va_start(ap, format);
  ret = vsnprintf(str, size, format, ap);
  va_end(ap);

  if (ret >= 0 && (size_t) ret != size-1)
    return ret;

  

  fakesize = size;
  fakestr = NULL;
  do {
    fakesize *= 2;
    free(fakestr);
    fakestr = malloc(fakesize);
    if (NULL == fakestr)
      return -1;
    va_start(ap, format);
    errno = 0;
    ret = vsnprintf(fakestr, fakesize, format, ap);
    va_end(ap);
  } while ((size_t) ret == fakesize-1 || (ret < 0 && (!errno || errno == ERANGE)));

  if (ret >= 0 && size) {
    if (size > (size_t) ret+1)
      size = ret+1;
    memcpy(str, fakestr, size-1);
    str[size-1] = 0;
  }
  free(fakestr);

  return ret;
}
#endif

void hwloc_add_uname_info(struct hwloc_topology *topology __hwloc_attribute_unused,
			  void *cached_uname __hwloc_attribute_unused)
{
#ifdef HAVE_UNAME
  struct utsname _utsname, *utsname;

  if (hwloc_obj_get_info_by_name(topology->levels[0][0], "OSName"))
    
    return;

  if (cached_uname)
    utsname = (struct utsname *) cached_uname;
  else {
    utsname = &_utsname;
    if (uname(utsname) < 0)
      return;
  }

  if (*utsname->sysname)
    hwloc_obj_add_info(topology->levels[0][0], "OSName", utsname->sysname);
  if (*utsname->release)
    hwloc_obj_add_info(topology->levels[0][0], "OSRelease", utsname->release);
  if (*utsname->version)
    hwloc_obj_add_info(topology->levels[0][0], "OSVersion", utsname->version);
  if (*utsname->nodename)
    hwloc_obj_add_info(topology->levels[0][0], "HostName", utsname->nodename);
  if (*utsname->machine)
    hwloc_obj_add_info(topology->levels[0][0], "Architecture", utsname->machine);
#endif 
}

char *
hwloc_progname(struct hwloc_topology *topology __hwloc_attribute_unused)
{
#if (defined HAVE_DECL_GETMODULEFILENAME) && HAVE_DECL_GETMODULEFILENAME
  char name[256], *local_basename;
  unsigned res = GetModuleFileName(NULL, name, sizeof(name));
  if (res == sizeof(name) || !res)
    return NULL;
  local_basename = strrchr(name, '\\');
  if (!local_basename)
    local_basename = name;
  else
    local_basename++;
  return strdup(local_basename);
#else 
  const char *name, *local_basename;
#if HAVE_DECL_GETPROGNAME
  name = getprogname(); 
#elif HAVE_DECL_GETEXECNAME
  name = getexecname(); 
#elif defined HAVE_PROGRAM_INVOCATION_NAME
  name = program_invocation_name; 
  
#elif defined HAVE___PROGNAME
  name = __progname; 
#else
  
  
  name = NULL;
#endif
  if (!name)
    return NULL;
  local_basename = strrchr(name, '/');
  if (!local_basename)
    local_basename = name;
  else
    local_basename++;
  return strdup(local_basename);
#endif 
}

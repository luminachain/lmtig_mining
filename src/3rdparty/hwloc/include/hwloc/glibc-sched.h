



#ifndef HWLOC_GLIBC_SCHED_H
#define HWLOC_GLIBC_SCHED_H

#include "hwloc.h"
#include "hwloc/helper.h"

#include <assert.h>

#if !defined _GNU_SOURCE || (!defined _SCHED_H && !defined _SCHED_H_) || (!defined CPU_SETSIZE && !defined sched_priority)
#error Please make sure to include sched.h before including glibc-sched.h, and define _GNU_SOURCE before any inclusion of sched.h
#endif


#ifdef __cplusplus
extern "C" {
#endif


#ifdef HWLOC_HAVE_CPU_SET






static __hwloc_inline int
hwloc_cpuset_to_glibc_sched_affinity(hwloc_topology_t topology __hwloc_attribute_unused, hwloc_const_cpuset_t hwlocset,
				    cpu_set_t *schedset, size_t schedsetsize)
{
#ifdef CPU_ZERO_S
  unsigned cpu;
  CPU_ZERO_S(schedsetsize, schedset);
  hwloc_bitmap_foreach_begin(cpu, hwlocset)
    CPU_SET_S(cpu, schedsetsize, schedset);
  hwloc_bitmap_foreach_end();
#else 
  unsigned cpu;
  CPU_ZERO(schedset);
  assert(schedsetsize == sizeof(cpu_set_t));
  hwloc_bitmap_foreach_begin(cpu, hwlocset)
    CPU_SET(cpu, schedset);
  hwloc_bitmap_foreach_end();
#endif 
  return 0;
}


static __hwloc_inline int
hwloc_cpuset_from_glibc_sched_affinity(hwloc_topology_t topology __hwloc_attribute_unused, hwloc_cpuset_t hwlocset,
                                       const cpu_set_t *schedset, size_t schedsetsize)
{
  int cpu;
#ifdef CPU_ZERO_S
  int count;
#endif
  hwloc_bitmap_zero(hwlocset);
#ifdef CPU_ZERO_S
  count = CPU_COUNT_S(schedsetsize, schedset);
  cpu = 0;
  while (count) {
    if (CPU_ISSET_S(cpu, schedsetsize, schedset)) {
      if (hwloc_bitmap_set(hwlocset, cpu) < 0)
        return -1;
      count--;
    }
    cpu++;
  }
#else 
  
  assert(schedsetsize == sizeof(cpu_set_t));
  for(cpu=0; cpu<CPU_SETSIZE; cpu++)
    if (CPU_ISSET(cpu, schedset))
      if (hwloc_bitmap_set(hwlocset, cpu) < 0)
        return -1;
#endif 
  return 0;
}




#endif 


#ifdef __cplusplus
} 
#endif


#endif 

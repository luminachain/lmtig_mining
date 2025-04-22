



#ifndef HWLOC_WINDOWS_H
#define HWLOC_WINDOWS_H

#include "hwloc.h"


#ifdef __cplusplus
extern "C" {
#endif






HWLOC_DECLSPEC int hwloc_windows_get_nr_processor_groups(hwloc_topology_t topology, unsigned long flags);


HWLOC_DECLSPEC int hwloc_windows_get_processor_group_cpuset(hwloc_topology_t topology, unsigned pg_index, hwloc_cpuset_t cpuset, unsigned long flags);




#ifdef __cplusplus
} 
#endif


#endif 

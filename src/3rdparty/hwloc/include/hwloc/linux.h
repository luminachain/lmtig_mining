



#ifndef HWLOC_LINUX_H
#define HWLOC_LINUX_H

#include "hwloc.h"

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif





HWLOC_DECLSPEC int hwloc_linux_set_tid_cpubind(hwloc_topology_t topology, pid_t tid, hwloc_const_cpuset_t set);


HWLOC_DECLSPEC int hwloc_linux_get_tid_cpubind(hwloc_topology_t topology, pid_t tid, hwloc_cpuset_t set);


HWLOC_DECLSPEC int hwloc_linux_get_tid_last_cpu_location(hwloc_topology_t topology, pid_t tid, hwloc_bitmap_t set);


HWLOC_DECLSPEC int hwloc_linux_read_path_as_cpumask(const char *path, hwloc_bitmap_t set);




#ifdef __cplusplus
} 
#endif


#endif 

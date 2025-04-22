



#ifndef HWLOC_OPENFABRICS_VERBS_H
#define HWLOC_OPENFABRICS_VERBS_H

#include "hwloc.h"
#include "hwloc/autogen/config.h"
#ifdef HWLOC_LINUX_SYS
#include "hwloc/linux.h"
#endif

#include <infiniband/verbs.h>


#ifdef __cplusplus
extern "C" {
#endif





static __hwloc_inline int
hwloc_ibv_get_device_cpuset(hwloc_topology_t topology __hwloc_attribute_unused,
			    struct ibv_device *ibdev, hwloc_cpuset_t set)
{
#ifdef HWLOC_LINUX_SYS
  
#define HWLOC_OPENFABRICS_VERBS_SYSFS_PATH_MAX 128
  char path[HWLOC_OPENFABRICS_VERBS_SYSFS_PATH_MAX];

  if (!hwloc_topology_is_thissystem(topology)) {
    errno = EINVAL;
    return -1;
  }

  sprintf(path, "/sys/class/infiniband/%s/device/local_cpus",
	  ibv_get_device_name(ibdev));
  if (hwloc_linux_read_path_as_cpumask(path, set) < 0
      || hwloc_bitmap_iszero(set))
    hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#else
  
  hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#endif
  return 0;
}


static __hwloc_inline hwloc_obj_t
hwloc_ibv_get_device_osdev_by_name(hwloc_topology_t topology,
				   const char *ibname)
{
	hwloc_obj_t osdev = NULL;
	while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
		if (HWLOC_OBJ_OSDEV_OPENFABRICS == osdev->attr->osdev.type
		    && osdev->name && !strcmp(ibname, osdev->name))
			return osdev;
	}
	return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_ibv_get_device_osdev(hwloc_topology_t topology,
			   struct ibv_device *ibdev)
{
	if (!hwloc_topology_is_thissystem(topology)) {
		errno = EINVAL;
		return NULL;
	}
	return hwloc_ibv_get_device_osdev_by_name(topology, ibv_get_device_name(ibdev));
}




#ifdef __cplusplus
} 
#endif


#endif 

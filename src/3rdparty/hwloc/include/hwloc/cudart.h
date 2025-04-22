



#ifndef HWLOC_CUDART_H
#define HWLOC_CUDART_H

#include "hwloc.h"
#include "hwloc/autogen/config.h"
#include "hwloc/helper.h"
#ifdef HWLOC_LINUX_SYS
#include "hwloc/linux.h"
#endif

#include <cuda.h> 
#include <cuda_runtime_api.h>


#ifdef __cplusplus
extern "C" {
#endif





static __hwloc_inline int
hwloc_cudart_get_device_pci_ids(hwloc_topology_t topology __hwloc_attribute_unused,
				int idx, int *domain, int *bus, int *dev)
{
  cudaError_t cerr;
  struct cudaDeviceProp prop;

  cerr = cudaGetDeviceProperties(&prop, idx);
  if (cerr) {
    errno = ENOSYS;
    return -1;
  }

#if CUDA_VERSION >= 4000
  *domain = prop.pciDomainID;
#else
  *domain = 0;
#endif

  *bus = prop.pciBusID;
  *dev = prop.pciDeviceID;

  return 0;
}


static __hwloc_inline int
hwloc_cudart_get_device_cpuset(hwloc_topology_t topology __hwloc_attribute_unused,
			       int idx, hwloc_cpuset_t set)
{
#ifdef HWLOC_LINUX_SYS
  
#define HWLOC_CUDART_DEVICE_SYSFS_PATH_MAX 128
  char path[HWLOC_CUDART_DEVICE_SYSFS_PATH_MAX];
  int domain, bus, dev;

  if (hwloc_cudart_get_device_pci_ids(topology, idx, &domain, &bus, &dev))
    return -1;

  if (!hwloc_topology_is_thissystem(topology)) {
    errno = EINVAL;
    return -1;
  }

  sprintf(path, "/sys/bus/pci/devices/%04x:%02x:%02x.0/local_cpus", (unsigned) domain, (unsigned) bus, (unsigned) dev);
  if (hwloc_linux_read_path_as_cpumask(path, set) < 0
      || hwloc_bitmap_iszero(set))
    hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#else
  
  hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#endif
  return 0;
}


static __hwloc_inline hwloc_obj_t
hwloc_cudart_get_device_pcidev(hwloc_topology_t topology, int idx)
{
  int domain, bus, dev;

  if (hwloc_cudart_get_device_pci_ids(topology, idx, &domain, &bus, &dev))
    return NULL;

  return hwloc_get_pcidev_by_busid(topology, domain, bus, dev, 0);
}


static __hwloc_inline hwloc_obj_t
hwloc_cudart_get_device_osdev_by_index(hwloc_topology_t topology, unsigned idx)
{
	hwloc_obj_t osdev = NULL;
	while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
		if (HWLOC_OBJ_OSDEV_COPROC == osdev->attr->osdev.type
		    && osdev->name
		    && !strncmp("cuda", osdev->name, 4)
		    && atoi(osdev->name + 4) == (int) idx)
			return osdev;
	}
	return NULL;
}




#ifdef __cplusplus
} 
#endif


#endif 

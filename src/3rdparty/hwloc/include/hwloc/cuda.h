



#ifndef HWLOC_CUDA_H
#define HWLOC_CUDA_H

#include "hwloc.h"
#include "hwloc/autogen/config.h"
#include "hwloc/helper.h"
#ifdef HWLOC_LINUX_SYS
#include "hwloc/linux.h"
#endif

#include <cuda.h>


#ifdef __cplusplus
extern "C" {
#endif





static __hwloc_inline int
hwloc_cuda_get_device_pci_ids(hwloc_topology_t topology __hwloc_attribute_unused,
			      CUdevice cudevice, int *domain, int *bus, int *dev)
{
  CUresult cres;

#if CUDA_VERSION >= 4000
  cres = cuDeviceGetAttribute(domain, CU_DEVICE_ATTRIBUTE_PCI_DOMAIN_ID, cudevice);
  if (cres != CUDA_SUCCESS) {
    errno = ENOSYS;
    return -1;
  }
#else
  *domain = 0;
#endif
  cres = cuDeviceGetAttribute(bus, CU_DEVICE_ATTRIBUTE_PCI_BUS_ID, cudevice);
  if (cres != CUDA_SUCCESS) {
    errno = ENOSYS;
    return -1;
  }
  cres = cuDeviceGetAttribute(dev, CU_DEVICE_ATTRIBUTE_PCI_DEVICE_ID, cudevice);
  if (cres != CUDA_SUCCESS) {
    errno = ENOSYS;
    return -1;
  }

  return 0;
}


static __hwloc_inline int
hwloc_cuda_get_device_cpuset(hwloc_topology_t topology __hwloc_attribute_unused,
			     CUdevice cudevice, hwloc_cpuset_t set)
{
#ifdef HWLOC_LINUX_SYS
  
#define HWLOC_CUDA_DEVICE_SYSFS_PATH_MAX 128
  char path[HWLOC_CUDA_DEVICE_SYSFS_PATH_MAX];
  int domainid, busid, deviceid;

  if (hwloc_cuda_get_device_pci_ids(topology, cudevice, &domainid, &busid, &deviceid))
    return -1;

  if (!hwloc_topology_is_thissystem(topology)) {
    errno = EINVAL;
    return -1;
  }

  sprintf(path, "/sys/bus/pci/devices/%04x:%02x:%02x.0/local_cpus", domainid, busid, deviceid);
  if (hwloc_linux_read_path_as_cpumask(path, set) < 0
      || hwloc_bitmap_iszero(set))
    hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#else
  
  hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#endif
  return 0;
}


static __hwloc_inline hwloc_obj_t
hwloc_cuda_get_device_pcidev(hwloc_topology_t topology, CUdevice cudevice)
{
  int domain, bus, dev;

  if (hwloc_cuda_get_device_pci_ids(topology, cudevice, &domain, &bus, &dev))
    return NULL;

  return hwloc_get_pcidev_by_busid(topology, domain, bus, dev, 0);
}


static __hwloc_inline hwloc_obj_t
hwloc_cuda_get_device_osdev(hwloc_topology_t topology, CUdevice cudevice)
{
	hwloc_obj_t osdev = NULL;
	int domain, bus, dev;

	if (hwloc_cuda_get_device_pci_ids(topology, cudevice, &domain, &bus, &dev))
		return NULL;

	osdev = NULL;
	while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
		hwloc_obj_t pcidev = osdev->parent;
		if (strncmp(osdev->name, "cuda", 4))
			continue;
		if (pcidev
		    && pcidev->type == HWLOC_OBJ_PCI_DEVICE
		    && (int) pcidev->attr->pcidev.domain == domain
		    && (int) pcidev->attr->pcidev.bus == bus
		    && (int) pcidev->attr->pcidev.dev == dev
		    && pcidev->attr->pcidev.func == 0)
			return osdev;
		
	}

	return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_cuda_get_device_osdev_by_index(hwloc_topology_t topology, unsigned idx)
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

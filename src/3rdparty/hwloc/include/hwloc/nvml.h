



#ifndef HWLOC_NVML_H
#define HWLOC_NVML_H

#include "hwloc.h"
#include "hwloc/autogen/config.h"
#include "hwloc/helper.h"
#ifdef HWLOC_LINUX_SYS
#include "hwloc/linux.h"
#endif

#include <nvml.h>


#ifdef __cplusplus
extern "C" {
#endif





static __hwloc_inline int
hwloc_nvml_get_device_cpuset(hwloc_topology_t topology __hwloc_attribute_unused,
			     nvmlDevice_t device, hwloc_cpuset_t set)
{
#ifdef HWLOC_LINUX_SYS
  
#define HWLOC_NVML_DEVICE_SYSFS_PATH_MAX 128
  char path[HWLOC_NVML_DEVICE_SYSFS_PATH_MAX];
  nvmlReturn_t nvres;
  nvmlPciInfo_t pci;

  if (!hwloc_topology_is_thissystem(topology)) {
    errno = EINVAL;
    return -1;
  }

  nvres = nvmlDeviceGetPciInfo(device, &pci);
  if (NVML_SUCCESS != nvres) {
    errno = EINVAL;
    return -1;
  }

  sprintf(path, "/sys/bus/pci/devices/%04x:%02x:%02x.0/local_cpus", pci.domain, pci.bus, pci.device);
  if (hwloc_linux_read_path_as_cpumask(path, set) < 0
      || hwloc_bitmap_iszero(set))
    hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#else
  
  hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#endif
  return 0;
}


static __hwloc_inline hwloc_obj_t
hwloc_nvml_get_device_osdev_by_index(hwloc_topology_t topology, unsigned idx)
{
	hwloc_obj_t osdev = NULL;
	while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
                if (HWLOC_OBJ_OSDEV_GPU == osdev->attr->osdev.type
                    && osdev->name
		    && !strncmp("nvml", osdev->name, 4)
		    && atoi(osdev->name + 4) == (int) idx)
                        return osdev;
        }
        return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_nvml_get_device_osdev(hwloc_topology_t topology, nvmlDevice_t device)
{
	hwloc_obj_t osdev;
	nvmlReturn_t nvres;
	nvmlPciInfo_t pci;
	char uuid[64];

	if (!hwloc_topology_is_thissystem(topology)) {
		errno = EINVAL;
		return NULL;
	}

	nvres = nvmlDeviceGetPciInfo(device, &pci);
	if (NVML_SUCCESS != nvres)
		return NULL;

	nvres = nvmlDeviceGetUUID(device, uuid, sizeof(uuid));
	if (NVML_SUCCESS != nvres)
		uuid[0] = '\0';

	osdev = NULL;
	while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
		hwloc_obj_t pcidev = osdev->parent;
		const char *info;

		if (strncmp(osdev->name, "nvml", 4))
			continue;

		if (pcidev
		    && pcidev->type == HWLOC_OBJ_PCI_DEVICE
		    && pcidev->attr->pcidev.domain == pci.domain
		    && pcidev->attr->pcidev.bus == pci.bus
		    && pcidev->attr->pcidev.dev == pci.device
		    && pcidev->attr->pcidev.func == 0)
			return osdev;

		info = hwloc_obj_get_info_by_name(osdev, "NVIDIAUUID");
		if (info && !strcmp(info, uuid))
			return osdev;
	}

	return NULL;
}




#ifdef __cplusplus
} 
#endif


#endif 

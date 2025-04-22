



#ifndef HWLOC_LEVELZERO_H
#define HWLOC_LEVELZERO_H

#include "hwloc.h"
#include "hwloc/autogen/config.h"
#include "hwloc/helper.h"
#ifdef HWLOC_LINUX_SYS
#include "hwloc/linux.h"
#endif

#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>


#ifdef __cplusplus
extern "C" {
#endif





static __hwloc_inline int
hwloc_levelzero_get_device_cpuset(hwloc_topology_t topology __hwloc_attribute_unused,
                                  ze_device_handle_t device, hwloc_cpuset_t set)
{
#ifdef HWLOC_LINUX_SYS
  
#define HWLOC_LEVELZERO_DEVICE_SYSFS_PATH_MAX 128
  char path[HWLOC_LEVELZERO_DEVICE_SYSFS_PATH_MAX];
  zes_pci_properties_t pci;
  zes_device_handle_t sdevice = device;
  ze_result_t res;

  if (!hwloc_topology_is_thissystem(topology)) {
    errno = EINVAL;
    return -1;
  }

  res = zesDevicePciGetProperties(sdevice, &pci);
  if (res != ZE_RESULT_SUCCESS) {
    errno = EINVAL;
    return -1;
  }

  sprintf(path, "/sys/bus/pci/devices/%04x:%02x:%02x.%01x/local_cpus",
          pci.address.domain, pci.address.bus, pci.address.device, pci.address.function);
  if (hwloc_linux_read_path_as_cpumask(path, set) < 0
      || hwloc_bitmap_iszero(set))
    hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#else
  
  hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#endif
  return 0;
}


static __hwloc_inline hwloc_obj_t
hwloc_levelzero_get_device_osdev(hwloc_topology_t topology, ze_device_handle_t device)
{
  zes_device_handle_t sdevice = device;
  zes_pci_properties_t pci;
  ze_result_t res;
  hwloc_obj_t osdev;

  if (!hwloc_topology_is_thissystem(topology)) {
    errno = EINVAL;
    return NULL;
  }

  res = zesDevicePciGetProperties(sdevice, &pci);
  if (res != ZE_RESULT_SUCCESS) {
    
    errno = EINVAL;
    return NULL;
  }

  osdev = NULL;
  while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
    hwloc_obj_t pcidev = osdev->parent;

    if (strncmp(osdev->name, "ze", 2))
      continue;

    if (pcidev
      && pcidev->type == HWLOC_OBJ_PCI_DEVICE
      && pcidev->attr->pcidev.domain == pci.address.domain
      && pcidev->attr->pcidev.bus == pci.address.bus
      && pcidev->attr->pcidev.dev == pci.address.device
      && pcidev->attr->pcidev.func == pci.address.function)
      return osdev;

    
  }

  return NULL;
}




#ifdef __cplusplus
} 
#endif


#endif 

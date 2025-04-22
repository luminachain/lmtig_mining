



#ifndef HWLOC_OPENCL_H
#define HWLOC_OPENCL_H

#include "hwloc.h"
#include "hwloc/autogen/config.h"
#include "hwloc/helper.h"
#ifdef HWLOC_LINUX_SYS
#include "hwloc/linux.h"
#endif

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif






typedef struct {
    cl_uint pci_domain;
    cl_uint pci_bus;
    cl_uint pci_device;
    cl_uint pci_function;
} hwloc_cl_device_pci_bus_info_khr;
#define HWLOC_CL_DEVICE_PCI_BUS_INFO_KHR 0x410F


#define HWLOC_CL_DEVICE_TOPOLOGY_AMD 0x4037
typedef union {
    struct { cl_uint type; cl_uint data[5]; } raw;
    struct { cl_uint type; cl_char unused[17]; cl_char bus; cl_char device; cl_char function; } pcie;
} hwloc_cl_device_topology_amd;
#define HWLOC_CL_DEVICE_TOPOLOGY_TYPE_PCIE_AMD 1


#define HWLOC_CL_DEVICE_PCI_BUS_ID_NV 0x4008
#define HWLOC_CL_DEVICE_PCI_SLOT_ID_NV 0x4009
#define HWLOC_CL_DEVICE_PCI_DOMAIN_ID_NV 0x400A





static __hwloc_inline int
hwloc_opencl_get_device_pci_busid(cl_device_id device,
                               unsigned *domain, unsigned *bus, unsigned *dev, unsigned *func)
{
	hwloc_cl_device_topology_amd amdtopo;
	hwloc_cl_device_pci_bus_info_khr khrbusinfo;
	cl_uint nvbus, nvslot, nvdomain;
	cl_int clret;

	clret = clGetDeviceInfo(device, HWLOC_CL_DEVICE_PCI_BUS_INFO_KHR, sizeof(khrbusinfo), &khrbusinfo, NULL);
	if (CL_SUCCESS == clret) {
		*domain = (unsigned) khrbusinfo.pci_domain;
		*bus = (unsigned) khrbusinfo.pci_bus;
		*dev = (unsigned) khrbusinfo.pci_device;
		*func = (unsigned) khrbusinfo.pci_function;
		return 0;
	}

	clret = clGetDeviceInfo(device, HWLOC_CL_DEVICE_TOPOLOGY_AMD, sizeof(amdtopo), &amdtopo, NULL);
	if (CL_SUCCESS == clret
	    && HWLOC_CL_DEVICE_TOPOLOGY_TYPE_PCIE_AMD == amdtopo.raw.type) {
		*domain = 0; 
		
		*bus = (unsigned) (unsigned char) amdtopo.pcie.bus;
		*dev = (unsigned) (unsigned char) amdtopo.pcie.device;
		*func = (unsigned) (unsigned char) amdtopo.pcie.function;
		return 0;
	}

	clret = clGetDeviceInfo(device, HWLOC_CL_DEVICE_PCI_BUS_ID_NV, sizeof(nvbus), &nvbus, NULL);
	if (CL_SUCCESS == clret) {
		clret = clGetDeviceInfo(device, HWLOC_CL_DEVICE_PCI_SLOT_ID_NV, sizeof(nvslot), &nvslot, NULL);
		if (CL_SUCCESS == clret) {
			clret = clGetDeviceInfo(device, HWLOC_CL_DEVICE_PCI_DOMAIN_ID_NV, sizeof(nvdomain), &nvdomain, NULL);
			if (CL_SUCCESS == clret) { 
				*domain = nvdomain;
			} else {
				*domain = 0;
			}
			*bus = nvbus & 0xff;
			
			*dev = nvslot >> 3;
			*func = nvslot & 0x7;
			return 0;
		}
	}

	return -1;
}


static __hwloc_inline int
hwloc_opencl_get_device_cpuset(hwloc_topology_t topology __hwloc_attribute_unused,
			       cl_device_id device __hwloc_attribute_unused,
			       hwloc_cpuset_t set)
{
#if (defined HWLOC_LINUX_SYS)
	
#define HWLOC_OPENCL_DEVICE_SYSFS_PATH_MAX 128
	char path[HWLOC_OPENCL_DEVICE_SYSFS_PATH_MAX];
	unsigned pcidomain, pcibus, pcidev, pcifunc;

	if (!hwloc_topology_is_thissystem(topology)) {
		errno = EINVAL;
		return -1;
	}

	if (hwloc_opencl_get_device_pci_busid(device, &pcidomain, &pcibus, &pcidev, &pcifunc) < 0) {
		hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
		return 0;
	}

	sprintf(path, "/sys/bus/pci/devices/%04x:%02x:%02x.%01x/local_cpus", pcidomain, pcibus, pcidev, pcifunc);
	if (hwloc_linux_read_path_as_cpumask(path, set) < 0
	    || hwloc_bitmap_iszero(set))
		hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#else
	
	hwloc_bitmap_copy(set, hwloc_topology_get_complete_cpuset(topology));
#endif
  return 0;
}


static __hwloc_inline hwloc_obj_t
hwloc_opencl_get_device_osdev_by_index(hwloc_topology_t topology,
				       unsigned platform_index, unsigned device_index)
{
	unsigned x = (unsigned) -1, y = (unsigned) -1;
	hwloc_obj_t osdev = NULL;
	while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
		if (HWLOC_OBJ_OSDEV_COPROC == osdev->attr->osdev.type
                    && osdev->name
		    && sscanf(osdev->name, "opencl%ud%u", &x, &y) == 2
		    && platform_index == x && device_index == y)
                        return osdev;
        }
        return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_opencl_get_device_osdev(hwloc_topology_t topology __hwloc_attribute_unused,
			      cl_device_id device __hwloc_attribute_unused)
{
	hwloc_obj_t osdev;
	unsigned pcidomain, pcibus, pcidevice, pcifunc;

	if (hwloc_opencl_get_device_pci_busid(device, &pcidomain, &pcibus, &pcidevice, &pcifunc) < 0) {
		errno = EINVAL;
		return NULL;
	}

	osdev = NULL;
	while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
		hwloc_obj_t pcidev = osdev->parent;
		if (strncmp(osdev->name, "opencl", 6))
			continue;
		if (pcidev
		    && pcidev->type == HWLOC_OBJ_PCI_DEVICE
		    && pcidev->attr->pcidev.domain == pcidomain
		    && pcidev->attr->pcidev.bus == pcibus
		    && pcidev->attr->pcidev.dev == pcidevice
		    && pcidev->attr->pcidev.func == pcifunc)
			return osdev;
		
	}

	return NULL;
}




#ifdef __cplusplus
} 
#endif


#endif 

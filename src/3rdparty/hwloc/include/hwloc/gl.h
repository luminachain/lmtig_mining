



#ifndef HWLOC_GL_H
#define HWLOC_GL_H

#include "hwloc.h"

#include <stdio.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif





static __hwloc_inline hwloc_obj_t
hwloc_gl_get_display_osdev_by_port_device(hwloc_topology_t topology,
					  unsigned port, unsigned device)
{
        unsigned x = (unsigned) -1, y = (unsigned) -1;
        hwloc_obj_t osdev = NULL;
        while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
                if (HWLOC_OBJ_OSDEV_GPU == osdev->attr->osdev.type
                    && osdev->name
                    && sscanf(osdev->name, ":%u.%u", &x, &y) == 2
                    && port == x && device == y)
                        return osdev;
        }
	errno = EINVAL;
        return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_gl_get_display_osdev_by_name(hwloc_topology_t topology,
				   const char *name)
{
        hwloc_obj_t osdev = NULL;
        while ((osdev = hwloc_get_next_osdev(topology, osdev)) != NULL) {
                if (HWLOC_OBJ_OSDEV_GPU == osdev->attr->osdev.type
                    && osdev->name
                    && !strcmp(name, osdev->name))
                        return osdev;
        }
	errno = EINVAL;
        return NULL;
}


static __hwloc_inline int
hwloc_gl_get_display_by_osdev(hwloc_topology_t topology __hwloc_attribute_unused,
			      hwloc_obj_t osdev,
			      unsigned *port, unsigned *device)
{
	unsigned x = -1, y = -1;
	if (HWLOC_OBJ_OSDEV_GPU == osdev->attr->osdev.type
	    && sscanf(osdev->name, ":%u.%u", &x, &y) == 2) {
		*port = x;
		*device = y;
		return 0;
	}
	errno = EINVAL;
	return -1;
}




#ifdef __cplusplus
} 
#endif


#endif 


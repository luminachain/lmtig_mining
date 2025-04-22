



#ifndef HWLOC_SHMEM_H
#define HWLOC_SHMEM_H

#include "hwloc.h"

#ifdef __cplusplus
extern "C" {
#elif 0
}
#endif





HWLOC_DECLSPEC int hwloc_shmem_topology_get_length(hwloc_topology_t topology,
						   size_t *lengthp,
						   unsigned long flags);


HWLOC_DECLSPEC int hwloc_shmem_topology_write(hwloc_topology_t topology,
					      int fd, hwloc_uint64_t fileoffset,
					      void *mmap_address, size_t length,
					      unsigned long flags);


HWLOC_DECLSPEC int hwloc_shmem_topology_adopt(hwloc_topology_t *topologyp,
					      int fd, hwloc_uint64_t fileoffset,
					      void *mmap_address, size_t length,
					      unsigned long flags);



#ifdef __cplusplus
} 
#endif


#endif 

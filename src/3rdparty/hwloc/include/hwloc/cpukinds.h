



#ifndef HWLOC_CPUKINDS_H
#define HWLOC_CPUKINDS_H

#include "hwloc.h"

#ifdef __cplusplus
extern "C" {
#elif 0
}
#endif




HWLOC_DECLSPEC int
hwloc_cpukinds_get_nr(hwloc_topology_t topology,
                      unsigned long flags);


HWLOC_DECLSPEC int
hwloc_cpukinds_get_by_cpuset(hwloc_topology_t topology,
                             hwloc_const_bitmap_t cpuset,
                             unsigned long flags);


HWLOC_DECLSPEC int
hwloc_cpukinds_get_info(hwloc_topology_t topology,
                        unsigned kind_index,
                        hwloc_bitmap_t cpuset,
                        int *efficiency,
                        unsigned *nr_infos, struct hwloc_info_s **infos,
                        unsigned long flags);


HWLOC_DECLSPEC int
hwloc_cpukinds_register(hwloc_topology_t topology,
                        hwloc_bitmap_t cpuset,
                        int forced_efficiency,
                        unsigned nr_infos, struct hwloc_info_s *infos,
                        unsigned long flags);



#ifdef __cplusplus
} 
#endif


#endif 

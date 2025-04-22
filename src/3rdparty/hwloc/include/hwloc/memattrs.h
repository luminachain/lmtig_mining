



#ifndef HWLOC_MEMATTR_H
#define HWLOC_MEMATTR_H

#include "hwloc.h"

#ifdef __cplusplus
extern "C" {
#elif 0
}
#endif




enum hwloc_memattr_id_e {
  
  HWLOC_MEMATTR_ID_CAPACITY = 0,

  
  HWLOC_MEMATTR_ID_LOCALITY = 1,

  
  HWLOC_MEMATTR_ID_BANDWIDTH = 2,

  
  HWLOC_MEMATTR_ID_READ_BANDWIDTH = 4,

  
  HWLOC_MEMATTR_ID_WRITE_BANDWIDTH = 5,

  
  HWLOC_MEMATTR_ID_LATENCY = 3,

  
  HWLOC_MEMATTR_ID_READ_LATENCY = 6,

  
  HWLOC_MEMATTR_ID_WRITE_LATENCY = 7,

  

  HWLOC_MEMATTR_ID_MAX 
};


typedef unsigned hwloc_memattr_id_t;


HWLOC_DECLSPEC int
hwloc_memattr_get_by_name(hwloc_topology_t topology,
                          const char *name,
                          hwloc_memattr_id_t *id);



enum hwloc_location_type_e {
  
  HWLOC_LOCATION_TYPE_CPUSET = 1,
  
  HWLOC_LOCATION_TYPE_OBJECT = 0
};


struct hwloc_location {
  
  enum hwloc_location_type_e type;
  
  union hwloc_location_u {
    
    hwloc_cpuset_t cpuset;
    
    hwloc_obj_t object;
  } location;
};



enum hwloc_local_numanode_flag_e {
  
  HWLOC_LOCAL_NUMANODE_FLAG_LARGER_LOCALITY = (1UL<<0),

  
  HWLOC_LOCAL_NUMANODE_FLAG_SMALLER_LOCALITY = (1UL<<1),

  
  HWLOC_LOCAL_NUMANODE_FLAG_ALL = (1UL<<2)
};


HWLOC_DECLSPEC int
hwloc_get_local_numanode_objs(hwloc_topology_t topology,
                              struct hwloc_location *location,
                              unsigned *nr,
                              hwloc_obj_t *nodes,
                              unsigned long flags);




HWLOC_DECLSPEC int
hwloc_memattr_get_value(hwloc_topology_t topology,
                        hwloc_memattr_id_t attribute,
                        hwloc_obj_t target_node,
                        struct hwloc_location *initiator,
                        unsigned long flags,
                        hwloc_uint64_t *value);


HWLOC_DECLSPEC int
hwloc_memattr_get_best_target(hwloc_topology_t topology,
                              hwloc_memattr_id_t attribute,
                              struct hwloc_location *initiator,
                              unsigned long flags,
                              hwloc_obj_t *best_target, hwloc_uint64_t *value);


HWLOC_DECLSPEC int
hwloc_memattr_get_best_initiator(hwloc_topology_t topology,
                                 hwloc_memattr_id_t attribute,
                                 hwloc_obj_t target_node,
                                 unsigned long flags,
                                 struct hwloc_location *best_initiator, hwloc_uint64_t *value);


HWLOC_DECLSPEC int
hwloc_memattr_get_targets(hwloc_topology_t topology,
                          hwloc_memattr_id_t attribute,
                          struct hwloc_location *initiator,
                          unsigned long flags,
                          unsigned *nr, hwloc_obj_t *targets, hwloc_uint64_t *values);


HWLOC_DECLSPEC int
hwloc_memattr_get_initiators(hwloc_topology_t topology,
                             hwloc_memattr_id_t attribute,
                             hwloc_obj_t target_node,
                             unsigned long flags,
                             unsigned *nr, struct hwloc_location *initiators, hwloc_uint64_t *values);







HWLOC_DECLSPEC int
hwloc_memattr_get_name(hwloc_topology_t topology,
                       hwloc_memattr_id_t attribute,
                       const char **name);


HWLOC_DECLSPEC int
hwloc_memattr_get_flags(hwloc_topology_t topology,
                        hwloc_memattr_id_t attribute,
                        unsigned long *flags);


enum hwloc_memattr_flag_e {
  
  HWLOC_MEMATTR_FLAG_HIGHER_FIRST = (1UL<<0),
  
  HWLOC_MEMATTR_FLAG_LOWER_FIRST = (1UL<<1),
  
  HWLOC_MEMATTR_FLAG_NEED_INITIATOR = (1UL<<2)
};


HWLOC_DECLSPEC int
hwloc_memattr_register(hwloc_topology_t topology,
                       const char *name,
                       unsigned long flags,
                       hwloc_memattr_id_t *id);


HWLOC_DECLSPEC int
hwloc_memattr_set_value(hwloc_topology_t topology,
                        hwloc_memattr_id_t attribute,
                        hwloc_obj_t target_node,
                        struct hwloc_location *initiator,
                        unsigned long flags,
                        hwloc_uint64_t value);



#ifdef __cplusplus
} 
#endif


#endif 

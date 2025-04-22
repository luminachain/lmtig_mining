



#ifndef HWLOC_DISTANCES_H
#define HWLOC_DISTANCES_H

#ifndef HWLOC_H
#error Please include the main hwloc.h instead
#endif


#ifdef __cplusplus
extern "C" {
#elif 0
}
#endif





struct hwloc_distances_s {
  unsigned nbobjs;		
  hwloc_obj_t *objs;		
  unsigned long kind;		
  hwloc_uint64_t *values;	
};


enum hwloc_distances_kind_e {
  
  HWLOC_DISTANCES_KIND_FROM_OS = (1UL<<0),
  
  HWLOC_DISTANCES_KIND_FROM_USER = (1UL<<1),

  
  HWLOC_DISTANCES_KIND_MEANS_LATENCY = (1UL<<2),
  
  HWLOC_DISTANCES_KIND_MEANS_BANDWIDTH = (1UL<<3),

  
  HWLOC_DISTANCES_KIND_HETEROGENEOUS_TYPES = (1UL<<4)
};


HWLOC_DECLSPEC int
hwloc_distances_get(hwloc_topology_t topology,
		    unsigned *nr, struct hwloc_distances_s **distances,
		    unsigned long kind, unsigned long flags);


HWLOC_DECLSPEC int
hwloc_distances_get_by_depth(hwloc_topology_t topology, int depth,
			     unsigned *nr, struct hwloc_distances_s **distances,
			     unsigned long kind, unsigned long flags);


HWLOC_DECLSPEC int
hwloc_distances_get_by_type(hwloc_topology_t topology, hwloc_obj_type_t type,
			    unsigned *nr, struct hwloc_distances_s **distances,
			    unsigned long kind, unsigned long flags);


HWLOC_DECLSPEC int
hwloc_distances_get_by_name(hwloc_topology_t topology, const char *name,
			    unsigned *nr, struct hwloc_distances_s **distances,
			    unsigned long flags);


HWLOC_DECLSPEC const char *
hwloc_distances_get_name(hwloc_topology_t topology, struct hwloc_distances_s *distances);


HWLOC_DECLSPEC void
hwloc_distances_release(hwloc_topology_t topology, struct hwloc_distances_s *distances);


enum hwloc_distances_transform_e {
  
  HWLOC_DISTANCES_TRANSFORM_REMOVE_NULL = 0,

  
  HWLOC_DISTANCES_TRANSFORM_LINKS = 1,

  
  HWLOC_DISTANCES_TRANSFORM_MERGE_SWITCH_PORTS = 2,

  
  HWLOC_DISTANCES_TRANSFORM_TRANSITIVE_CLOSURE = 3
};


HWLOC_DECLSPEC int hwloc_distances_transform(hwloc_topology_t topology, struct hwloc_distances_s *distances,
                                             enum hwloc_distances_transform_e transform,
                                             void *transform_attr,
                                             unsigned long flags);








static __hwloc_inline int
hwloc_distances_obj_index(struct hwloc_distances_s *distances, hwloc_obj_t obj)
{
  unsigned i;
  for(i=0; i<distances->nbobjs; i++)
    if (distances->objs[i] == obj)
      return (int)i;
  return -1;
}


static __hwloc_inline int
hwloc_distances_obj_pair_values(struct hwloc_distances_s *distances,
				hwloc_obj_t obj1, hwloc_obj_t obj2,
				hwloc_uint64_t *value1to2, hwloc_uint64_t *value2to1)
{
  int i1 = hwloc_distances_obj_index(distances, obj1);
  int i2 = hwloc_distances_obj_index(distances, obj2);
  if (i1 < 0 || i2 < 0)
    return -1;
  *value1to2 = distances->values[i1 * distances->nbobjs + i2];
  *value2to1 = distances->values[i2 * distances->nbobjs + i1];
  return 0;
}








typedef void * hwloc_distances_add_handle_t;


HWLOC_DECLSPEC hwloc_distances_add_handle_t
hwloc_distances_add_create(hwloc_topology_t topology,
                           const char *name, unsigned long kind,
                           unsigned long flags);


HWLOC_DECLSPEC int hwloc_distances_add_values(hwloc_topology_t topology,
                                              hwloc_distances_add_handle_t handle,
                                              unsigned nbobjs, hwloc_obj_t *objs,
                                              hwloc_uint64_t *values,
                                              unsigned long flags);


enum hwloc_distances_add_flag_e {
  
  HWLOC_DISTANCES_ADD_FLAG_GROUP = (1UL<<0),
  
  HWLOC_DISTANCES_ADD_FLAG_GROUP_INACCURATE = (1UL<<1)
};


HWLOC_DECLSPEC int hwloc_distances_add_commit(hwloc_topology_t topology,
                                              hwloc_distances_add_handle_t handle,
                                              unsigned long flags);








HWLOC_DECLSPEC int hwloc_distances_remove(hwloc_topology_t topology);


HWLOC_DECLSPEC int hwloc_distances_remove_by_depth(hwloc_topology_t topology, int depth);


static __hwloc_inline int
hwloc_distances_remove_by_type(hwloc_topology_t topology, hwloc_obj_type_t type)
{
  int depth = hwloc_get_type_depth(topology, type);
  if (depth == HWLOC_TYPE_DEPTH_UNKNOWN || depth == HWLOC_TYPE_DEPTH_MULTIPLE)
    return 0;
  return hwloc_distances_remove_by_depth(topology, depth);
}


HWLOC_DECLSPEC int hwloc_distances_release_remove(hwloc_topology_t topology, struct hwloc_distances_s *distances);




#ifdef __cplusplus
} 
#endif


#endif 

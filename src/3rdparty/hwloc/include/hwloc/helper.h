



#ifndef HWLOC_HELPER_H
#define HWLOC_HELPER_H

#ifndef HWLOC_H
#error Please include the main hwloc.h instead
#endif

#include <stdlib.h>
#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif





HWLOC_DECLSPEC int
hwloc_obj_type_is_normal(hwloc_obj_type_t type);


HWLOC_DECLSPEC int
hwloc_obj_type_is_io(hwloc_obj_type_t type);


HWLOC_DECLSPEC int
hwloc_obj_type_is_memory(hwloc_obj_type_t type);


HWLOC_DECLSPEC int
hwloc_obj_type_is_cache(hwloc_obj_type_t type);


HWLOC_DECLSPEC int
hwloc_obj_type_is_dcache(hwloc_obj_type_t type);


HWLOC_DECLSPEC int
hwloc_obj_type_is_icache(hwloc_obj_type_t type);








static __hwloc_inline hwloc_obj_t
hwloc_get_first_largest_obj_inside_cpuset(hwloc_topology_t topology, hwloc_const_cpuset_t set)
{
  hwloc_obj_t obj = hwloc_get_root_obj(topology);
  if (!hwloc_bitmap_intersects(obj->cpuset, set))
    return NULL;
  while (!hwloc_bitmap_isincluded(obj->cpuset, set)) {
    
    hwloc_obj_t child = obj->first_child;
    while (child) {
      if (hwloc_bitmap_intersects(child->cpuset, set))
	break;
      child = child->next_sibling;
    }
    if (!child)
      
      return obj;
    
    obj = child;
  }
  
  return obj;
}


HWLOC_DECLSPEC int hwloc_get_largest_objs_inside_cpuset (hwloc_topology_t topology, hwloc_const_cpuset_t set,
						 hwloc_obj_t * __hwloc_restrict objs, int max);


static __hwloc_inline hwloc_obj_t
hwloc_get_next_obj_inside_cpuset_by_depth (hwloc_topology_t topology, hwloc_const_cpuset_t set,
					   int depth, hwloc_obj_t prev)
{
  hwloc_obj_t next = hwloc_get_next_obj_by_depth(topology, depth, prev);
  if (!next)
    return NULL;
  while (next && (hwloc_bitmap_iszero(next->cpuset) || !hwloc_bitmap_isincluded(next->cpuset, set)))
    next = next->next_cousin;
  return next;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_next_obj_inside_cpuset_by_type (hwloc_topology_t topology, hwloc_const_cpuset_t set,
					  hwloc_obj_type_t type, hwloc_obj_t prev)
{
  int depth = hwloc_get_type_depth(topology, type);
  if (depth == HWLOC_TYPE_DEPTH_UNKNOWN || depth == HWLOC_TYPE_DEPTH_MULTIPLE)
    return NULL;
  return hwloc_get_next_obj_inside_cpuset_by_depth(topology, set, depth, prev);
}


static __hwloc_inline hwloc_obj_t
hwloc_get_obj_inside_cpuset_by_depth (hwloc_topology_t topology, hwloc_const_cpuset_t set,
				      int depth, unsigned idx) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_obj_inside_cpuset_by_depth (hwloc_topology_t topology, hwloc_const_cpuset_t set,
				      int depth, unsigned idx)
{
  hwloc_obj_t obj = hwloc_get_obj_by_depth (topology, depth, 0);
  unsigned count = 0;
  if (!obj)
    return NULL;
  while (obj) {
    if (!hwloc_bitmap_iszero(obj->cpuset) && hwloc_bitmap_isincluded(obj->cpuset, set)) {
      if (count == idx)
	return obj;
      count++;
    }
    obj = obj->next_cousin;
  }
  return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_obj_inside_cpuset_by_type (hwloc_topology_t topology, hwloc_const_cpuset_t set,
				     hwloc_obj_type_t type, unsigned idx) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_obj_inside_cpuset_by_type (hwloc_topology_t topology, hwloc_const_cpuset_t set,
				     hwloc_obj_type_t type, unsigned idx)
{
  int depth = hwloc_get_type_depth(topology, type);
  if (depth == HWLOC_TYPE_DEPTH_UNKNOWN || depth == HWLOC_TYPE_DEPTH_MULTIPLE)
    return NULL;
  return hwloc_get_obj_inside_cpuset_by_depth(topology, set, depth, idx);
}


static __hwloc_inline unsigned
hwloc_get_nbobjs_inside_cpuset_by_depth (hwloc_topology_t topology, hwloc_const_cpuset_t set,
					 int depth) __hwloc_attribute_pure;
static __hwloc_inline unsigned
hwloc_get_nbobjs_inside_cpuset_by_depth (hwloc_topology_t topology, hwloc_const_cpuset_t set,
					 int depth)
{
  hwloc_obj_t obj = hwloc_get_obj_by_depth (topology, depth, 0);
  unsigned count = 0;
  if (!obj)
    return 0;
  while (obj) {
    if (!hwloc_bitmap_iszero(obj->cpuset) && hwloc_bitmap_isincluded(obj->cpuset, set))
      count++;
    obj = obj->next_cousin;
  }
  return count;
}


static __hwloc_inline int
hwloc_get_nbobjs_inside_cpuset_by_type (hwloc_topology_t topology, hwloc_const_cpuset_t set,
					hwloc_obj_type_t type) __hwloc_attribute_pure;
static __hwloc_inline int
hwloc_get_nbobjs_inside_cpuset_by_type (hwloc_topology_t topology, hwloc_const_cpuset_t set,
					hwloc_obj_type_t type)
{
  int depth = hwloc_get_type_depth(topology, type);
  if (depth == HWLOC_TYPE_DEPTH_UNKNOWN)
    return 0;
  if (depth == HWLOC_TYPE_DEPTH_MULTIPLE)
    return -1; 
  return (int) hwloc_get_nbobjs_inside_cpuset_by_depth(topology, set, depth);
}


static __hwloc_inline int
hwloc_get_obj_index_inside_cpuset (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_const_cpuset_t set,
				   hwloc_obj_t obj) __hwloc_attribute_pure;
static __hwloc_inline int
hwloc_get_obj_index_inside_cpuset (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_const_cpuset_t set,
				   hwloc_obj_t obj)
{
  int idx = 0;
  if (!hwloc_bitmap_isincluded(obj->cpuset, set))
    return -1;
  
  while ((obj = obj->prev_cousin) != NULL)
    if (!hwloc_bitmap_iszero(obj->cpuset) && hwloc_bitmap_isincluded(obj->cpuset, set))
      idx++;
  return idx;
}








static __hwloc_inline hwloc_obj_t
hwloc_get_child_covering_cpuset (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_const_cpuset_t set,
				hwloc_obj_t parent) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_child_covering_cpuset (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_const_cpuset_t set,
				hwloc_obj_t parent)
{
  hwloc_obj_t child;
  if (hwloc_bitmap_iszero(set))
    return NULL;
  child = parent->first_child;
  while (child) {
    if (child->cpuset && hwloc_bitmap_isincluded(set, child->cpuset))
      return child;
    child = child->next_sibling;
  }
  return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_obj_covering_cpuset (hwloc_topology_t topology, hwloc_const_cpuset_t set) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_obj_covering_cpuset (hwloc_topology_t topology, hwloc_const_cpuset_t set)
{
  struct hwloc_obj *current = hwloc_get_root_obj(topology);
  if (hwloc_bitmap_iszero(set) || !hwloc_bitmap_isincluded(set, current->cpuset))
    return NULL;
  while (1) {
    hwloc_obj_t child = hwloc_get_child_covering_cpuset(topology, set, current);
    if (!child)
      return current;
    current = child;
  }
}


static __hwloc_inline hwloc_obj_t
hwloc_get_next_obj_covering_cpuset_by_depth(hwloc_topology_t topology, hwloc_const_cpuset_t set,
					    int depth, hwloc_obj_t prev)
{
  hwloc_obj_t next = hwloc_get_next_obj_by_depth(topology, depth, prev);
  if (!next)
    return NULL;
  while (next && !hwloc_bitmap_intersects(set, next->cpuset))
    next = next->next_cousin;
  return next;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_next_obj_covering_cpuset_by_type(hwloc_topology_t topology, hwloc_const_cpuset_t set,
					   hwloc_obj_type_t type, hwloc_obj_t prev)
{
  int depth = hwloc_get_type_depth(topology, type);
  if (depth == HWLOC_TYPE_DEPTH_UNKNOWN || depth == HWLOC_TYPE_DEPTH_MULTIPLE)
    return NULL;
  return hwloc_get_next_obj_covering_cpuset_by_depth(topology, set, depth, prev);
}








static __hwloc_inline hwloc_obj_t
hwloc_get_ancestor_obj_by_depth (hwloc_topology_t topology __hwloc_attribute_unused, int depth, hwloc_obj_t obj) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_ancestor_obj_by_depth (hwloc_topology_t topology __hwloc_attribute_unused, int depth, hwloc_obj_t obj)
{
  hwloc_obj_t ancestor = obj;
  if (obj->depth < depth)
    return NULL;
  while (ancestor && ancestor->depth > depth)
    ancestor = ancestor->parent;
  return ancestor;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_ancestor_obj_by_type (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_type_t type, hwloc_obj_t obj) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_ancestor_obj_by_type (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_type_t type, hwloc_obj_t obj)
{
  hwloc_obj_t ancestor = obj->parent;
  while (ancestor && ancestor->type != type)
    ancestor = ancestor->parent;
  return ancestor;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_common_ancestor_obj (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_t obj1, hwloc_obj_t obj2) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_common_ancestor_obj (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_t obj1, hwloc_obj_t obj2)
{
  
  while (obj1 != obj2) {
    while (obj1->depth > obj2->depth)
      obj1 = obj1->parent;
    while (obj2->depth > obj1->depth)
      obj2 = obj2->parent;
    if (obj1 != obj2 && obj1->depth == obj2->depth) {
      obj1 = obj1->parent;
      obj2 = obj2->parent;
    }
  }
  return obj1;
}


static __hwloc_inline int
hwloc_obj_is_in_subtree (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_t obj, hwloc_obj_t subtree_root) __hwloc_attribute_pure;
static __hwloc_inline int
hwloc_obj_is_in_subtree (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_t obj, hwloc_obj_t subtree_root)
{
  return obj->cpuset && subtree_root->cpuset && hwloc_bitmap_isincluded(obj->cpuset, subtree_root->cpuset);
}


static __hwloc_inline hwloc_obj_t
hwloc_get_next_child (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_t parent, hwloc_obj_t prev)
{
  hwloc_obj_t obj;
  int state = 0;
  if (prev) {
    if (prev->type == HWLOC_OBJ_MISC)
      state = 3;
    else if (hwloc_obj_type_is_io(prev->type))
      state = 2;
    else if (hwloc_obj_type_is_memory(prev->type))
      state = 1;
    obj = prev->next_sibling;
  } else {
    obj = parent->first_child;
  }
  if (!obj && state == 0) {
    obj = parent->memory_first_child;
    state = 1;
  }
  if (!obj && state == 1) {
    obj = parent->io_first_child;
    state = 2;
  }
  if (!obj && state == 2) {
    obj = parent->misc_first_child;
    state = 3;
  }
  return obj;
}








static __hwloc_inline int
hwloc_get_cache_type_depth (hwloc_topology_t topology,
			    unsigned cachelevel, hwloc_obj_cache_type_t cachetype)
{
  int depth;
  int found = HWLOC_TYPE_DEPTH_UNKNOWN;
  for (depth=0; ; depth++) {
    hwloc_obj_t obj = hwloc_get_obj_by_depth(topology, depth, 0);
    if (!obj)
      break;
    if (!hwloc_obj_type_is_dcache(obj->type) || obj->attr->cache.depth != cachelevel)
      
      continue;
    if (cachetype == (hwloc_obj_cache_type_t) -1) {
      if (found != HWLOC_TYPE_DEPTH_UNKNOWN) {
	
        return HWLOC_TYPE_DEPTH_MULTIPLE;
      }
      
      found = depth;
      continue;
    }
    if (obj->attr->cache.type == cachetype || obj->attr->cache.type == HWLOC_OBJ_CACHE_UNIFIED)
      
      return depth;
  }
  
  return found;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_cache_covering_cpuset (hwloc_topology_t topology, hwloc_const_cpuset_t set) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_cache_covering_cpuset (hwloc_topology_t topology, hwloc_const_cpuset_t set)
{
  hwloc_obj_t current = hwloc_get_obj_covering_cpuset(topology, set);
  while (current) {
    if (hwloc_obj_type_is_dcache(current->type))
      return current;
    current = current->parent;
  }
  return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_shared_cache_covering_obj (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_t obj) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_shared_cache_covering_obj (hwloc_topology_t topology __hwloc_attribute_unused, hwloc_obj_t obj)
{
  hwloc_obj_t current = obj->parent;
  if (!obj->cpuset)
    return NULL;
  while (current) {
    if (!hwloc_bitmap_isequal(current->cpuset, obj->cpuset)
        && hwloc_obj_type_is_dcache(current->type))
      return current;
    current = current->parent;
  }
  return NULL;
}








HWLOC_DECLSPEC int hwloc_bitmap_singlify_per_core(hwloc_topology_t topology, hwloc_bitmap_t cpuset, unsigned which);


static __hwloc_inline hwloc_obj_t
hwloc_get_pu_obj_by_os_index(hwloc_topology_t topology, unsigned os_index) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_pu_obj_by_os_index(hwloc_topology_t topology, unsigned os_index)
{
  hwloc_obj_t obj = NULL;
  while ((obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_PU, obj)) != NULL)
    if (obj->os_index == os_index)
      return obj;
  return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_numanode_obj_by_os_index(hwloc_topology_t topology, unsigned os_index) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_numanode_obj_by_os_index(hwloc_topology_t topology, unsigned os_index)
{
  hwloc_obj_t obj = NULL;
  while ((obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_NUMANODE, obj)) != NULL)
    if (obj->os_index == os_index)
      return obj;
  return NULL;
}



HWLOC_DECLSPEC unsigned hwloc_get_closest_objs (hwloc_topology_t topology, hwloc_obj_t src, hwloc_obj_t * __hwloc_restrict objs, unsigned max);


static __hwloc_inline hwloc_obj_t
hwloc_get_obj_below_by_type (hwloc_topology_t topology,
			     hwloc_obj_type_t type1, unsigned idx1,
			     hwloc_obj_type_t type2, unsigned idx2) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_obj_below_by_type (hwloc_topology_t topology,
			     hwloc_obj_type_t type1, unsigned idx1,
			     hwloc_obj_type_t type2, unsigned idx2)
{
  hwloc_obj_t obj;
  obj = hwloc_get_obj_by_type (topology, type1, idx1);
  if (!obj)
    return NULL;
  return hwloc_get_obj_inside_cpuset_by_type(topology, obj->cpuset, type2, idx2);
}


static __hwloc_inline hwloc_obj_t
hwloc_get_obj_below_array_by_type (hwloc_topology_t topology, int nr, hwloc_obj_type_t *typev, unsigned *idxv) __hwloc_attribute_pure;
static __hwloc_inline hwloc_obj_t
hwloc_get_obj_below_array_by_type (hwloc_topology_t topology, int nr, hwloc_obj_type_t *typev, unsigned *idxv)
{
  hwloc_obj_t obj = hwloc_get_root_obj(topology);
  int i;
  for(i=0; i<nr; i++) {
    if (!obj)
      return NULL;
    obj = hwloc_get_obj_inside_cpuset_by_type(topology, obj->cpuset, typev[i], idxv[i]);
  }
  return obj;
}


HWLOC_DECLSPEC hwloc_obj_t
hwloc_get_obj_with_same_locality(hwloc_topology_t topology, hwloc_obj_t src,
                                 hwloc_obj_type_t type, const char *subtype, const char *nameprefix,
                                 unsigned long flags);








enum hwloc_distrib_flags_e {
  
  HWLOC_DISTRIB_FLAG_REVERSE = (1UL<<0)
};


static __hwloc_inline int
hwloc_distrib(hwloc_topology_t topology,
	      hwloc_obj_t *roots, unsigned n_roots,
	      hwloc_cpuset_t *set,
	      unsigned n,
	      int until, unsigned long flags)
{
  unsigned i;
  unsigned tot_weight;
  unsigned given, givenweight;
  hwloc_cpuset_t *cpusetp = set;

  if (!n || (flags & ~HWLOC_DISTRIB_FLAG_REVERSE)) {
    errno = EINVAL;
    return -1;
  }

  tot_weight = 0;
  for (i = 0; i < n_roots; i++)
    tot_weight += (unsigned) hwloc_bitmap_weight(roots[i]->cpuset);

  for (i = 0, given = 0, givenweight = 0; i < n_roots; i++) {
    unsigned chunk, weight;
    hwloc_obj_t root = roots[flags & HWLOC_DISTRIB_FLAG_REVERSE ? n_roots-1-i : i];
    hwloc_cpuset_t cpuset = root->cpuset;
    while (!hwloc_obj_type_is_normal(root->type))
      
      root = root->parent;
    weight = (unsigned) hwloc_bitmap_weight(cpuset);
    if (!weight)
      continue;
    
    chunk = (( (givenweight+weight) * n  + tot_weight-1) / tot_weight)
          - ((  givenweight         * n  + tot_weight-1) / tot_weight);
    if (!root->arity || chunk <= 1 || root->depth >= until) {
      
      if (chunk) {
	
	unsigned j;
	for (j=0; j < chunk; j++)
	  cpusetp[j] = hwloc_bitmap_dup(cpuset);
      } else {
	
	assert(given);
	hwloc_bitmap_or(cpusetp[-1], cpusetp[-1], cpuset);
      }
    } else {
      
      hwloc_distrib(topology, root->children, root->arity, cpusetp, chunk, until, flags);
    }
    cpusetp += chunk;
    given += chunk;
    givenweight += weight;
  }

  return 0;
}








HWLOC_DECLSPEC hwloc_const_cpuset_t
hwloc_topology_get_complete_cpuset(hwloc_topology_t topology) __hwloc_attribute_pure;


HWLOC_DECLSPEC hwloc_const_cpuset_t
hwloc_topology_get_topology_cpuset(hwloc_topology_t topology) __hwloc_attribute_pure;


HWLOC_DECLSPEC hwloc_const_cpuset_t
hwloc_topology_get_allowed_cpuset(hwloc_topology_t topology) __hwloc_attribute_pure;


HWLOC_DECLSPEC hwloc_const_nodeset_t
hwloc_topology_get_complete_nodeset(hwloc_topology_t topology) __hwloc_attribute_pure;


HWLOC_DECLSPEC hwloc_const_nodeset_t
hwloc_topology_get_topology_nodeset(hwloc_topology_t topology) __hwloc_attribute_pure;


HWLOC_DECLSPEC hwloc_const_nodeset_t
hwloc_topology_get_allowed_nodeset(hwloc_topology_t topology) __hwloc_attribute_pure;








static __hwloc_inline int
hwloc_cpuset_to_nodeset(hwloc_topology_t topology, hwloc_const_cpuset_t _cpuset, hwloc_nodeset_t nodeset)
{
	int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
	hwloc_obj_t obj = NULL;
	assert(depth != HWLOC_TYPE_DEPTH_UNKNOWN);
	hwloc_bitmap_zero(nodeset);
	while ((obj = hwloc_get_next_obj_covering_cpuset_by_depth(topology, _cpuset, depth, obj)) != NULL)
		if (hwloc_bitmap_set(nodeset, obj->os_index) < 0)
			return -1;
	return 0;
}


static __hwloc_inline int
hwloc_cpuset_from_nodeset(hwloc_topology_t topology, hwloc_cpuset_t _cpuset, hwloc_const_nodeset_t nodeset)
{
	int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
	hwloc_obj_t obj = NULL;
	assert(depth != HWLOC_TYPE_DEPTH_UNKNOWN);
	hwloc_bitmap_zero(_cpuset);
	while ((obj = hwloc_get_next_obj_by_depth(topology, depth, obj)) != NULL) {
		if (hwloc_bitmap_isset(nodeset, obj->os_index))
			
			if (hwloc_bitmap_or(_cpuset, _cpuset, obj->cpuset) < 0)
				return -1;
	}
	return 0;
}








static __hwloc_inline hwloc_obj_t
hwloc_get_non_io_ancestor_obj(hwloc_topology_t topology __hwloc_attribute_unused,
			      hwloc_obj_t ioobj)
{
  hwloc_obj_t obj = ioobj;
  while (obj && !obj->cpuset) {
    obj = obj->parent;
  }
  return obj;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_next_pcidev(hwloc_topology_t topology, hwloc_obj_t prev)
{
  return hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_PCI_DEVICE, prev);
}


static __hwloc_inline hwloc_obj_t
hwloc_get_pcidev_by_busid(hwloc_topology_t topology,
			  unsigned domain, unsigned bus, unsigned dev, unsigned func)
{
  hwloc_obj_t obj = NULL;
  while ((obj = hwloc_get_next_pcidev(topology, obj)) != NULL) {
    if (obj->attr->pcidev.domain == domain
	&& obj->attr->pcidev.bus == bus
	&& obj->attr->pcidev.dev == dev
	&& obj->attr->pcidev.func == func)
      return obj;
  }
  return NULL;
}


static __hwloc_inline hwloc_obj_t
hwloc_get_pcidev_by_busidstring(hwloc_topology_t topology, const char *busid)
{
  unsigned domain = 0; 
  unsigned bus, dev, func;

  if (sscanf(busid, "%x:%x.%x", &bus, &dev, &func) != 3
      && sscanf(busid, "%x:%x:%x.%x", &domain, &bus, &dev, &func) != 4) {
    errno = EINVAL;
    return NULL;
  }

  return hwloc_get_pcidev_by_busid(topology, domain, bus, dev, func);
}


static __hwloc_inline hwloc_obj_t
hwloc_get_next_osdev(hwloc_topology_t topology, hwloc_obj_t prev)
{
  return hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_OS_DEVICE, prev);
}


static __hwloc_inline hwloc_obj_t
hwloc_get_next_bridge(hwloc_topology_t topology, hwloc_obj_t prev)
{
  return hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_BRIDGE, prev);
}


static __hwloc_inline int
hwloc_bridge_covers_pcibus(hwloc_obj_t bridge,
			   unsigned domain, unsigned bus)
{
  return bridge->type == HWLOC_OBJ_BRIDGE
    && bridge->attr->bridge.downstream_type == HWLOC_OBJ_BRIDGE_PCI
    && bridge->attr->bridge.downstream.pci.domain == domain
    && bridge->attr->bridge.downstream.pci.secondary_bus <= bus
    && bridge->attr->bridge.downstream.pci.subordinate_bus >= bus;
}





#ifdef __cplusplus
} 
#endif


#endif 





#ifndef HWLOC_LINUX_LIBNUMA_H
#define HWLOC_LINUX_LIBNUMA_H

#include "hwloc.h"

#include <numa.h>


#ifdef __cplusplus
extern "C" {
#endif






static __hwloc_inline int
hwloc_cpuset_to_linux_libnuma_ulongs(hwloc_topology_t topology, hwloc_const_cpuset_t cpuset,
				    unsigned long *mask, unsigned long *maxnode)
{
  int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
  unsigned long outmaxnode = -1;
  hwloc_obj_t node = NULL;

  
  *maxnode = (*maxnode + 8*sizeof(*mask) - 1) & ~(8*sizeof(*mask) - 1);
  memset(mask, 0, *maxnode/8);

  while ((node = hwloc_get_next_obj_covering_cpuset_by_depth(topology, cpuset, depth, node)) != NULL) {
    if (node->os_index >= *maxnode)
      continue;
    mask[node->os_index/sizeof(*mask)/8] |= 1UL << (node->os_index % (sizeof(*mask)*8));
    if (outmaxnode == (unsigned long) -1 || outmaxnode < node->os_index)
      outmaxnode = node->os_index;
  }

  *maxnode = outmaxnode+1;
  return 0;
}


static __hwloc_inline int
hwloc_nodeset_to_linux_libnuma_ulongs(hwloc_topology_t topology, hwloc_const_nodeset_t nodeset,
				      unsigned long *mask, unsigned long *maxnode)
{
  int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
  unsigned long outmaxnode = -1;
  hwloc_obj_t node = NULL;

  
  *maxnode = (*maxnode + 8*sizeof(*mask) - 1) & ~(8*sizeof(*mask) - 1);
  memset(mask, 0, *maxnode/8);

  while ((node = hwloc_get_next_obj_by_depth(topology, depth, node)) != NULL) {
    if (node->os_index >= *maxnode)
      continue;
    if (!hwloc_bitmap_isset(nodeset, node->os_index))
      continue;
    mask[node->os_index/sizeof(*mask)/8] |= 1UL << (node->os_index % (sizeof(*mask)*8));
    if (outmaxnode == (unsigned long) -1 || outmaxnode < node->os_index)
      outmaxnode = node->os_index;
  }

  *maxnode = outmaxnode+1;
  return 0;
}


static __hwloc_inline int
hwloc_cpuset_from_linux_libnuma_ulongs(hwloc_topology_t topology, hwloc_cpuset_t cpuset,
				      const unsigned long *mask, unsigned long maxnode)
{
  int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
  hwloc_obj_t node = NULL;
  hwloc_bitmap_zero(cpuset);
  while ((node = hwloc_get_next_obj_by_depth(topology, depth, node)) != NULL)
    if (node->os_index < maxnode
	&& (mask[node->os_index/sizeof(*mask)/8] & (1UL << (node->os_index % (sizeof(*mask)*8)))))
      if (hwloc_bitmap_or(cpuset, cpuset, node->cpuset) < 0)
        return -1;
  return 0;
}


static __hwloc_inline int
hwloc_nodeset_from_linux_libnuma_ulongs(hwloc_topology_t topology, hwloc_nodeset_t nodeset,
					const unsigned long *mask, unsigned long maxnode)
{
  int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
  hwloc_obj_t node = NULL;
  hwloc_bitmap_zero(nodeset);
  while ((node = hwloc_get_next_obj_by_depth(topology, depth, node)) != NULL)
    if (node->os_index < maxnode
	&& (mask[node->os_index/sizeof(*mask)/8] & (1UL << (node->os_index % (sizeof(*mask)*8)))))
      if (hwloc_bitmap_set(nodeset, node->os_index) < 0)
        return -1;
  return 0;
}









static __hwloc_inline struct bitmask *
hwloc_cpuset_to_linux_libnuma_bitmask(hwloc_topology_t topology, hwloc_const_cpuset_t cpuset) __hwloc_attribute_malloc;
static __hwloc_inline struct bitmask *
hwloc_cpuset_to_linux_libnuma_bitmask(hwloc_topology_t topology, hwloc_const_cpuset_t cpuset)
{
  int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
  hwloc_obj_t node = NULL;
  struct bitmask *bitmask = numa_allocate_cpumask();
  if (!bitmask)
    return NULL;
  while ((node = hwloc_get_next_obj_covering_cpuset_by_depth(topology, cpuset, depth, node)) != NULL)
    if (node->attr->numanode.local_memory)
      numa_bitmask_setbit(bitmask, node->os_index);
  return bitmask;
}


static __hwloc_inline struct bitmask *
hwloc_nodeset_to_linux_libnuma_bitmask(hwloc_topology_t topology, hwloc_const_nodeset_t nodeset) __hwloc_attribute_malloc;
static __hwloc_inline struct bitmask *
hwloc_nodeset_to_linux_libnuma_bitmask(hwloc_topology_t topology, hwloc_const_nodeset_t nodeset)
{
  int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
  hwloc_obj_t node = NULL;
  struct bitmask *bitmask = numa_allocate_cpumask();
  if (!bitmask)
    return NULL;
  while ((node = hwloc_get_next_obj_by_depth(topology, depth, node)) != NULL)
    if (hwloc_bitmap_isset(nodeset, node->os_index) && node->attr->numanode.local_memory)
      numa_bitmask_setbit(bitmask, node->os_index);
  return bitmask;
}


static __hwloc_inline int
hwloc_cpuset_from_linux_libnuma_bitmask(hwloc_topology_t topology, hwloc_cpuset_t cpuset,
					const struct bitmask *bitmask)
{
  int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
  hwloc_obj_t node = NULL;
  hwloc_bitmap_zero(cpuset);
  while ((node = hwloc_get_next_obj_by_depth(topology, depth, node)) != NULL)
    if (numa_bitmask_isbitset(bitmask, node->os_index))
      if (hwloc_bitmap_or(cpuset, cpuset, node->cpuset) < 0)
        return -1;
  return 0;
}


static __hwloc_inline int
hwloc_nodeset_from_linux_libnuma_bitmask(hwloc_topology_t topology, hwloc_nodeset_t nodeset,
					 const struct bitmask *bitmask)
{
  int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_NUMANODE);
  hwloc_obj_t node = NULL;
  hwloc_bitmap_zero(nodeset);
  while ((node = hwloc_get_next_obj_by_depth(topology, depth, node)) != NULL)
    if (numa_bitmask_isbitset(bitmask, node->os_index))
      if (hwloc_bitmap_set(nodeset, node->os_index) < 0)
        return -1;
  return 0;
}




#ifdef __cplusplus
} 
#endif


#endif 

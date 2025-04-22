




#ifdef HWLOC_INSIDE_PLUGIN

#error This file should not be used in plugins
#endif


#ifndef HWLOC_PRIVATE_H
#define HWLOC_PRIVATE_H

#include "private/autogen/config.h"
#include "hwloc.h"
#include "hwloc/bitmap.h"
#include "private/components.h"
#include "private/misc.h"

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#include <string.h>

#define HWLOC_TOPOLOGY_ABI 0x20400 

struct hwloc_internal_location_s {
  enum hwloc_location_type_e type;
  union {
    struct {
      hwloc_obj_t obj; 
      uint64_t gp_index;
      hwloc_obj_type_t type;
    } object; 
    hwloc_cpuset_t cpuset; 
  } location;
};



struct hwloc_topology {
  unsigned topology_abi;

  unsigned nb_levels;					
  unsigned nb_levels_allocated;				
  unsigned *level_nbobjects; 				
  struct hwloc_obj ***levels;				
  unsigned long flags;
  int type_depth[HWLOC_OBJ_TYPE_MAX];
  enum hwloc_type_filter_e type_filter[HWLOC_OBJ_TYPE_MAX];
  int is_thissystem;
  int is_loaded;
  int modified;                                         
  hwloc_pid_t pid;                                      
  void *userdata;
  uint64_t next_gp_index;

  void *adopted_shmem_addr;
  size_t adopted_shmem_length;

#define HWLOC_NR_SLEVELS 6
#define HWLOC_SLEVEL_NUMANODE 0
#define HWLOC_SLEVEL_BRIDGE 1
#define HWLOC_SLEVEL_PCIDEV 2
#define HWLOC_SLEVEL_OSDEV 3
#define HWLOC_SLEVEL_MISC 4
#define HWLOC_SLEVEL_MEMCACHE 5
  
#define HWLOC_SLEVEL_FROM_DEPTH(x) (HWLOC_TYPE_DEPTH_NUMANODE-(x))
#define HWLOC_SLEVEL_TO_DEPTH(x) (HWLOC_TYPE_DEPTH_NUMANODE-(x))
  struct hwloc_special_level_s {
    unsigned nbobjs;
    struct hwloc_obj **objs;
    struct hwloc_obj *first, *last; 
  } slevels[HWLOC_NR_SLEVELS];

  hwloc_bitmap_t allowed_cpuset;
  hwloc_bitmap_t allowed_nodeset;

  struct hwloc_binding_hooks {
    
    int (*set_thisproc_cpubind)(hwloc_topology_t topology, hwloc_const_cpuset_t set, int flags);
    int (*get_thisproc_cpubind)(hwloc_topology_t topology, hwloc_cpuset_t set, int flags);
    int (*set_thisthread_cpubind)(hwloc_topology_t topology, hwloc_const_cpuset_t set, int flags);
    int (*get_thisthread_cpubind)(hwloc_topology_t topology, hwloc_cpuset_t set, int flags);
    int (*set_proc_cpubind)(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_const_cpuset_t set, int flags);
    int (*get_proc_cpubind)(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_cpuset_t set, int flags);
#ifdef hwloc_thread_t
    int (*set_thread_cpubind)(hwloc_topology_t topology, hwloc_thread_t tid, hwloc_const_cpuset_t set, int flags);
    int (*get_thread_cpubind)(hwloc_topology_t topology, hwloc_thread_t tid, hwloc_cpuset_t set, int flags);
#endif

    int (*get_thisproc_last_cpu_location)(hwloc_topology_t topology, hwloc_cpuset_t set, int flags);
    int (*get_thisthread_last_cpu_location)(hwloc_topology_t topology, hwloc_cpuset_t set, int flags);
    int (*get_proc_last_cpu_location)(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_cpuset_t set, int flags);

    int (*set_thisproc_membind)(hwloc_topology_t topology, hwloc_const_nodeset_t nodeset, hwloc_membind_policy_t policy, int flags);
    int (*get_thisproc_membind)(hwloc_topology_t topology, hwloc_nodeset_t nodeset, hwloc_membind_policy_t * policy, int flags);
    int (*set_thisthread_membind)(hwloc_topology_t topology, hwloc_const_nodeset_t nodeset, hwloc_membind_policy_t policy, int flags);
    int (*get_thisthread_membind)(hwloc_topology_t topology, hwloc_nodeset_t nodeset, hwloc_membind_policy_t * policy, int flags);
    int (*set_proc_membind)(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_const_nodeset_t nodeset, hwloc_membind_policy_t policy, int flags);
    int (*get_proc_membind)(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_nodeset_t nodeset, hwloc_membind_policy_t * policy, int flags);
    int (*set_area_membind)(hwloc_topology_t topology, const void *addr, size_t len, hwloc_const_nodeset_t nodeset, hwloc_membind_policy_t policy, int flags);
    int (*get_area_membind)(hwloc_topology_t topology, const void *addr, size_t len, hwloc_nodeset_t nodeset, hwloc_membind_policy_t * policy, int flags);
    int (*get_area_memlocation)(hwloc_topology_t topology, const void *addr, size_t len, hwloc_nodeset_t nodeset, int flags);
    
    void *(*alloc)(hwloc_topology_t topology, size_t len);
    
    void *(*alloc_membind)(hwloc_topology_t topology, size_t len, hwloc_const_nodeset_t nodeset, hwloc_membind_policy_t policy, int flags);
    int (*free_membind)(hwloc_topology_t topology, void *addr, size_t len);

    int (*get_allowed_resources)(hwloc_topology_t topology);
  } binding_hooks;

  struct hwloc_topology_support support;

  void (*userdata_export_cb)(void *reserved, struct hwloc_topology *topology, struct hwloc_obj *obj);
  void (*userdata_import_cb)(struct hwloc_topology *topology, struct hwloc_obj *obj, const char *name, const void *buffer, size_t length);
  int userdata_not_decoded;

  struct hwloc_internal_distances_s {
    char *name; 

    unsigned id; 

    
    hwloc_obj_type_t unique_type;
    hwloc_obj_type_t *different_types;

    
    unsigned nbobjs;
    uint64_t *indexes; 
#define HWLOC_DIST_TYPE_USE_OS_INDEX(_type) ((_type) == HWLOC_OBJ_PU || (_type == HWLOC_OBJ_NUMANODE))
    uint64_t *values; 
    unsigned long kind;

#define HWLOC_INTERNAL_DIST_FLAG_OBJS_VALID (1U<<0) 
#define HWLOC_INTERNAL_DIST_FLAG_NOT_COMMITTED (1U<<1) 
    unsigned iflags;

    
    hwloc_obj_t *objs; 

    struct hwloc_internal_distances_s *prev, *next;
  } *first_dist, *last_dist;
  unsigned next_dist_id;

  
  unsigned nr_memattrs;
  struct hwloc_internal_memattr_s {
    
    char *name; 
    unsigned long flags;
#define HWLOC_IMATTR_FLAG_STATIC_NAME (1U<<0) 
#define HWLOC_IMATTR_FLAG_CACHE_VALID (1U<<1) 
#define HWLOC_IMATTR_FLAG_CONVENIENCE (1U<<2) 
    unsigned iflags;

    
    unsigned nr_targets;
    struct hwloc_internal_memattr_target_s {
      
      hwloc_obj_t obj; 
      hwloc_obj_type_t type;
      unsigned os_index; 
      hwloc_uint64_t gp_index;

      
      hwloc_uint64_t noinitiator_value;
      
      unsigned nr_initiators;
      struct hwloc_internal_memattr_initiator_s {
        struct hwloc_internal_location_s initiator;
        hwloc_uint64_t value;
      } *initiators;
    } *targets;
  } *memattrs;

  
  unsigned nr_cpukinds;
  unsigned nr_cpukinds_allocated;
  struct hwloc_internal_cpukind_s {
    hwloc_cpuset_t cpuset;
#define HWLOC_CPUKIND_EFFICIENCY_UNKNOWN -1
    int efficiency;
    int forced_efficiency; 
    hwloc_uint64_t ranking_value; 
    unsigned nr_infos;
    struct hwloc_info_s *infos;
  } *cpukinds;

  int grouping;
  int grouping_verbose;
  unsigned grouping_nbaccuracies;
  float grouping_accuracies[5];
  unsigned grouping_next_subkind;

  
  struct hwloc_backend * backends;
  struct hwloc_backend * get_pci_busid_cpuset_backend; 
  unsigned backend_phases;
  unsigned backend_excluded_phases;

  
  struct hwloc_tma * tma;



  

  
  int want_some_cpu_caches;

  
  struct hwloc_numanode_attr_s machine_memory;

  
  int pci_has_forced_locality;
  unsigned pci_forced_locality_nr;
  struct hwloc_pci_forced_locality_s {
    unsigned domain;
    unsigned bus_first, bus_last;
    hwloc_bitmap_t cpuset;
  } * pci_forced_locality;
  hwloc_uint64_t pci_locality_quirks;

  
  unsigned nr_blacklisted_components;
  struct hwloc_topology_forced_component_s {
    struct hwloc_disc_component *component;
    unsigned phases;
  } *blacklisted_components;

  
  struct hwloc_pci_locality_s {
    unsigned domain;
    unsigned bus_min;
    unsigned bus_max;
    hwloc_bitmap_t cpuset;
    hwloc_obj_t parent;
    struct hwloc_pci_locality_s *prev, *next;
  } *first_pci_locality, *last_pci_locality;
};

extern void hwloc_alloc_root_sets(hwloc_obj_t root);
extern void hwloc_setup_pu_level(struct hwloc_topology *topology, unsigned nb_pus);
extern int hwloc_get_sysctlbyname(const char *name, int64_t *n);
extern int hwloc_get_sysctl(int name[], unsigned namelen, int64_t *n);


#define HWLOC_FALLBACK_NBPROCESSORS_INCLUDE_OFFLINE 1 
extern int hwloc_fallback_nbprocessors(unsigned flags);

extern int64_t hwloc_fallback_memsize(void);

extern int hwloc__object_cpusets_compare_first(hwloc_obj_t obj1, hwloc_obj_t obj2);
extern void hwloc__reorder_children(hwloc_obj_t parent);

extern void hwloc_topology_setup_defaults(struct hwloc_topology *topology);
extern void hwloc_topology_clear(struct hwloc_topology *topology);


extern struct hwloc_obj * hwloc__attach_memory_object(struct hwloc_topology *topology, hwloc_obj_t parent,
                                                      hwloc_obj_t obj, const char *reason);

extern hwloc_obj_t hwloc_get_obj_by_type_and_gp_index(hwloc_topology_t topology, hwloc_obj_type_t type, uint64_t gp_index);

extern void hwloc_pci_discovery_init(struct hwloc_topology *topology);
extern void hwloc_pci_discovery_prepare(struct hwloc_topology *topology);
extern void hwloc_pci_discovery_exit(struct hwloc_topology *topology);


extern hwloc_obj_t hwloc_find_insert_io_parent_by_complete_cpuset(struct hwloc_topology *topology, hwloc_cpuset_t cpuset);

extern int hwloc__add_info(struct hwloc_info_s **infosp, unsigned *countp, const char *name, const char *value);
extern int hwloc__add_info_nodup(struct hwloc_info_s **infosp, unsigned *countp, const char *name, const char *value, int replace);
extern int hwloc__move_infos(struct hwloc_info_s **dst_infosp, unsigned *dst_countp, struct hwloc_info_s **src_infosp, unsigned *src_countp);
extern int hwloc__tma_dup_infos(struct hwloc_tma *tma, struct hwloc_info_s **dst_infosp, unsigned *dst_countp, struct hwloc_info_s *src_infos, unsigned src_count);
extern void hwloc__free_infos(struct hwloc_info_s *infos, unsigned count);


extern void hwloc_set_native_binding_hooks(struct hwloc_binding_hooks *hooks, struct hwloc_topology_support *support);

extern void hwloc_set_binding_hooks(struct hwloc_topology *topology);

#if defined(HWLOC_LINUX_SYS)
extern void hwloc_set_linuxfs_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

#if defined(HWLOC_BGQ_SYS)
extern void hwloc_set_bgq_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

#ifdef HWLOC_SOLARIS_SYS
extern void hwloc_set_solaris_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

#ifdef HWLOC_AIX_SYS
extern void hwloc_set_aix_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

#ifdef HWLOC_WIN_SYS
extern void hwloc_set_windows_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

#ifdef HWLOC_DARWIN_SYS
extern void hwloc_set_darwin_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

#ifdef HWLOC_FREEBSD_SYS
extern void hwloc_set_freebsd_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

#ifdef HWLOC_NETBSD_SYS
extern void hwloc_set_netbsd_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

#ifdef HWLOC_HPUX_SYS
extern void hwloc_set_hpux_hooks(struct hwloc_binding_hooks *binding_hooks, struct hwloc_topology_support *support);
#endif 

extern int hwloc_look_hardwired_fujitsu_k(struct hwloc_topology *topology);
extern int hwloc_look_hardwired_fujitsu_fx10(struct hwloc_topology *topology);
extern int hwloc_look_hardwired_fujitsu_fx100(struct hwloc_topology *topology);


extern void hwloc_add_uname_info(struct hwloc_topology *topology, void *cached_uname);


extern void hwloc_free_unlinked_object(hwloc_obj_t obj);


extern void hwloc_free_object_and_children(hwloc_obj_t obj);


extern void hwloc_free_object_siblings_and_children(hwloc_obj_t obj);


void *hwloc_alloc_heap(hwloc_topology_t topology, size_t len);


void *hwloc_alloc_mmap(hwloc_topology_t topology, size_t len);


int hwloc_free_heap(hwloc_topology_t topology, void *addr, size_t len);


int hwloc_free_mmap(hwloc_topology_t topology, void *addr, size_t len);


static __hwloc_inline void *
hwloc_alloc_or_fail(hwloc_topology_t topology, size_t len, int flags)
{
  if (flags & HWLOC_MEMBIND_STRICT)
    return NULL;
  return hwloc_alloc(topology, len);
}

extern void hwloc_internal_distances_init(hwloc_topology_t topology);
extern void hwloc_internal_distances_prepare(hwloc_topology_t topology);
extern void hwloc_internal_distances_destroy(hwloc_topology_t topology);
extern int hwloc_internal_distances_dup(hwloc_topology_t new, hwloc_topology_t old);
extern void hwloc_internal_distances_refresh(hwloc_topology_t topology);
extern void hwloc_internal_distances_invalidate_cached_objs(hwloc_topology_t topology);


extern int hwloc_internal_distances_add_by_index(hwloc_topology_t topology, const char *name, hwloc_obj_type_t unique_type, hwloc_obj_type_t *different_types, unsigned nbobjs, uint64_t *indexes, uint64_t *values, unsigned long kind, unsigned long flags);
extern int hwloc_internal_distances_add(hwloc_topology_t topology, const char *name, unsigned nbobjs, hwloc_obj_t *objs, uint64_t *values, unsigned long kind, unsigned long flags);

extern void hwloc_internal_memattrs_init(hwloc_topology_t topology);
extern void hwloc_internal_memattrs_prepare(hwloc_topology_t topology);
extern void hwloc_internal_memattrs_destroy(hwloc_topology_t topology);
extern void hwloc_internal_memattrs_need_refresh(hwloc_topology_t topology);
extern void hwloc_internal_memattrs_refresh(hwloc_topology_t topology);
extern int hwloc_internal_memattrs_dup(hwloc_topology_t new, hwloc_topology_t old);
extern int hwloc_internal_memattr_set_value(hwloc_topology_t topology, hwloc_memattr_id_t id, hwloc_obj_type_t target_type, hwloc_uint64_t target_gp_index, unsigned target_os_index, struct hwloc_internal_location_s *initiator, hwloc_uint64_t value);
extern int hwloc_internal_memattrs_guess_memory_tiers(hwloc_topology_t topology, int force_subtype);

extern void hwloc_internal_cpukinds_init(hwloc_topology_t topology);
extern int hwloc_internal_cpukinds_rank(hwloc_topology_t topology);
extern void hwloc_internal_cpukinds_destroy(hwloc_topology_t topology);
extern int hwloc_internal_cpukinds_dup(hwloc_topology_t new, hwloc_topology_t old);
#define HWLOC_CPUKINDS_REGISTER_FLAG_OVERWRITE_FORCED_EFFICIENCY (1<<0)
extern int hwloc_internal_cpukinds_register(hwloc_topology_t topology, hwloc_cpuset_t cpuset, int forced_efficiency, const struct hwloc_info_s *infos, unsigned nr_infos, unsigned long flags);
extern void hwloc_internal_cpukinds_restrict(hwloc_topology_t topology);


extern int hwloc_encode_to_base64(const char *src, size_t srclength, char *target, size_t targsize);

extern int hwloc_decode_from_base64(char const *src, char *target, size_t targsize);


#ifdef HWLOC_HAVE_CORRECT_SNPRINTF
#define hwloc_snprintf snprintf
#else
extern int hwloc_snprintf(char *str, size_t size, const char *format, ...) __hwloc_attribute_format(printf, 3, 4);
#endif


extern char * hwloc_progname(struct hwloc_topology *topology);



#define HWLOC_GROUP_KIND_USER				0	
#define HWLOC_GROUP_KIND_SYNTHETIC			10	

#define HWLOC_GROUP_KIND_INTEL_KNL_SUBNUMA_CLUSTER	100	
#define HWLOC_GROUP_KIND_INTEL_EXTTOPOENUM_UNKNOWN	101	
#define HWLOC_GROUP_KIND_INTEL_MODULE			102	
#define HWLOC_GROUP_KIND_INTEL_TILE			103	
#define HWLOC_GROUP_KIND_INTEL_DIE			104	
#define HWLOC_GROUP_KIND_S390_BOOK			110	
#define HWLOC_GROUP_KIND_AMD_COMPUTE_UNIT		120	
#define HWLOC_GROUP_KIND_AMD_COMPLEX                    121     

#define HWLOC_GROUP_KIND_SOLARIS_PG_HW_PERF		200	
#define HWLOC_GROUP_KIND_AIX_SDL_UNKNOWN		210	
#define HWLOC_GROUP_KIND_WINDOWS_PROCESSOR_GROUP	220	
#define HWLOC_GROUP_KIND_WINDOWS_RELATIONSHIP_UNKNOWN	221	
#define HWLOC_GROUP_KIND_LINUX_CLUSTER                  222     

#define HWLOC_GROUP_KIND_DISTANCE			900	

#define HWLOC_GROUP_KIND_IO				1000	
#define HWLOC_GROUP_KIND_MEMORY				1001	


struct hwloc_tma {
  void * (*malloc)(struct hwloc_tma *, size_t);
  void *data;
  int dontfree; 
};

static __hwloc_inline void *
hwloc_tma_malloc(struct hwloc_tma *tma,
		 size_t size)
{
  if (tma) {
    return tma->malloc(tma, size);
  } else {
    return malloc(size);
  }
}

static __hwloc_inline void *
hwloc_tma_calloc(struct hwloc_tma *tma,
		 size_t size)
{
  char *ptr = hwloc_tma_malloc(tma, size);
  if (ptr)
    memset(ptr, 0, size);
  return ptr;
}

static __hwloc_inline char *
hwloc_tma_strdup(struct hwloc_tma *tma,
		 const char *src)
{
  size_t len = strlen(src);
  char *ptr = hwloc_tma_malloc(tma, len+1);
  if (ptr)
    memcpy(ptr, src, len+1);
  return ptr;
}


extern hwloc_bitmap_t hwloc_bitmap_tma_dup(struct hwloc_tma *tma, hwloc_const_bitmap_t old);

extern int hwloc__topology_dup(hwloc_topology_t *newp, hwloc_topology_t old, struct hwloc_tma *tma);
extern void hwloc__topology_disadopt(hwloc_topology_t  topology);

#endif 

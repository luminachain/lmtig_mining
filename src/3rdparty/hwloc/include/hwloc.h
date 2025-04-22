





#ifndef HWLOC_H
#define HWLOC_H

#include "hwloc/autogen/config.h"

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


#include "hwloc/rename.h"



#include "hwloc/bitmap.h"


#ifdef __cplusplus
extern "C" {
#endif








#define HWLOC_API_VERSION 0x00020b00


HWLOC_DECLSPEC unsigned hwloc_get_api_version(void);


#define HWLOC_COMPONENT_ABI 7








typedef hwloc_bitmap_t hwloc_cpuset_t;

typedef hwloc_const_bitmap_t hwloc_const_cpuset_t;


typedef hwloc_bitmap_t hwloc_nodeset_t;

typedef hwloc_const_bitmap_t hwloc_const_nodeset_t;








typedef enum {


#define HWLOC_OBJ_TYPE_MIN HWLOC_OBJ_MACHINE 


  HWLOC_OBJ_MACHINE,	

  HWLOC_OBJ_PACKAGE,	
  HWLOC_OBJ_CORE,	
  HWLOC_OBJ_PU,		

  HWLOC_OBJ_L1CACHE,	
  HWLOC_OBJ_L2CACHE,	
  HWLOC_OBJ_L3CACHE,	
  HWLOC_OBJ_L4CACHE,	
  HWLOC_OBJ_L5CACHE,	

  HWLOC_OBJ_L1ICACHE,	
  HWLOC_OBJ_L2ICACHE,	
  HWLOC_OBJ_L3ICACHE,	

  HWLOC_OBJ_GROUP,	

  HWLOC_OBJ_NUMANODE,	

  HWLOC_OBJ_BRIDGE,	
  HWLOC_OBJ_PCI_DEVICE,	
  HWLOC_OBJ_OS_DEVICE,	

  HWLOC_OBJ_MISC,	

  HWLOC_OBJ_MEMCACHE,	

  HWLOC_OBJ_DIE,	

  HWLOC_OBJ_TYPE_MAX    
} hwloc_obj_type_t;


typedef enum hwloc_obj_cache_type_e {
  HWLOC_OBJ_CACHE_UNIFIED,      
  HWLOC_OBJ_CACHE_DATA,         
  HWLOC_OBJ_CACHE_INSTRUCTION   
} hwloc_obj_cache_type_t;


typedef enum hwloc_obj_bridge_type_e {
  HWLOC_OBJ_BRIDGE_HOST,	
  HWLOC_OBJ_BRIDGE_PCI		
} hwloc_obj_bridge_type_t;


typedef enum hwloc_obj_osdev_type_e {
  HWLOC_OBJ_OSDEV_BLOCK,	
  HWLOC_OBJ_OSDEV_GPU,		
  HWLOC_OBJ_OSDEV_NETWORK,	
  HWLOC_OBJ_OSDEV_OPENFABRICS,	
  HWLOC_OBJ_OSDEV_DMA,		
  HWLOC_OBJ_OSDEV_COPROC	
} hwloc_obj_osdev_type_t;


HWLOC_DECLSPEC int hwloc_compare_types (hwloc_obj_type_t type1, hwloc_obj_type_t type2) __hwloc_attribute_const;


#define HWLOC_TYPE_UNORDERED INT_MAX







union hwloc_obj_attr_u;


struct hwloc_obj {
  
  hwloc_obj_type_t type;		
  char *subtype;			

  unsigned os_index;			
#define HWLOC_UNKNOWN_INDEX (unsigned)-1

  char *name;				

  hwloc_uint64_t total_memory; 

  union hwloc_obj_attr_u *attr;		

  
  int depth;				
  unsigned logical_index;		

  
  struct hwloc_obj *next_cousin;	
  struct hwloc_obj *prev_cousin;	

  
  struct hwloc_obj *parent;		
  unsigned sibling_rank;		
  struct hwloc_obj *next_sibling;	
  struct hwloc_obj *prev_sibling;	
  
  
  unsigned arity;			
  struct hwloc_obj **children;		
  struct hwloc_obj *first_child;	
  struct hwloc_obj *last_child;		
  

  int symmetric_subtree;		

  
  
  unsigned memory_arity;		
  struct hwloc_obj *memory_first_child;	
  

  
  
  unsigned io_arity;			
  struct hwloc_obj *io_first_child;	
  

  
  
  unsigned misc_arity;			
  struct hwloc_obj *misc_first_child;	
  

  
  hwloc_cpuset_t cpuset;		
  hwloc_cpuset_t complete_cpuset;       

  hwloc_nodeset_t nodeset;              
  hwloc_nodeset_t complete_nodeset;     

  struct hwloc_info_s *infos;		
  unsigned infos_count;			

  
  void *userdata;			

  hwloc_uint64_t gp_index;			
};

typedef struct hwloc_obj * hwloc_obj_t;


union hwloc_obj_attr_u {
  
  struct hwloc_numanode_attr_s {
    hwloc_uint64_t local_memory; 
    unsigned page_types_len; 
    
    struct hwloc_memory_page_type_s {
      hwloc_uint64_t size;	
      hwloc_uint64_t count;	
    } * page_types;
  } numanode;

  
  struct hwloc_cache_attr_s {
    hwloc_uint64_t size;		  
    unsigned depth;			  
    unsigned linesize;			  
    int associativity;			  
    hwloc_obj_cache_type_t type;          
  } cache;
  
  struct hwloc_group_attr_s {
    unsigned depth;			  
    unsigned kind;			  
    unsigned subkind;			  
    unsigned char dont_merge;		  
  } group;
  
  struct hwloc_pcidev_attr_s {
#ifndef HWLOC_HAVE_32BITS_PCI_DOMAIN
    unsigned short domain; 
#else
    unsigned int domain; 
#endif
    unsigned char bus;   
    unsigned char dev;   
    unsigned char func;  
    unsigned short class_id;  
    unsigned short vendor_id;    
    unsigned short device_id;    
    unsigned short subvendor_id; 
    unsigned short subdevice_id; 
    unsigned char revision;   
    float linkspeed; 
  } pcidev;
  
  struct hwloc_bridge_attr_s {
    union {
      struct hwloc_pcidev_attr_s pci; 
    } upstream;
    hwloc_obj_bridge_type_t upstream_type; 
    union {
      struct {
#ifndef HWLOC_HAVE_32BITS_PCI_DOMAIN
        unsigned short domain; 
#else
        unsigned int domain;   
#endif
        unsigned char secondary_bus;   
        unsigned char subordinate_bus; 
      } pci;
    } downstream;
    hwloc_obj_bridge_type_t downstream_type; 
    unsigned depth;
  } bridge;
  
  struct hwloc_osdev_attr_s {
    hwloc_obj_osdev_type_t type;
  } osdev;
};


struct hwloc_info_s {
  char *name;	
  char *value;	
};







struct hwloc_topology;

typedef struct hwloc_topology * hwloc_topology_t;


HWLOC_DECLSPEC int hwloc_topology_init (hwloc_topology_t *topologyp);


HWLOC_DECLSPEC int hwloc_topology_load(hwloc_topology_t topology);


HWLOC_DECLSPEC void hwloc_topology_destroy (hwloc_topology_t topology);


HWLOC_DECLSPEC int hwloc_topology_dup(hwloc_topology_t *newtopology, hwloc_topology_t oldtopology);


HWLOC_DECLSPEC int hwloc_topology_abi_check(hwloc_topology_t topology);


HWLOC_DECLSPEC void hwloc_topology_check(hwloc_topology_t topology);








HWLOC_DECLSPEC int hwloc_topology_get_depth(hwloc_topology_t __hwloc_restrict topology) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_get_type_depth (hwloc_topology_t topology, hwloc_obj_type_t type);

enum hwloc_get_type_depth_e {
    HWLOC_TYPE_DEPTH_UNKNOWN = -1,    
    HWLOC_TYPE_DEPTH_MULTIPLE = -2,   
    HWLOC_TYPE_DEPTH_NUMANODE = -3,   
    HWLOC_TYPE_DEPTH_BRIDGE = -4,     
    HWLOC_TYPE_DEPTH_PCI_DEVICE = -5, 
    HWLOC_TYPE_DEPTH_OS_DEVICE = -6,  
    HWLOC_TYPE_DEPTH_MISC = -7,       
    HWLOC_TYPE_DEPTH_MEMCACHE = -8    
};


HWLOC_DECLSPEC int hwloc_get_memory_parents_depth (hwloc_topology_t topology);


static __hwloc_inline int
hwloc_get_type_or_below_depth (hwloc_topology_t topology, hwloc_obj_type_t type) __hwloc_attribute_pure;


static __hwloc_inline int
hwloc_get_type_or_above_depth (hwloc_topology_t topology, hwloc_obj_type_t type) __hwloc_attribute_pure;


HWLOC_DECLSPEC hwloc_obj_type_t hwloc_get_depth_type (hwloc_topology_t topology, int depth) __hwloc_attribute_pure;


HWLOC_DECLSPEC unsigned hwloc_get_nbobjs_by_depth (hwloc_topology_t topology, int depth) __hwloc_attribute_pure;


static __hwloc_inline int
hwloc_get_nbobjs_by_type (hwloc_topology_t topology, hwloc_obj_type_t type) __hwloc_attribute_pure;


static __hwloc_inline hwloc_obj_t
hwloc_get_root_obj (hwloc_topology_t topology) __hwloc_attribute_pure;


HWLOC_DECLSPEC hwloc_obj_t hwloc_get_obj_by_depth (hwloc_topology_t topology, int depth, unsigned idx) __hwloc_attribute_pure;


static __hwloc_inline hwloc_obj_t
hwloc_get_obj_by_type (hwloc_topology_t topology, hwloc_obj_type_t type, unsigned idx) __hwloc_attribute_pure;


static __hwloc_inline hwloc_obj_t
hwloc_get_next_obj_by_depth (hwloc_topology_t topology, int depth, hwloc_obj_t prev);


static __hwloc_inline hwloc_obj_t
hwloc_get_next_obj_by_type (hwloc_topology_t topology, hwloc_obj_type_t type,
			    hwloc_obj_t prev);








HWLOC_DECLSPEC const char * hwloc_obj_type_string (hwloc_obj_type_t type) __hwloc_attribute_const;


HWLOC_DECLSPEC int hwloc_obj_type_snprintf(char * __hwloc_restrict string, size_t size,
					   hwloc_obj_t obj,
					   int verbose);


HWLOC_DECLSPEC int hwloc_obj_attr_snprintf(char * __hwloc_restrict string, size_t size,
					   hwloc_obj_t obj, const char * __hwloc_restrict separator,
					   int verbose);


HWLOC_DECLSPEC int hwloc_type_sscanf(const char *string,
				     hwloc_obj_type_t *typep,
				     union hwloc_obj_attr_u *attrp, size_t attrsize);


HWLOC_DECLSPEC int hwloc_type_sscanf_as_depth(const char *string,
					      hwloc_obj_type_t *typep,
					      hwloc_topology_t topology, int *depthp);








static __hwloc_inline const char *
hwloc_obj_get_info_by_name(hwloc_obj_t obj, const char *name) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_obj_add_info(hwloc_obj_t obj, const char *name, const char *value);


HWLOC_DECLSPEC int hwloc_obj_set_subtype(hwloc_topology_t topology, hwloc_obj_t obj, const char *subtype);








typedef enum {
  
  HWLOC_CPUBIND_PROCESS = (1<<0),

  
  HWLOC_CPUBIND_THREAD = (1<<1),

  
  HWLOC_CPUBIND_STRICT = (1<<2),

  
  HWLOC_CPUBIND_NOMEMBIND = (1<<3)
} hwloc_cpubind_flags_t;


HWLOC_DECLSPEC int hwloc_set_cpubind(hwloc_topology_t topology, hwloc_const_cpuset_t set, int flags);


HWLOC_DECLSPEC int hwloc_get_cpubind(hwloc_topology_t topology, hwloc_cpuset_t set, int flags);


HWLOC_DECLSPEC int hwloc_set_proc_cpubind(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_const_cpuset_t set, int flags);


HWLOC_DECLSPEC int hwloc_get_proc_cpubind(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_cpuset_t set, int flags);

#ifdef hwloc_thread_t

HWLOC_DECLSPEC int hwloc_set_thread_cpubind(hwloc_topology_t topology, hwloc_thread_t thread, hwloc_const_cpuset_t set, int flags);
#endif

#ifdef hwloc_thread_t

HWLOC_DECLSPEC int hwloc_get_thread_cpubind(hwloc_topology_t topology, hwloc_thread_t thread, hwloc_cpuset_t set, int flags);
#endif


HWLOC_DECLSPEC int hwloc_get_last_cpu_location(hwloc_topology_t topology, hwloc_cpuset_t set, int flags);


HWLOC_DECLSPEC int hwloc_get_proc_last_cpu_location(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_cpuset_t set, int flags);








typedef enum {
  
  HWLOC_MEMBIND_DEFAULT =	0,

  
  HWLOC_MEMBIND_FIRSTTOUCH =	1,

  
  HWLOC_MEMBIND_BIND =		2,

  
  HWLOC_MEMBIND_INTERLEAVE =	3,

  
  HWLOC_MEMBIND_WEIGHTED_INTERLEAVE = 5,

  
  HWLOC_MEMBIND_NEXTTOUCH =	4,

  
  HWLOC_MEMBIND_MIXED = -1
} hwloc_membind_policy_t;


typedef enum {
  
  HWLOC_MEMBIND_PROCESS =       (1<<0),

 
  HWLOC_MEMBIND_THREAD =        (1<<1),

 
  HWLOC_MEMBIND_STRICT =        (1<<2),

 
  HWLOC_MEMBIND_MIGRATE =       (1<<3),

  
  HWLOC_MEMBIND_NOCPUBIND =     (1<<4),

  
  HWLOC_MEMBIND_BYNODESET =     (1<<5)
} hwloc_membind_flags_t;


HWLOC_DECLSPEC int hwloc_set_membind(hwloc_topology_t topology, hwloc_const_bitmap_t set, hwloc_membind_policy_t policy, int flags);


HWLOC_DECLSPEC int hwloc_get_membind(hwloc_topology_t topology, hwloc_bitmap_t set, hwloc_membind_policy_t * policy, int flags);


HWLOC_DECLSPEC int hwloc_set_proc_membind(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_const_bitmap_t set, hwloc_membind_policy_t policy, int flags);


HWLOC_DECLSPEC int hwloc_get_proc_membind(hwloc_topology_t topology, hwloc_pid_t pid, hwloc_bitmap_t set, hwloc_membind_policy_t * policy, int flags);


HWLOC_DECLSPEC int hwloc_set_area_membind(hwloc_topology_t topology, const void *addr, size_t len, hwloc_const_bitmap_t set, hwloc_membind_policy_t policy, int flags);


HWLOC_DECLSPEC int hwloc_get_area_membind(hwloc_topology_t topology, const void *addr, size_t len, hwloc_bitmap_t set, hwloc_membind_policy_t * policy, int flags);


HWLOC_DECLSPEC int hwloc_get_area_memlocation(hwloc_topology_t topology, const void *addr, size_t len, hwloc_bitmap_t set, int flags);


HWLOC_DECLSPEC void *hwloc_alloc(hwloc_topology_t topology, size_t len);


HWLOC_DECLSPEC void *hwloc_alloc_membind(hwloc_topology_t topology, size_t len, hwloc_const_bitmap_t set, hwloc_membind_policy_t policy, int flags) __hwloc_attribute_malloc;


static __hwloc_inline void *
hwloc_alloc_membind_policy(hwloc_topology_t topology, size_t len, hwloc_const_bitmap_t set, hwloc_membind_policy_t policy, int flags) __hwloc_attribute_malloc;


HWLOC_DECLSPEC int hwloc_free(hwloc_topology_t topology, void *addr, size_t len);








HWLOC_DECLSPEC int hwloc_topology_set_pid(hwloc_topology_t __hwloc_restrict topology, hwloc_pid_t pid);


HWLOC_DECLSPEC int hwloc_topology_set_synthetic(hwloc_topology_t __hwloc_restrict topology, const char * __hwloc_restrict description);


HWLOC_DECLSPEC int hwloc_topology_set_xml(hwloc_topology_t __hwloc_restrict topology, const char * __hwloc_restrict xmlpath);


HWLOC_DECLSPEC int hwloc_topology_set_xmlbuffer(hwloc_topology_t __hwloc_restrict topology, const char * __hwloc_restrict buffer, int size);


enum hwloc_topology_components_flag_e {
  
  HWLOC_TOPOLOGY_COMPONENTS_FLAG_BLACKLIST = (1UL<<0)
};


HWLOC_DECLSPEC int hwloc_topology_set_components(hwloc_topology_t __hwloc_restrict topology, unsigned long flags, const char * __hwloc_restrict name);








enum hwloc_topology_flags_e {
 
  HWLOC_TOPOLOGY_FLAG_INCLUDE_DISALLOWED = (1UL<<0),

 
  HWLOC_TOPOLOGY_FLAG_IS_THISSYSTEM = (1UL<<1),

 
  HWLOC_TOPOLOGY_FLAG_THISSYSTEM_ALLOWED_RESOURCES = (1UL<<2),

  
  HWLOC_TOPOLOGY_FLAG_IMPORT_SUPPORT = (1UL<<3),

  
  HWLOC_TOPOLOGY_FLAG_RESTRICT_TO_CPUBINDING = (1UL<<4),

  
  HWLOC_TOPOLOGY_FLAG_RESTRICT_TO_MEMBINDING = (1UL<<5),

  
  HWLOC_TOPOLOGY_FLAG_DONT_CHANGE_BINDING = (1UL<<6),

  
  HWLOC_TOPOLOGY_FLAG_NO_DISTANCES = (1UL<<7),

  
  HWLOC_TOPOLOGY_FLAG_NO_MEMATTRS = (1UL<<8),

  
  HWLOC_TOPOLOGY_FLAG_NO_CPUKINDS = (1UL<<9)
};


HWLOC_DECLSPEC int hwloc_topology_set_flags (hwloc_topology_t topology, unsigned long flags);


HWLOC_DECLSPEC unsigned long hwloc_topology_get_flags (hwloc_topology_t topology);


HWLOC_DECLSPEC int hwloc_topology_is_thissystem(hwloc_topology_t  __hwloc_restrict topology) __hwloc_attribute_pure;


struct hwloc_topology_discovery_support {
  
  unsigned char pu;
  
  unsigned char numa;
  
  unsigned char numa_memory;
  
  unsigned char disallowed_pu;
  
  unsigned char disallowed_numa;
  
  unsigned char cpukind_efficiency;
};


struct hwloc_topology_cpubind_support {
  
  unsigned char set_thisproc_cpubind;
  
  unsigned char get_thisproc_cpubind;
  
  unsigned char set_proc_cpubind;
  
  unsigned char get_proc_cpubind;
  
  unsigned char set_thisthread_cpubind;
  
  unsigned char get_thisthread_cpubind;
  
  unsigned char set_thread_cpubind;
  
  unsigned char get_thread_cpubind;
  
  unsigned char get_thisproc_last_cpu_location;
  
  unsigned char get_proc_last_cpu_location;
  
  unsigned char get_thisthread_last_cpu_location;
};


struct hwloc_topology_membind_support {
  
  unsigned char set_thisproc_membind;
  
  unsigned char get_thisproc_membind;
  
  unsigned char set_proc_membind;
  
  unsigned char get_proc_membind;
  
  unsigned char set_thisthread_membind;
  
  unsigned char get_thisthread_membind;
  
  unsigned char set_area_membind;
  
  unsigned char get_area_membind;
  
  unsigned char alloc_membind;
  
  unsigned char firsttouch_membind;
  
  unsigned char bind_membind;
  
  unsigned char interleave_membind;
  
  unsigned char nexttouch_membind;
  
  unsigned char migrate_membind;
  
  unsigned char get_area_memlocation;
  
  unsigned char weighted_interleave_membind;
};


struct hwloc_topology_misc_support {
  
  unsigned char imported_support;
};


struct hwloc_topology_support {
  struct hwloc_topology_discovery_support *discovery;
  struct hwloc_topology_cpubind_support *cpubind;
  struct hwloc_topology_membind_support *membind;
  struct hwloc_topology_misc_support *misc;
};


HWLOC_DECLSPEC const struct hwloc_topology_support *hwloc_topology_get_support(hwloc_topology_t __hwloc_restrict topology);


enum hwloc_type_filter_e {
  
  HWLOC_TYPE_FILTER_KEEP_ALL = 0,

  
  HWLOC_TYPE_FILTER_KEEP_NONE = 1,

  
  HWLOC_TYPE_FILTER_KEEP_STRUCTURE = 2,

  
  HWLOC_TYPE_FILTER_KEEP_IMPORTANT = 3
};


HWLOC_DECLSPEC int hwloc_topology_set_type_filter(hwloc_topology_t topology, hwloc_obj_type_t type, enum hwloc_type_filter_e filter);


HWLOC_DECLSPEC int hwloc_topology_get_type_filter(hwloc_topology_t topology, hwloc_obj_type_t type, enum hwloc_type_filter_e *filter);


HWLOC_DECLSPEC int hwloc_topology_set_all_types_filter(hwloc_topology_t topology, enum hwloc_type_filter_e filter);


HWLOC_DECLSPEC int hwloc_topology_set_cache_types_filter(hwloc_topology_t topology, enum hwloc_type_filter_e filter);


HWLOC_DECLSPEC int hwloc_topology_set_icache_types_filter(hwloc_topology_t topology, enum hwloc_type_filter_e filter);


HWLOC_DECLSPEC int hwloc_topology_set_io_types_filter(hwloc_topology_t topology, enum hwloc_type_filter_e filter);


HWLOC_DECLSPEC void hwloc_topology_set_userdata(hwloc_topology_t topology, const void *userdata);


HWLOC_DECLSPEC void * hwloc_topology_get_userdata(hwloc_topology_t topology);








enum hwloc_restrict_flags_e {
  
  HWLOC_RESTRICT_FLAG_REMOVE_CPULESS = (1UL<<0),

  
  HWLOC_RESTRICT_FLAG_BYNODESET =  (1UL<<3),

  
  HWLOC_RESTRICT_FLAG_REMOVE_MEMLESS = (1UL<<4),

  
  HWLOC_RESTRICT_FLAG_ADAPT_MISC = (1UL<<1),

  
  HWLOC_RESTRICT_FLAG_ADAPT_IO = (1UL<<2)
};


HWLOC_DECLSPEC int hwloc_topology_restrict(hwloc_topology_t __hwloc_restrict topology, hwloc_const_bitmap_t set, unsigned long flags);


enum hwloc_allow_flags_e {
  
  HWLOC_ALLOW_FLAG_ALL = (1UL<<0),

  
  HWLOC_ALLOW_FLAG_LOCAL_RESTRICTIONS = (1UL<<1),

  
  HWLOC_ALLOW_FLAG_CUSTOM = (1UL<<2)
};


HWLOC_DECLSPEC int hwloc_topology_allow(hwloc_topology_t __hwloc_restrict topology, hwloc_const_cpuset_t cpuset, hwloc_const_nodeset_t nodeset, unsigned long flags);


HWLOC_DECLSPEC hwloc_obj_t hwloc_topology_insert_misc_object(hwloc_topology_t topology, hwloc_obj_t parent, const char *name);


HWLOC_DECLSPEC hwloc_obj_t hwloc_topology_alloc_group_object(hwloc_topology_t topology);


HWLOC_DECLSPEC int hwloc_topology_free_group_object(hwloc_topology_t topology, hwloc_obj_t group);


HWLOC_DECLSPEC hwloc_obj_t hwloc_topology_insert_group_object(hwloc_topology_t topology, hwloc_obj_t group);


HWLOC_DECLSPEC int hwloc_obj_add_other_obj_sets(hwloc_obj_t dst, hwloc_obj_t src);


HWLOC_DECLSPEC int hwloc_topology_refresh(hwloc_topology_t topology);





#ifdef __cplusplus
} 
#endif



#include "hwloc/helper.h"


#include "hwloc/inlines.h"


#include "hwloc/memattrs.h"


#include "hwloc/cpukinds.h"


#include "hwloc/export.h"


#include "hwloc/distances.h"


#include "hwloc/diff.h"


#include "hwloc/deprecated.h"

#endif 

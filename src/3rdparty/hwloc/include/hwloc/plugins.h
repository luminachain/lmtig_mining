

#ifndef HWLOC_PLUGINS_H
#define HWLOC_PLUGINS_H



struct hwloc_backend;

#include "hwloc.h"

#ifdef HWLOC_INSIDE_PLUGIN

#ifdef HWLOC_HAVE_LTDL
#include <ltdl.h>
#else
#include <dlfcn.h>
#endif
#endif






struct hwloc_disc_component {
  
  const char *name;

  
  unsigned phases;

  
  unsigned excluded_phases;

  
  struct hwloc_backend * (*instantiate)(struct hwloc_topology *topology, struct hwloc_disc_component *component, unsigned excluded_phases, const void *data1, const void *data2, const void *data3);

  
  unsigned priority;

  
  unsigned enabled_by_default;

  
  struct hwloc_disc_component * next;
};









typedef enum hwloc_disc_phase_e {
  
  HWLOC_DISC_PHASE_GLOBAL = (1U<<0),

  
  HWLOC_DISC_PHASE_CPU = (1U<<1),

  
  HWLOC_DISC_PHASE_MEMORY = (1U<<2),

  
  HWLOC_DISC_PHASE_PCI = (1U<<3),

  
  HWLOC_DISC_PHASE_IO = (1U<<4),

  
  HWLOC_DISC_PHASE_MISC = (1U<<5),

  
  HWLOC_DISC_PHASE_ANNOTATE = (1U<<6),

  
  HWLOC_DISC_PHASE_TWEAK = (1U<<7)
} hwloc_disc_phase_t;


enum hwloc_disc_status_flag_e {
  
  HWLOC_DISC_STATUS_FLAG_GOT_ALLOWED_RESOURCES = (1UL<<1)
};


struct hwloc_disc_status {
  
  hwloc_disc_phase_t phase;

  
  unsigned excluded_phases;

  
  unsigned long flags;
};


struct hwloc_backend {
  
  struct hwloc_disc_component * component;
  
  struct hwloc_topology * topology;
  
  int envvar_forced;
  
  struct hwloc_backend * next;

  
  unsigned phases;

  
  unsigned long flags;

  
  int is_thissystem;

  
  void * private_data;
  
  void (*disable)(struct hwloc_backend *backend);

  
  int (*discover)(struct hwloc_backend *backend, struct hwloc_disc_status *status);

  
  int (*get_pci_busid_cpuset)(struct hwloc_backend *backend, struct hwloc_pcidev_attr_s *busid, hwloc_bitmap_t cpuset);
};


HWLOC_DECLSPEC struct hwloc_backend * hwloc_backend_alloc(struct hwloc_topology *topology, struct hwloc_disc_component *component);


HWLOC_DECLSPEC int hwloc_backend_enable(struct hwloc_backend *backend);









typedef enum hwloc_component_type_e {
  
  HWLOC_COMPONENT_TYPE_DISC,

  
  HWLOC_COMPONENT_TYPE_XML
} hwloc_component_type_t;


struct hwloc_component {
  
  unsigned abi;

  
  int (*init)(unsigned long flags);

  
  void (*finalize)(unsigned long flags);

  
  hwloc_component_type_t type;

  
  unsigned long flags;

  
  void * data;
};









HWLOC_DECLSPEC int hwloc_hide_errors(void);

#define HWLOC_SHOW_CRITICAL_ERRORS() (hwloc_hide_errors() < 2)
#define HWLOC_SHOW_ALL_ERRORS() (hwloc_hide_errors() == 0)


HWLOC_DECLSPEC hwloc_obj_t
hwloc__insert_object_by_cpuset(struct hwloc_topology *topology, hwloc_obj_t root,
                               hwloc_obj_t obj, const char *reason);


HWLOC_DECLSPEC void hwloc_insert_object_by_parent(struct hwloc_topology *topology, hwloc_obj_t parent, hwloc_obj_t obj);


HWLOC_DECLSPEC hwloc_obj_t hwloc_alloc_setup_object(hwloc_topology_t topology, hwloc_obj_type_t type, unsigned os_index);


HWLOC_DECLSPEC int hwloc_obj_add_children_sets(hwloc_obj_t obj);


HWLOC_DECLSPEC int hwloc_topology_reconnect(hwloc_topology_t topology, unsigned long flags __hwloc_attribute_unused);


static __hwloc_inline int
hwloc_plugin_check_namespace(const char *pluginname __hwloc_attribute_unused, const char *symbol __hwloc_attribute_unused)
{
#ifdef HWLOC_INSIDE_PLUGIN
  void *sym;
#ifdef HWLOC_HAVE_LTDL
  lt_dlhandle handle = lt_dlopen(NULL);
#else
  void *handle = dlopen(NULL, RTLD_NOW|RTLD_LOCAL);
#endif
  if (!handle)
    
    return 0;
#ifdef HWLOC_HAVE_LTDL
  sym = lt_dlsym(handle, symbol);
  lt_dlclose(handle);
#else
  sym = dlsym(handle, symbol);
  dlclose(handle);
#endif
  if (!sym) {
    static int verboseenv_checked = 0;
    static int verboseenv_value = 0;
    if (!verboseenv_checked) {
      const char *verboseenv = getenv("HWLOC_PLUGINS_VERBOSE");
      verboseenv_value = verboseenv ? atoi(verboseenv) : 0;
      verboseenv_checked = 1;
    }
    if (verboseenv_value)
      fprintf(stderr, "Plugin `%s' disabling itself because it cannot find the `%s' core symbol.\n",
	      pluginname, symbol);
    return -1;
  }
#endif 
  return 0;
}









static __hwloc_inline int
hwloc_filter_check_pcidev_subtype_important(unsigned classid)
{
  unsigned baseclass = classid >> 8;
  return (baseclass == 0x03 
	  || baseclass == 0x02 
	  || baseclass == 0x01 
	  || baseclass == 0x00 
	  || baseclass == 0x0b 
	  || classid == 0x0c04 
	  || classid == 0x0c06 
          || classid == 0x0502 
          || baseclass == 0x06 
	  || baseclass == 0x12 );
}


static __hwloc_inline int
hwloc_filter_check_osdev_subtype_important(hwloc_obj_osdev_type_t subtype)
{
  return (subtype != HWLOC_OBJ_OSDEV_DMA);
}


static __hwloc_inline int
hwloc_filter_check_keep_object_type(hwloc_topology_t topology, hwloc_obj_type_t type)
{
  enum hwloc_type_filter_e filter = HWLOC_TYPE_FILTER_KEEP_NONE;
  hwloc_topology_get_type_filter(topology, type, &filter);
  assert(filter != HWLOC_TYPE_FILTER_KEEP_IMPORTANT); 
  return filter == HWLOC_TYPE_FILTER_KEEP_NONE ? 0 : 1;
}


static __hwloc_inline int
hwloc_filter_check_keep_object(hwloc_topology_t topology, hwloc_obj_t obj)
{
  hwloc_obj_type_t type = obj->type;
  enum hwloc_type_filter_e filter = HWLOC_TYPE_FILTER_KEEP_NONE;
  hwloc_topology_get_type_filter(topology, type, &filter);
  if (filter == HWLOC_TYPE_FILTER_KEEP_NONE)
    return 0;
  if (filter == HWLOC_TYPE_FILTER_KEEP_IMPORTANT) {
    if (type == HWLOC_OBJ_PCI_DEVICE)
      return hwloc_filter_check_pcidev_subtype_important(obj->attr->pcidev.class_id);
    if (type == HWLOC_OBJ_OS_DEVICE)
      return hwloc_filter_check_osdev_subtype_important(obj->attr->osdev.type);
  }
  return 1;
}









HWLOC_DECLSPEC unsigned hwloc_pcidisc_find_cap(const unsigned char *config, unsigned cap);


HWLOC_DECLSPEC int hwloc_pcidisc_find_linkspeed(const unsigned char *config, unsigned offset, float *linkspeed);


HWLOC_DECLSPEC hwloc_obj_type_t hwloc_pcidisc_check_bridge_type(unsigned device_class, const unsigned char *config);


HWLOC_DECLSPEC int hwloc_pcidisc_find_bridge_buses(unsigned domain, unsigned bus, unsigned dev, unsigned func,
						   unsigned *secondary_busp, unsigned *subordinate_busp,
						   const unsigned char *config);


HWLOC_DECLSPEC void hwloc_pcidisc_tree_insert_by_busid(struct hwloc_obj **treep, struct hwloc_obj *obj);


HWLOC_DECLSPEC int hwloc_pcidisc_tree_attach(struct hwloc_topology *topology, struct hwloc_obj *tree);









HWLOC_DECLSPEC struct hwloc_obj * hwloc_pci_find_parent_by_busid(struct hwloc_topology *topology, unsigned domain, unsigned bus, unsigned dev, unsigned func);


HWLOC_DECLSPEC struct hwloc_obj * hwloc_pci_find_by_busid(struct hwloc_topology *topology, unsigned domain, unsigned bus, unsigned dev, unsigned func);










typedef void * hwloc_backend_distances_add_handle_t;


HWLOC_DECLSPEC hwloc_backend_distances_add_handle_t
hwloc_backend_distances_add_create(hwloc_topology_t topology,
                                   const char *name, unsigned long kind,
                                   unsigned long flags);


HWLOC_DECLSPEC int
hwloc_backend_distances_add_values(hwloc_topology_t topology,
                                   hwloc_backend_distances_add_handle_t handle,
                                   unsigned nbobjs, hwloc_obj_t *objs,
                                   hwloc_uint64_t *values,
                                   unsigned long flags);


HWLOC_DECLSPEC int
hwloc_backend_distances_add_commit(hwloc_topology_t topology,
                                   hwloc_backend_distances_add_handle_t handle,
                                   unsigned long flags);






#endif 

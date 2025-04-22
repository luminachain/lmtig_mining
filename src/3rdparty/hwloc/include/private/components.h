


#ifdef HWLOC_INSIDE_PLUGIN

#error This file should not be used in plugins
#endif


#ifndef PRIVATE_COMPONENTS_H
#define PRIVATE_COMPONENTS_H 1

#include "hwloc/plugins.h"

struct hwloc_topology;

extern int hwloc_disc_component_force_enable(struct hwloc_topology *topology,
					     int envvar_forced, 
					     const char *name,
					     const void *data1, const void *data2, const void *data3);
extern void hwloc_disc_components_enable_others(struct hwloc_topology *topology);


extern void hwloc_backends_is_thissystem(struct hwloc_topology *topology);
extern void hwloc_backends_find_callbacks(struct hwloc_topology *topology);


extern void hwloc_topology_components_init(struct hwloc_topology *topology);

extern void hwloc_backends_disable_all(struct hwloc_topology *topology);

extern void hwloc_topology_components_fini(struct hwloc_topology *topology);


extern void hwloc_components_init(void); 
extern void hwloc_components_fini(void); 

#endif 






#ifndef HWLOC_EXPORT_H
#define HWLOC_EXPORT_H

#ifndef HWLOC_H
#error Please include the main hwloc.h instead
#endif


#ifdef __cplusplus
extern "C" {
#elif 0
}
#endif





enum hwloc_topology_export_xml_flags_e {
 
 HWLOC_TOPOLOGY_EXPORT_XML_FLAG_V1 = (1UL<<0)
};


HWLOC_DECLSPEC int hwloc_topology_export_xml(hwloc_topology_t topology, const char *xmlpath, unsigned long flags);


HWLOC_DECLSPEC int hwloc_topology_export_xmlbuffer(hwloc_topology_t topology, char **xmlbuffer, int *buflen, unsigned long flags);


HWLOC_DECLSPEC void hwloc_free_xmlbuffer(hwloc_topology_t topology, char *xmlbuffer);


HWLOC_DECLSPEC void hwloc_topology_set_userdata_export_callback(hwloc_topology_t topology,
								void (*export_cb)(void *reserved, hwloc_topology_t topology, hwloc_obj_t obj));


HWLOC_DECLSPEC int hwloc_export_obj_userdata(void *reserved, hwloc_topology_t topology, hwloc_obj_t obj, const char *name, const void *buffer, size_t length);


HWLOC_DECLSPEC int hwloc_export_obj_userdata_base64(void *reserved, hwloc_topology_t topology, hwloc_obj_t obj, const char *name, const void *buffer, size_t length);


HWLOC_DECLSPEC void hwloc_topology_set_userdata_import_callback(hwloc_topology_t topology,
								void (*import_cb)(hwloc_topology_t topology, hwloc_obj_t obj, const char *name, const void *buffer, size_t length));







enum hwloc_topology_export_synthetic_flags_e {
 
 HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_EXTENDED_TYPES = (1UL<<0),

 
 HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_ATTRS = (1UL<<1),

 
 HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_V1 = (1UL<<2),

 
 HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_IGNORE_MEMORY = (1UL<<3)
};


  HWLOC_DECLSPEC int hwloc_topology_export_synthetic(hwloc_topology_t topology, char *buffer, size_t buflen, unsigned long flags);





#ifdef __cplusplus
} 
#endif


#endif 

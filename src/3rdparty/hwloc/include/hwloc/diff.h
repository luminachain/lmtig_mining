



#ifndef HWLOC_DIFF_H
#define HWLOC_DIFF_H

#ifndef HWLOC_H
#error Please include the main hwloc.h instead
#endif


#ifdef __cplusplus
extern "C" {
#elif 0
}
#endif






typedef enum hwloc_topology_diff_obj_attr_type_e {
  
  HWLOC_TOPOLOGY_DIFF_OBJ_ATTR_SIZE,

  

  HWLOC_TOPOLOGY_DIFF_OBJ_ATTR_NAME,
  
  HWLOC_TOPOLOGY_DIFF_OBJ_ATTR_INFO
} hwloc_topology_diff_obj_attr_type_t;


union hwloc_topology_diff_obj_attr_u {
  struct hwloc_topology_diff_obj_attr_generic_s {
    
    hwloc_topology_diff_obj_attr_type_t type;
  } generic;

  
  struct hwloc_topology_diff_obj_attr_uint64_s {
    
    hwloc_topology_diff_obj_attr_type_t type;
    hwloc_uint64_t index; 
    hwloc_uint64_t oldvalue;
    hwloc_uint64_t newvalue;
  } uint64;

  
  struct hwloc_topology_diff_obj_attr_string_s {
    
    hwloc_topology_diff_obj_attr_type_t type;
    char *name; 
    char *oldvalue;
    char *newvalue;
  } string;
};



typedef enum hwloc_topology_diff_type_e {
  
  HWLOC_TOPOLOGY_DIFF_OBJ_ATTR,

  
  HWLOC_TOPOLOGY_DIFF_TOO_COMPLEX
} hwloc_topology_diff_type_t;


typedef union hwloc_topology_diff_u {
  struct hwloc_topology_diff_generic_s {
    
    hwloc_topology_diff_type_t type;
    union hwloc_topology_diff_u * next; 
  } generic;

  
  struct hwloc_topology_diff_obj_attr_s {
    hwloc_topology_diff_type_t type; 
    union hwloc_topology_diff_u * next;
    
    int obj_depth;
    unsigned obj_index;
    union hwloc_topology_diff_obj_attr_u diff;
  } obj_attr;

  
  struct hwloc_topology_diff_too_complex_s {
    hwloc_topology_diff_type_t type; 
    union hwloc_topology_diff_u * next;
    
    int obj_depth;
    unsigned obj_index;
  } too_complex;
} * hwloc_topology_diff_t;



HWLOC_DECLSPEC int hwloc_topology_diff_build(hwloc_topology_t topology, hwloc_topology_t newtopology, unsigned long flags, hwloc_topology_diff_t *diff);


enum hwloc_topology_diff_apply_flags_e {
  
  HWLOC_TOPOLOGY_DIFF_APPLY_REVERSE = (1UL<<0)
};


HWLOC_DECLSPEC int hwloc_topology_diff_apply(hwloc_topology_t topology, hwloc_topology_diff_t diff, unsigned long flags);


HWLOC_DECLSPEC int hwloc_topology_diff_destroy(hwloc_topology_diff_t diff);


HWLOC_DECLSPEC int hwloc_topology_diff_load_xml(const char *xmlpath, hwloc_topology_diff_t *diff, char **refname);


HWLOC_DECLSPEC int hwloc_topology_diff_export_xml(hwloc_topology_diff_t diff, const char *refname, const char *xmlpath);


HWLOC_DECLSPEC int hwloc_topology_diff_load_xmlbuffer(const char *xmlbuffer, int buflen, hwloc_topology_diff_t *diff, char **refname);


HWLOC_DECLSPEC int hwloc_topology_diff_export_xmlbuffer(hwloc_topology_diff_t diff, const char *refname, char **xmlbuffer, int *buflen);




#ifdef __cplusplus
} 
#endif


#endif 

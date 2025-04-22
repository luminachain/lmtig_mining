


#ifdef HWLOC_INSIDE_PLUGIN

#error This file should not be used in plugins
#endif


#ifndef HWLOC_PRIVATE_SOLARIS_CHIPTYPE_H
#define HWLOC_PRIVATE_SOLARIS_CHIPTYPE_H

struct hwloc_solaris_chip_info_s {
  char *model;
  char *type;
  
#define HWLOC_SOLARIS_CHIP_INFO_L1I 0
#define HWLOC_SOLARIS_CHIP_INFO_L1D 1
#define HWLOC_SOLARIS_CHIP_INFO_L2I 2
#define HWLOC_SOLARIS_CHIP_INFO_L2D 3
#define HWLOC_SOLARIS_CHIP_INFO_L3  4
  long cache_size[5]; 
  unsigned cache_linesize[5];
  unsigned cache_associativity[5];
  int l2_unified;
};


extern void hwloc_solaris_get_chip_info(struct hwloc_solaris_chip_info_s *info);

#endif 



#include "private/autogen/config.h"
#include "hwloc.h"
#include "private/private.h"
#include "private/debug.h"
#include "private/misc.h"
#include "private/cpuid-x86.h"

#include <sys/types.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_VALGRIND_VALGRIND_H
#include <valgrind/valgrind.h>
#endif

struct hwloc_x86_backend_data_s {
  unsigned nbprocs;
  hwloc_bitmap_t apicid_set;
  int apicid_unique;
  char *src_cpuiddump_path;
  int is_knl;
  int is_hybrid;
  int found_die_ids;
  int found_complex_ids;
  int found_unit_ids;
  int found_module_ids;
  int found_tile_ids;
};



struct cpuiddump {
  unsigned nr;
  struct cpuiddump_entry {
    unsigned inmask; 
    unsigned ineax;
    unsigned inebx;
    unsigned inecx;
    unsigned inedx;
    unsigned outeax;
    unsigned outebx;
    unsigned outecx;
    unsigned outedx;
  } *entries;
};

static void
cpuiddump_free(struct cpuiddump *cpuiddump)
{
  if (cpuiddump->nr)
    free(cpuiddump->entries);
  free(cpuiddump);
}

static struct cpuiddump *
cpuiddump_read(const char *dirpath, unsigned idx)
{
  struct cpuiddump *cpuiddump;
  struct cpuiddump_entry *cur;
  size_t filenamelen;
  char *filename;
  FILE *file;
  char line[128];
  unsigned nr;

  cpuiddump = malloc(sizeof(*cpuiddump));
  if (!cpuiddump) {
    fprintf(stderr, "hwloc/x86: Failed to allocate cpuiddump for PU #%u, ignoring cpuiddump.\n", idx);
    goto out;
  }

  filenamelen = strlen(dirpath) + 15;
  filename = malloc(filenamelen);
  if (!filename)
    goto out_with_dump;
  snprintf(filename, filenamelen, "%s/pu%u", dirpath, idx);
  file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "hwloc/x86: Could not read dumped cpuid file %s, ignoring cpuiddump.\n", filename);
    goto out_with_filename;
  }

  nr = 0;
  while (fgets(line, sizeof(line), file))
    nr++;
  cpuiddump->entries = malloc(nr * sizeof(struct cpuiddump_entry));
  if (!cpuiddump->entries) {
    fprintf(stderr, "hwloc/x86: Failed to allocate %u cpuiddump entries for PU #%u, ignoring cpuiddump.\n", nr, idx);
    goto out_with_file;
  }

  fseek(file, 0, SEEK_SET);
  cur = &cpuiddump->entries[0];
  nr = 0;
  while (fgets(line, sizeof(line), file)) {
    if (*line == '#')
      continue;
    if (sscanf(line, "%x %x %x %x %x => %x %x %x %x",
	      &cur->inmask,
	      &cur->ineax, &cur->inebx, &cur->inecx, &cur->inedx,
	      &cur->outeax, &cur->outebx, &cur->outecx, &cur->outedx) == 9) {
      cur++;
      nr++;
    }
  }

  cpuiddump->nr = nr;
  fclose(file);
  free(filename);
  return cpuiddump;

 out_with_file:
  fclose(file);
 out_with_filename:
  free(filename);
 out_with_dump:
  free(cpuiddump);
 out:
  return NULL;
}

static void
cpuiddump_find_by_input(unsigned *eax, unsigned *ebx, unsigned *ecx, unsigned *edx, struct cpuiddump *cpuiddump)
{
  unsigned i;

  for(i=0; i<cpuiddump->nr; i++) {
    struct cpuiddump_entry *entry = &cpuiddump->entries[i];
    if ((entry->inmask & 0x1) && *eax != entry->ineax)
      continue;
    if ((entry->inmask & 0x2) && *ebx != entry->inebx)
      continue;
    if ((entry->inmask & 0x4) && *ecx != entry->inecx)
      continue;
    if ((entry->inmask & 0x8) && *edx != entry->inedx)
      continue;
    *eax = entry->outeax;
    *ebx = entry->outebx;
    *ecx = entry->outecx;
    *edx = entry->outedx;
    return;
  }

  fprintf(stderr, "hwloc/x86: Couldn't find %x,%x,%x,%x in dumped cpuid, returning 0s.\n",
	  *eax, *ebx, *ecx, *edx);
  *eax = 0;
  *ebx = 0;
  *ecx = 0;
  *edx = 0;
}

static void cpuid_or_from_dump(unsigned *eax, unsigned *ebx, unsigned *ecx, unsigned *edx, struct cpuiddump *src_cpuiddump)
{
  if (src_cpuiddump) {
    cpuiddump_find_by_input(eax, ebx, ecx, edx, src_cpuiddump);
  } else {
    hwloc_x86_cpuid(eax, ebx, ecx, edx);
  }
}



enum hwloc_x86_disc_flags {
  HWLOC_X86_DISC_FLAG_FULL = (1<<0), 
  HWLOC_X86_DISC_FLAG_TOPOEXT_NUMANODES = (1<<1) 
};

#define has_topoext(features) ((features)[6] & (1 << 22))
#define has_x2apic(features) ((features)[4] & (1 << 21))
#define has_hybrid(features) ((features)[18] & (1 << 15))

struct cacheinfo {
  hwloc_obj_cache_type_t type;
  unsigned level;
  unsigned nbthreads_sharing;
  unsigned cacheid;

  unsigned linesize;
  unsigned linepart;
  int inclusive;
  int ways;
  unsigned sets;
  unsigned long size;
};

struct procinfo {
  unsigned present;
  unsigned apicid;
#define PKG 0
#define CORE 1
#define NODE 2
#define UNIT 3
#define TILE 4
#define MODULE 5
#define DIE 6
#define COMPLEX 7
#define HWLOC_X86_PROCINFO_ID_NR 8
  unsigned ids[HWLOC_X86_PROCINFO_ID_NR];
  unsigned *otherids;
  unsigned levels;
  unsigned numcaches;
  struct cacheinfo *cache;
  char cpuvendor[13];
  char cpumodel[3*4*4+1];
  unsigned cpustepping;
  unsigned cpumodelnumber;
  unsigned cpufamilynumber;

  unsigned hybridcoretype;
  unsigned hybridnativemodel;
};

enum cpuid_type {
  intel,
  amd,
  zhaoxin,
  hygon,
  unknown
};


static void setup__amd_cache_legacy(struct procinfo *infos, unsigned level, hwloc_obj_cache_type_t type, unsigned nbthreads_sharing, unsigned cpuid)
{
  struct cacheinfo *cache, *tmpcaches;
  unsigned cachenum;
  unsigned long size = 0;

  if (level == 1)
    size = ((cpuid >> 24)) << 10;
  else if (level == 2)
    size = ((cpuid >> 16)) << 10;
  else if (level == 3)
    size = ((cpuid >> 18)) << 19;
  if (!size)
    return;

  tmpcaches = realloc(infos->cache, (infos->numcaches+1)*sizeof(*infos->cache));
  if (!tmpcaches)
    
    return;
  infos->cache = tmpcaches;
  cachenum = infos->numcaches++;

  cache = &infos->cache[cachenum];

  cache->type = type;
  cache->level = level;
  cache->nbthreads_sharing = nbthreads_sharing;
  cache->linesize = cpuid & 0xff;
  cache->linepart = 0;
  cache->inclusive = 0; 

  if (level == 1) {
    cache->ways = (cpuid >> 16) & 0xff;
    if (cache->ways == 0xff)
      
      cache->ways = -1;
  } else {
    static const unsigned ways_tab[] = { 0, 1, 2, 0, 4, 0, 8, 0, 16, 0, 32, 48, 64, 96, 128, -1 };
    unsigned ways = (cpuid >> 12) & 0xf;
    cache->ways = ways_tab[ways];
  }
  cache->size = size;
  cache->sets = 0;

  hwloc_debug("cache L%u t%u linesize %u ways %d size %luKB\n", cache->level, cache->nbthreads_sharing, cache->linesize, cache->ways, cache->size >> 10);
}


static void read_amd_caches_legacy(struct procinfo *infos, struct cpuiddump *src_cpuiddump, unsigned legacy_max_log_proc)
{
  unsigned eax, ebx, ecx, edx;

  eax = 0x80000005;
  cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
  setup__amd_cache_legacy(infos, 1, HWLOC_OBJ_CACHE_DATA, 1, ecx); 
  setup__amd_cache_legacy(infos, 1, HWLOC_OBJ_CACHE_INSTRUCTION, 1, edx); 

  eax = 0x80000006;
  cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
  if (ecx & 0xf000)
    
    setup__amd_cache_legacy(infos, 2, HWLOC_OBJ_CACHE_UNIFIED, 1, ecx); 
  if (edx & 0xf000)
    setup__amd_cache_legacy(infos, 3, HWLOC_OBJ_CACHE_UNIFIED, legacy_max_log_proc, edx); 
}


static void read_amd_caches_topoext(struct procinfo *infos, struct cpuiddump *src_cpuiddump)
{
  unsigned eax, ebx, ecx, edx;
  unsigned cachenum;
  struct cacheinfo *cache;

  
  assert(!infos->numcaches);

  for (cachenum = 0; cachenum<16 ; cachenum++) {
    eax = 0x8000001d;
    ecx = cachenum;
    cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
    if ((eax & 0x1f) == 0)
      break;
    infos->numcaches++;
  }

  cache = infos->cache = malloc(infos->numcaches * sizeof(*infos->cache));
  if (cache) {
    for (cachenum = 0; cachenum<16 ; cachenum++) {
      unsigned long linesize, linepart, ways, sets;
      eax = 0x8000001d;
      ecx = cachenum;
      cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);

      if ((eax & 0x1f) == 0)
	break;
      switch (eax & 0x1f) {
      case 1: cache->type = HWLOC_OBJ_CACHE_DATA; break;
      case 2: cache->type = HWLOC_OBJ_CACHE_INSTRUCTION; break;
      default: cache->type = HWLOC_OBJ_CACHE_UNIFIED; break;
      }

      cache->level = (eax >> 5) & 0x7;
      
      cache->nbthreads_sharing = ((eax >> 14) &  0xfff) + 1;

      cache->linesize = linesize = (ebx & 0xfff) + 1;
      cache->linepart = linepart = ((ebx >> 12) & 0x3ff) + 1;
      ways = ((ebx >> 22) & 0x3ff) + 1;

      if (eax & (1 << 9))
	
	cache->ways = -1;
      else
	cache->ways = ways;
      cache->sets = sets = ecx + 1;
      cache->size = linesize * linepart * ways * sets;
      cache->inclusive = edx & 0x2;

      hwloc_debug("cache %u L%u%c t%u linesize %lu linepart %lu ways %lu sets %lu, size %luKB\n",
		  cachenum, cache->level,
		  cache->type == HWLOC_OBJ_CACHE_DATA ? 'd' : cache->type == HWLOC_OBJ_CACHE_INSTRUCTION ? 'i' : 'u',
		  cache->nbthreads_sharing, linesize, linepart, ways, sets, cache->size >> 10);

      cache++;
    }
  } else {
    infos->numcaches = 0;
  }
}


static void read_intel_caches(struct hwloc_x86_backend_data_s *data, struct procinfo *infos, struct cpuiddump *src_cpuiddump)
{
  unsigned level;
  struct cacheinfo *tmpcaches;
  unsigned eax, ebx, ecx, edx;
  unsigned oldnumcaches = infos->numcaches; 
  unsigned cachenum;
  struct cacheinfo *cache;

  for (cachenum = 0; cachenum<16 ; cachenum++) {
    eax = 0x04;
    ecx = cachenum;
    cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);

    hwloc_debug("cache %u type %u\n", cachenum, eax & 0x1f);
    if ((eax & 0x1f) == 0)
      break;
    level = (eax >> 5) & 0x7;
    if (data->is_knl && level == 3)
      
      break;
    infos->numcaches++;
  }

  tmpcaches = realloc(infos->cache, infos->numcaches * sizeof(*infos->cache));
  if (!tmpcaches) {
    infos->numcaches = oldnumcaches;
  } else {
    infos->cache = tmpcaches;
    cache = &infos->cache[oldnumcaches];

    for (cachenum = 0; cachenum<16 ; cachenum++) {
      unsigned long linesize, linepart, ways, sets;
      eax = 0x04;
      ecx = cachenum;
      cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);

      if ((eax & 0x1f) == 0)
	break;
      level = (eax >> 5) & 0x7;
      if (data->is_knl && level == 3)
	
	break;
      switch (eax & 0x1f) {
      case 1: cache->type = HWLOC_OBJ_CACHE_DATA; break;
      case 2: cache->type = HWLOC_OBJ_CACHE_INSTRUCTION; break;
      default: cache->type = HWLOC_OBJ_CACHE_UNIFIED; break;
      }

      cache->level = level;
      cache->nbthreads_sharing = ((eax >> 14) & 0xfff) + 1;

      cache->linesize = linesize = (ebx & 0xfff) + 1;
      cache->linepart = linepart = ((ebx >> 12) & 0x3ff) + 1;
      ways = ((ebx >> 22) & 0x3ff) + 1;
      if (eax & (1 << 9))
        
        cache->ways = -1;
      else
        cache->ways = ways;
      cache->sets = sets = ecx + 1;
      cache->size = linesize * linepart * ways * sets;
      cache->inclusive = edx & 0x2;

      hwloc_debug("cache %u L%u%c t%u linesize %lu linepart %lu ways %lu sets %lu, size %luKB\n",
		  cachenum, cache->level,
		  cache->type == HWLOC_OBJ_CACHE_DATA ? 'd' : cache->type == HWLOC_OBJ_CACHE_INSTRUCTION ? 'i' : 'u',
		  cache->nbthreads_sharing, linesize, linepart, ways, sets, cache->size >> 10);
      cache++;
    }
  }
}


static void read_amd_cores_legacy(struct procinfo *infos, struct cpuiddump *src_cpuiddump)
{
  unsigned eax, ebx, ecx, edx;
  unsigned max_nbcores;
  unsigned max_nbthreads;
  unsigned coreidsize;
  unsigned logprocid;
  unsigned threadid __hwloc_attribute_unused;

  eax = 0x80000008;
  cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);

  coreidsize = (ecx >> 12) & 0xf;
  hwloc_debug("core ID size: %u\n", coreidsize);
  if (!coreidsize) {
    max_nbcores = (ecx & 0xff) + 1;
  } else
    max_nbcores = 1 << coreidsize;
  hwloc_debug("Thus max # of cores: %u\n", max_nbcores);

  
  max_nbthreads = 1 ;
  hwloc_debug("and max # of threads: %u\n", max_nbthreads);

  
  infos->ids[PKG] = infos->apicid / max_nbcores;
  logprocid = infos->apicid % max_nbcores;
  infos->ids[CORE] = logprocid / max_nbthreads;
  threadid = logprocid % max_nbthreads;
  hwloc_debug("this is thread %u of core %u\n", threadid, infos->ids[CORE]);
}


static void read_amd_cores_topoext(struct hwloc_x86_backend_data_s *data, struct procinfo *infos, unsigned long flags __hwloc_attribute_unused, struct cpuiddump *src_cpuiddump)
{
  unsigned apic_id, nodes_per_proc = 0;
  unsigned eax, ebx, ecx, edx;

  eax = 0x8000001e;
  cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
  infos->apicid = apic_id = eax;

    if (infos->cpufamilynumber == 0x16) {
      
      infos->ids[NODE] = 0;
      nodes_per_proc = 1;
    } else {
      
      infos->ids[NODE] = ecx & 0xff;
      nodes_per_proc = ((ecx >> 8) & 7) + 1;
    }
    if ((infos->cpufamilynumber == 0x15 && nodes_per_proc > 2)
	|| ((infos->cpufamilynumber == 0x17 || infos->cpufamilynumber == 0x18) && nodes_per_proc > 4)
        || (infos->cpufamilynumber == 0x19 && nodes_per_proc > 1)) {
      hwloc_debug("warning: undefined nodes_per_proc value %u, assuming it means %u\n", nodes_per_proc, nodes_per_proc);
    }

  if (infos->cpufamilynumber <= 0x16) { 
    unsigned cores_per_unit;
    
    infos->ids[UNIT] = ebx & 0xff;
    data->found_unit_ids = 1;
    cores_per_unit = ((ebx >> 8) & 0xff) + 1;
    hwloc_debug("topoext %08x, %u nodes, node %u, %u cores in unit %u\n", apic_id, nodes_per_proc, infos->ids[NODE], cores_per_unit, infos->ids[UNIT]);
    
  } else {
    unsigned threads_per_core;
    infos->ids[CORE] = ebx & 0xff;
    threads_per_core = ((ebx >> 8) & 0xff) + 1;
    hwloc_debug("topoext %08x, %u nodes, node %u, %u threads in core %u\n", apic_id, nodes_per_proc, infos->ids[NODE], threads_per_core, infos->ids[CORE]);
  }
}


static void read_extended_topo(struct hwloc_x86_backend_data_s *data, struct procinfo *infos, unsigned leaf, enum cpuid_type cpuid_type __hwloc_attribute_unused, struct cpuiddump *src_cpuiddump)
{
  unsigned level, apic_nextshift, apic_type, apic_id = 0, apic_shift = 0, id;
  unsigned threadid __hwloc_attribute_unused = 0; 
  unsigned eax, ebx, ecx = 0, edx;
  int apic_packageshift = 0;

  for (level = 0; level<32 ; level++) {
    ecx = level;
    eax = leaf;
    cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
    
    if (!(ebx & 0xffff) || !(ecx & 0xff00))
      break;
    apic_packageshift = eax & 0x1f;
  }

  if (level) {
    infos->otherids = malloc(level * sizeof(*infos->otherids));
    if (infos->otherids) {
      infos->levels = level;
      for (level = 0; level<32 ; level++) {
	ecx = level;
	eax = leaf;
	cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
        if (!(ebx & 0xffff) || !(ecx & 0xff00))
          break;
	apic_nextshift = eax & 0x1f;
	apic_type = (ecx & 0xff00) >> 8;
	apic_id = edx;
	id = (apic_id >> apic_shift) & ((1 << (apic_packageshift - apic_shift)) - 1);
	hwloc_debug("x2APIC %08x %u: nextshift %u nextnumber %2u type %u id %2u\n",
                    apic_id,
                    level,
                    apic_nextshift,
                    ebx & 0xffff ,
                    apic_type,
                    id);
	infos->apicid = apic_id;
	infos->otherids[level] = UINT_MAX;
        switch (apic_type) {
        case 1:
          threadid = id;
          break;
        case 2:
          infos->ids[CORE] = id;
          break;
        case 3:
          if (leaf == 0x80000026) {
            data->found_complex_ids = 1;
            infos->ids[COMPLEX] = id;
          } else {
            data->found_module_ids = 1;
            infos->ids[MODULE] = id;
          }
          break;
        case 4:
          if (leaf == 0x80000026) {
            data->found_die_ids = 1;
            infos->ids[DIE] = id;
          } else {
            data->found_tile_ids = 1;
            infos->ids[TILE] = id;
          }
          break;
        case 5:
          if (leaf == 0x80000026) {
            goto unknown_type;
          } else {
            data->found_die_ids = 1;
            infos->ids[DIE] = id;
          }
          break;
        case 6:
          
          
        default:
        unknown_type:
          hwloc_debug("x2APIC %u: unknown type %u\n", level, apic_type);
          infos->otherids[level] = apic_id >> apic_shift;
          break;
        }
        apic_shift = apic_nextshift;
      }
      infos->apicid = apic_id;
      infos->ids[PKG] = apic_id >> apic_shift;
      hwloc_debug("x2APIC remainder: %u\n", infos->ids[PKG]);
      hwloc_debug("this is thread %u of core %u\n", threadid, infos->ids[CORE]);
    }
  }
}


static void look_proc(struct hwloc_backend *backend, struct procinfo *infos, unsigned long flags, unsigned highest_cpuid, unsigned highest_ext_cpuid, unsigned *features, enum cpuid_type cpuid_type, struct cpuiddump *src_cpuiddump)
{
  struct hwloc_x86_backend_data_s *data = backend->private_data;
  unsigned eax, ebx, ecx = 0, edx;
  unsigned cachenum;
  struct cacheinfo *cache;
  unsigned regs[4];
  unsigned legacy_max_log_proc; 
  unsigned legacy_log_proc_id;
  unsigned _model, _extendedmodel, _family, _extendedfamily;

  infos->present = 1;

  
  eax = 0x01;
  cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
  infos->apicid = ebx >> 24;
  if (edx & (1 << 28)) {
    legacy_max_log_proc = 1 << hwloc_flsl(((ebx >> 16) & 0xff) - 1);
  } else {
    hwloc_debug("HTT bit not set in CPUID 0x01.edx, assuming legacy_max_log_proc = 1\n");
    legacy_max_log_proc = 1;
  }

  hwloc_debug("APIC ID 0x%02x legacy_max_log_proc %u\n", infos->apicid, legacy_max_log_proc);
  infos->ids[PKG] = infos->apicid / legacy_max_log_proc;
  legacy_log_proc_id = infos->apicid % legacy_max_log_proc;
  hwloc_debug("phys %u legacy thread %u\n", infos->ids[PKG], legacy_log_proc_id);

  
  _model          = (eax>>4) & 0xf;
  _extendedmodel  = (eax>>16) & 0xf;
  _family         = (eax>>8) & 0xf;
  _extendedfamily = (eax>>20) & 0xff;
  if ((cpuid_type == intel || cpuid_type == amd || cpuid_type == hygon) && _family == 0xf) {
    infos->cpufamilynumber = _family + _extendedfamily;
  } else {
    infos->cpufamilynumber = _family;
  }
  if ((cpuid_type == intel && (_family == 0x6 || _family == 0xf))
      || ((cpuid_type == amd || cpuid_type == hygon) && _family == 0xf)
      || (cpuid_type == zhaoxin && (_family == 0x6 || _family == 0x7))) {
    infos->cpumodelnumber = _model + (_extendedmodel << 4);
  } else {
    infos->cpumodelnumber = _model;
  }
  infos->cpustepping = eax & 0xf;

  if (cpuid_type == intel && infos->cpufamilynumber == 0x6 &&
      (infos->cpumodelnumber == 0x57 || infos->cpumodelnumber == 0x85))
    data->is_knl = 1; 

  
  memset(regs, 0, sizeof(regs));
  regs[0] = 0;
  cpuid_or_from_dump(&regs[0], &regs[1], &regs[3], &regs[2], src_cpuiddump);
  memcpy(infos->cpuvendor, regs+1, 4*3);
  

  
  if (highest_ext_cpuid >= 0x80000004) {
    memset(regs, 0, sizeof(regs));
    regs[0] = 0x80000002;
    cpuid_or_from_dump(&regs[0], &regs[1], &regs[2], &regs[3], src_cpuiddump);
    memcpy(infos->cpumodel, regs, 4*4);
    regs[0] = 0x80000003;
    cpuid_or_from_dump(&regs[0], &regs[1], &regs[2], &regs[3], src_cpuiddump);
    memcpy(infos->cpumodel + 4*4, regs, 4*4);
    regs[0] = 0x80000004;
    cpuid_or_from_dump(&regs[0], &regs[1], &regs[2], &regs[3], src_cpuiddump);
    memcpy(infos->cpumodel + 4*4*2, regs, 4*4);
    
  }

  if ((cpuid_type != amd && cpuid_type != hygon) && highest_cpuid >= 0x04) {
    
    eax = 0x04;
    ecx = 0;
    cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
    if ((eax & 0x1f) != 0) {
      
      unsigned max_nbcores;
      unsigned max_nbthreads;
      unsigned threadid __hwloc_attribute_unused;
      hwloc_debug("Trying to get core/thread IDs from 0x04...\n");
      max_nbcores = ((eax >> 26) & 0x3f) + 1;
      hwloc_debug("found %u cores max\n", max_nbcores);
      
      if (!max_nbcores) {
        hwloc_debug("cannot detect core/thread IDs from 0x04 without a valid max of cores\n");
      } else {
        max_nbthreads = legacy_max_log_proc / max_nbcores;
        hwloc_debug("found %u threads max\n", max_nbthreads);
        if (!max_nbthreads) {
          hwloc_debug("cannot detect core/thread IDs from 0x04 without a valid max of threads\n");
        } else {
          threadid = legacy_log_proc_id % max_nbthreads;
          infos->ids[CORE] = legacy_log_proc_id / max_nbthreads;
          hwloc_debug("this is thread %u of core %u\n", threadid, infos->ids[CORE]);
        }
      }
    }
  }

  if (highest_cpuid >= 0x1a && has_hybrid(features)) {
    
    eax = 0x1a;
    ecx = 0;
    cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
    infos->hybridcoretype = eax >> 24;
    infos->hybridnativemodel = eax & 0xffffff;
    data->is_hybrid = 1;
  }

  

  if (cpuid_type != intel && cpuid_type != zhaoxin && highest_ext_cpuid >= 0x80000008 && !has_x2apic(features)) {
    
    read_amd_cores_legacy(infos, src_cpuiddump);
  }

  if (cpuid_type != intel && cpuid_type != zhaoxin && has_topoext(features)) {
    
    read_amd_cores_topoext(data, infos, flags, src_cpuiddump);
  }

  if ((cpuid_type == amd) && highest_ext_cpuid >= 0x80000026) {
    
    read_extended_topo(data, infos, 0x80000026, cpuid_type, src_cpuiddump);

  } else if ((cpuid_type == intel || cpuid_type == zhaoxin) && highest_cpuid >= 0x1f) {
    
    read_extended_topo(data, infos, 0x1f, cpuid_type, src_cpuiddump);

  } else if ((cpuid_type == intel || cpuid_type == amd || cpuid_type == zhaoxin)
	     && highest_cpuid >= 0x0b && has_x2apic(features)) {
    
    read_extended_topo(data, infos, 0x0b, cpuid_type, src_cpuiddump);
  }

 if (backend->topology->want_some_cpu_caches) {
  

  infos->numcaches = 0;
  infos->cache = NULL;

  if (cpuid_type != intel && cpuid_type != zhaoxin && has_topoext(features)) {
    
    read_amd_caches_topoext(infos, src_cpuiddump);

  } else if (cpuid_type != intel && cpuid_type != zhaoxin && highest_ext_cpuid >= 0x80000006) {
    
    read_amd_caches_legacy(infos, src_cpuiddump, legacy_max_log_proc);
  }

  if ((cpuid_type != amd && cpuid_type != hygon) && highest_cpuid >= 0x04) {
    
    read_intel_caches(data, infos, src_cpuiddump);
  }

  
  for (cachenum = 0; cachenum < infos->numcaches; cachenum++) {
    cache = &infos->cache[cachenum];

    
    cache->cacheid = infos->apicid / cache->nbthreads_sharing;

    if (cpuid_type == intel) {
      
      unsigned bits = hwloc_flsl(cache->nbthreads_sharing-1);
      unsigned mask = ~((1U<<bits) - 1);
      cache->cacheid = infos->apicid & mask;

    } else if (cpuid_type == amd) {
      
      if (infos->cpufamilynumber >= 0x17 && cache->level == 3) {
	
        unsigned nbapics_sharing = cache->nbthreads_sharing;
        if (nbapics_sharing & (nbapics_sharing-1))
          
          nbapics_sharing = 1U<<(1+hwloc_ffsl(nbapics_sharing));

	cache->cacheid = infos->apicid / nbapics_sharing;

      } else if (infos->cpufamilynumber== 0x10 && infos->cpumodelnumber == 0x9
	  && cache->level == 3
	  && (cache->ways == -1 || (cache->ways % 2 == 0)) && cache->nbthreads_sharing >= 8) {
	
	if (cache->nbthreads_sharing == 16)
	  cache->nbthreads_sharing = 12; 
	cache->nbthreads_sharing /= 2;
	cache->size /= 2;
	if (cache->ways != -1)
	  cache->ways /= 2;
	
	cache->cacheid = (infos->apicid % legacy_max_log_proc) / cache->nbthreads_sharing 
	  + 2 * (infos->apicid / legacy_max_log_proc); 

      } else if (infos->cpufamilynumber == 0x15
		 && (infos->cpumodelnumber == 0x1  || infos->cpumodelnumber == 0x2 )
		 && cache->level == 3 && cache->nbthreads_sharing == 6) {
	
	cache->cacheid = (infos->apicid % legacy_max_log_proc) / cache->nbthreads_sharing 
	  + 2 * (infos->apicid / legacy_max_log_proc); 
      }
    } else if (cpuid_type == hygon) {
      if (infos->cpufamilynumber == 0x18
	  && cache->level == 3 && cache->nbthreads_sharing == 6) {
        
        cache->cacheid = infos->apicid / 8;
      }
    }
  }
 }

  if (hwloc_bitmap_isset(data->apicid_set, infos->apicid))
    data->apicid_unique = 0;
  else
    hwloc_bitmap_set(data->apicid_set, infos->apicid);
}

static void
hwloc_x86_add_cpuinfos(hwloc_obj_t obj, struct procinfo *info, int replace)
{
  char number[12];
  if (info->cpuvendor[0])
    hwloc__add_info_nodup(&obj->infos, &obj->infos_count, "CPUVendor", info->cpuvendor, replace);
  snprintf(number, sizeof(number), "%u", info->cpufamilynumber);
  hwloc__add_info_nodup(&obj->infos, &obj->infos_count, "CPUFamilyNumber", number, replace);
  snprintf(number, sizeof(number), "%u", info->cpumodelnumber);
  hwloc__add_info_nodup(&obj->infos, &obj->infos_count, "CPUModelNumber", number, replace);
  if (info->cpumodel[0]) {
    const char *c = info->cpumodel;
    while (*c == ' ')
      c++;
    hwloc__add_info_nodup(&obj->infos, &obj->infos_count, "CPUModel", c, replace);
  }
  snprintf(number, sizeof(number), "%u", info->cpustepping);
  hwloc__add_info_nodup(&obj->infos, &obj->infos_count, "CPUStepping", number, replace);
}

static void
hwloc_x86_add_groups(hwloc_topology_t topology,
		     struct procinfo *infos,
		     unsigned nbprocs,
		     hwloc_bitmap_t remaining_cpuset,
		     unsigned type,
		     const char *subtype,
		     unsigned kind,
		     int dont_merge)
{
  hwloc_bitmap_t obj_cpuset;
  hwloc_obj_t obj;
  unsigned i, j;

  while ((i = hwloc_bitmap_first(remaining_cpuset)) != (unsigned) -1) {
    unsigned packageid = infos[i].ids[PKG];
    unsigned id = infos[i].ids[type];

    if (id == (unsigned)-1) {
      hwloc_bitmap_clr(remaining_cpuset, i);
      continue;
    }

    obj_cpuset = hwloc_bitmap_alloc();
    for (j = i; j < nbprocs; j++) {
      if (infos[j].ids[type] == (unsigned) -1) {
	hwloc_bitmap_clr(remaining_cpuset, j);
	continue;
      }

      if (infos[j].ids[PKG] == packageid && infos[j].ids[type] == id) {
	hwloc_bitmap_set(obj_cpuset, j);
	hwloc_bitmap_clr(remaining_cpuset, j);
      }
    }

    obj = hwloc_alloc_setup_object(topology, HWLOC_OBJ_GROUP, id);
    obj->cpuset = obj_cpuset;
    obj->subtype = strdup(subtype);
    obj->attr->group.kind = kind;
    obj->attr->group.dont_merge = dont_merge;
    hwloc_debug_2args_bitmap("os %s %u has cpuset %s\n",
			     subtype, id, obj_cpuset);
    hwloc__insert_object_by_cpuset(topology, NULL, obj, "x86:group");
  }
}


static void summarize(struct hwloc_backend *backend, struct procinfo *infos, unsigned long flags)
{
  struct hwloc_topology *topology = backend->topology;
  struct hwloc_x86_backend_data_s *data = backend->private_data;
  unsigned nbprocs = data->nbprocs;
  hwloc_bitmap_t complete_cpuset = hwloc_bitmap_alloc();
  unsigned i, j, l, level;
  int one = -1;
  hwloc_bitmap_t remaining_cpuset;
  int gotnuma = 0;
  int fulldiscovery = (flags & HWLOC_X86_DISC_FLAG_FULL);

#ifdef HWLOC_DEBUG
  hwloc_debug("\nSummary of x86 CPUID topology:\n");
  for(i=0; i<nbprocs; i++) {
    hwloc_debug("PU %u present=%u apicid=%u on PKG %d CORE %d DIE %d NODE %d\n",
                i, infos[i].present, infos[i].apicid,
                infos[i].ids[PKG], infos[i].ids[CORE], infos[i].ids[DIE], infos[i].ids[NODE]);
  }
  hwloc_debug("\n");
#endif

  for (i = 0; i < nbprocs; i++)
    if (infos[i].present) {
      hwloc_bitmap_set(complete_cpuset, i);
      one = i;
    }

  if (one == -1) {
    hwloc_bitmap_free(complete_cpuset);
    return;
  }

  remaining_cpuset = hwloc_bitmap_alloc();

  

  if (hwloc_filter_check_keep_object_type(topology, HWLOC_OBJ_PACKAGE)) {
    
    hwloc_obj_t package;

    hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
    while ((i = hwloc_bitmap_first(remaining_cpuset)) != (unsigned) -1) {
      if (fulldiscovery) {
	unsigned packageid = infos[i].ids[PKG];
	hwloc_bitmap_t package_cpuset = hwloc_bitmap_alloc();

	for (j = i; j < nbprocs; j++) {
	  if (infos[j].ids[PKG] == packageid) {
	    hwloc_bitmap_set(package_cpuset, j);
	    hwloc_bitmap_clr(remaining_cpuset, j);
	  }
	}
	package = hwloc_alloc_setup_object(topology, HWLOC_OBJ_PACKAGE, packageid);
	package->cpuset = package_cpuset;

	hwloc_x86_add_cpuinfos(package, &infos[i], 0);

	hwloc_debug_1arg_bitmap("os package %u has cpuset %s\n",
				packageid, package_cpuset);
	hwloc__insert_object_by_cpuset(topology, NULL, package, "x86:package");

      } else {
	
	hwloc_bitmap_t set = hwloc_bitmap_alloc();
	hwloc_bitmap_set(set, i);
	package = hwloc_get_next_obj_covering_cpuset_by_type(topology, set, HWLOC_OBJ_PACKAGE, NULL);
	hwloc_bitmap_free(set);
	if (package) {
	  
	  hwloc_x86_add_cpuinfos(package, &infos[i], 1);
	  hwloc_bitmap_andnot(remaining_cpuset, remaining_cpuset, package->cpuset);
	} else {
	  
	  hwloc_x86_add_cpuinfos(hwloc_get_root_obj(topology), &infos[i], 1);
	  break;
	}
      }
    }
  }

  
  if (fulldiscovery && (flags & HWLOC_X86_DISC_FLAG_TOPOEXT_NUMANODES)) {
    hwloc_bitmap_t node_cpuset;
    hwloc_obj_t node;

    

    hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
    while ((i = hwloc_bitmap_first(remaining_cpuset)) != (unsigned) -1) {
      unsigned packageid = infos[i].ids[PKG];
      unsigned nodeid = infos[i].ids[NODE];

      if (nodeid == (unsigned)-1) {
        hwloc_bitmap_clr(remaining_cpuset, i);
	continue;
      }

      node_cpuset = hwloc_bitmap_alloc();
      for (j = i; j < nbprocs; j++) {
	if (infos[j].ids[NODE] == (unsigned) -1) {
	  hwloc_bitmap_clr(remaining_cpuset, j);
	  continue;
	}

        if (infos[j].ids[PKG] == packageid && infos[j].ids[NODE] == nodeid) {
          hwloc_bitmap_set(node_cpuset, j);
          hwloc_bitmap_clr(remaining_cpuset, j);
        }
      }
      node = hwloc_alloc_setup_object(topology, HWLOC_OBJ_NUMANODE, nodeid);
      node->cpuset = node_cpuset;
      node->nodeset = hwloc_bitmap_alloc();
      hwloc_bitmap_set(node->nodeset, nodeid);
      hwloc_debug_1arg_bitmap("os node %u has cpuset %s\n",
          nodeid, node_cpuset);
      hwloc__insert_object_by_cpuset(topology, NULL, node, "x86:numa");
      gotnuma++;
    }
  }

  if (hwloc_filter_check_keep_object_type(topology, HWLOC_OBJ_GROUP)) {
    if (fulldiscovery) {
      if (data->found_unit_ids) {
        
        hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
        hwloc_x86_add_groups(topology, infos, nbprocs, remaining_cpuset,
                             COMPLEX, "Complex",
                             HWLOC_GROUP_KIND_AMD_COMPLEX, 0);
      }
      if (data->found_unit_ids) {
        
        hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
        hwloc_x86_add_groups(topology, infos, nbprocs, remaining_cpuset,
                             UNIT, "Compute Unit",
                             HWLOC_GROUP_KIND_AMD_COMPUTE_UNIT, 0);
      }
      if (data->found_module_ids) {
        
        hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
        hwloc_x86_add_groups(topology, infos, nbprocs, remaining_cpuset,
                             MODULE, "Module",
                             HWLOC_GROUP_KIND_INTEL_MODULE, 0);
      }
      if (data->found_tile_ids) {
        
        hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
        hwloc_x86_add_groups(topology, infos, nbprocs, remaining_cpuset,
                             TILE, "Tile",
                             HWLOC_GROUP_KIND_INTEL_TILE, 0);
      }

      
      if (infos[one].otherids) {
	for (level = infos[one].levels-1; level <= infos[one].levels-1; level--) {
	  if (infos[one].otherids[level] != UINT_MAX) {
	    hwloc_bitmap_t unknown_cpuset;
	    hwloc_obj_t unknown_obj;

	    hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
	    while ((i = hwloc_bitmap_first(remaining_cpuset)) != (unsigned) -1) {
	      unsigned unknownid = infos[i].otherids[level];

	      unknown_cpuset = hwloc_bitmap_alloc();
	      for (j = i; j < nbprocs; j++) {
		if (infos[j].otherids[level] == unknownid) {
		  hwloc_bitmap_set(unknown_cpuset, j);
		  hwloc_bitmap_clr(remaining_cpuset, j);
		}
	      }
	      unknown_obj = hwloc_alloc_setup_object(topology, HWLOC_OBJ_GROUP, unknownid);
	      unknown_obj->cpuset = unknown_cpuset;
	      unknown_obj->attr->group.kind = HWLOC_GROUP_KIND_INTEL_EXTTOPOENUM_UNKNOWN;
	      unknown_obj->attr->group.subkind = level;
	      hwloc_debug_2args_bitmap("os unknown%u %u has cpuset %s\n",
				       level, unknownid, unknown_cpuset);
	      hwloc__insert_object_by_cpuset(topology, NULL, unknown_obj, "x86:group:unknown");
	    }
	  }
	}
      }
    }
  }

  if (data->found_die_ids
      && hwloc_filter_check_keep_object_type(topology, HWLOC_OBJ_DIE)) {
    
    if (fulldiscovery) {
      hwloc_bitmap_t die_cpuset;
      hwloc_obj_t die;

      hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
      while ((i = hwloc_bitmap_first(remaining_cpuset)) != (unsigned) -1) {
	unsigned packageid = infos[i].ids[PKG];
	unsigned dieid = infos[i].ids[DIE];

	if (dieid == (unsigned) -1) {
	  hwloc_bitmap_clr(remaining_cpuset, i);
	  continue;
	}

	die_cpuset = hwloc_bitmap_alloc();
	for (j = i; j < nbprocs; j++) {
	  if (infos[j].ids[DIE] == (unsigned) -1) {
	    hwloc_bitmap_clr(remaining_cpuset, j);
	    continue;
	  }

	  if (infos[j].ids[PKG] == packageid && infos[j].ids[DIE] == dieid) {
	    hwloc_bitmap_set(die_cpuset, j);
	    hwloc_bitmap_clr(remaining_cpuset, j);
	  }
	}
	die = hwloc_alloc_setup_object(topology, HWLOC_OBJ_DIE, dieid);
	die->cpuset = die_cpuset;
	hwloc_debug_1arg_bitmap("os die %u has cpuset %s\n",
				dieid, die_cpuset);
	hwloc__insert_object_by_cpuset(topology, NULL, die, "x86:die");
      }
    }
  }

  if (hwloc_filter_check_keep_object_type(topology, HWLOC_OBJ_CORE)) {
    
    if (fulldiscovery) {
      hwloc_bitmap_t core_cpuset;
      hwloc_obj_t core;

      hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
      while ((i = hwloc_bitmap_first(remaining_cpuset)) != (unsigned) -1) {
	unsigned packageid = infos[i].ids[PKG];
	unsigned nodeid = infos[i].ids[NODE];
	unsigned coreid = infos[i].ids[CORE];

	if (coreid == (unsigned) -1) {
	  hwloc_bitmap_clr(remaining_cpuset, i);
	  continue;
	}

	core_cpuset = hwloc_bitmap_alloc();
	for (j = i; j < nbprocs; j++) {
	  if (infos[j].ids[CORE] == (unsigned) -1) {
	    hwloc_bitmap_clr(remaining_cpuset, j);
	    continue;
	  }

	  if (infos[j].ids[PKG] == packageid && infos[j].ids[NODE] == nodeid && infos[j].ids[CORE] == coreid) {
	    hwloc_bitmap_set(core_cpuset, j);
	    hwloc_bitmap_clr(remaining_cpuset, j);
	  }
	}
	core = hwloc_alloc_setup_object(topology, HWLOC_OBJ_CORE, coreid);
	core->cpuset = core_cpuset;
	hwloc_debug_1arg_bitmap("os core %u has cpuset %s\n",
				coreid, core_cpuset);
	hwloc__insert_object_by_cpuset(topology, NULL, core, "x86:core");
      }
    }
  }

  
  if (fulldiscovery) {
    hwloc_debug("%s", "\n\n * CPU cpusets *\n\n");
    for (i=0; i<nbprocs; i++)
      if(infos[i].present) { 
       struct hwloc_obj *obj = hwloc_alloc_setup_object(topology, HWLOC_OBJ_PU, i);
       obj->cpuset = hwloc_bitmap_alloc();
       hwloc_bitmap_only(obj->cpuset, i);
       hwloc_debug_1arg_bitmap("PU %u has cpuset %s\n", i, obj->cpuset);
       hwloc__insert_object_by_cpuset(topology, NULL, obj, "x86:pu");
     }
  }

  
  
  level = 0;
  for (i = 0; i < nbprocs; i++)
    for (j = 0; j < infos[i].numcaches; j++)
      if (infos[i].cache[j].level > level)
        level = infos[i].cache[j].level;
  while (level > 0) {
    hwloc_obj_cache_type_t type;
    HWLOC_BUILD_ASSERT(HWLOC_OBJ_CACHE_DATA == HWLOC_OBJ_CACHE_UNIFIED+1);
    HWLOC_BUILD_ASSERT(HWLOC_OBJ_CACHE_INSTRUCTION == HWLOC_OBJ_CACHE_DATA+1);
    for (type = HWLOC_OBJ_CACHE_UNIFIED; type <= HWLOC_OBJ_CACHE_INSTRUCTION; type++) {
      
      hwloc_obj_type_t otype;
      hwloc_obj_t cache;

      otype = hwloc_cache_type_by_depth_type(level, type);
      if (otype == HWLOC_OBJ_TYPE_NONE)
	continue;
      if (!hwloc_filter_check_keep_object_type(topology, otype))
	continue;

      hwloc_bitmap_copy(remaining_cpuset, complete_cpuset);
      while ((i = hwloc_bitmap_first(remaining_cpuset)) != (unsigned) -1) {
	hwloc_bitmap_t puset;

	for (l = 0; l < infos[i].numcaches; l++) {
	  if (infos[i].cache[l].level == level && infos[i].cache[l].type == type)
	    break;
	}
	if (l == infos[i].numcaches) {
	  
	  hwloc_bitmap_clr(remaining_cpuset, i);
	  continue;
	}

	puset = hwloc_bitmap_alloc();
	hwloc_bitmap_set(puset, i);
	cache = hwloc_get_next_obj_covering_cpuset_by_type(topology, puset, otype, NULL);
	hwloc_bitmap_free(puset);

	if (cache) {
	  
	  if (!hwloc_obj_get_info_by_name(cache, "Inclusive"))
	    hwloc_obj_add_info(cache, "Inclusive", infos[i].cache[l].inclusive ? "1" : "0");
	  hwloc_bitmap_andnot(remaining_cpuset, remaining_cpuset, cache->cpuset);
	} else {
	  
	  hwloc_bitmap_t cache_cpuset;
	  unsigned packageid = infos[i].ids[PKG];
	  unsigned cacheid = infos[i].cache[l].cacheid;
	  
	  cache_cpuset = hwloc_bitmap_alloc();
	  for (j = i; j < nbprocs; j++) {
	    unsigned l2;
	    for (l2 = 0; l2 < infos[j].numcaches; l2++) {
	      if (infos[j].cache[l2].level == level && infos[j].cache[l2].type == type)
		break;
	    }
	    if (l2 == infos[j].numcaches) {
	      
	      hwloc_bitmap_clr(remaining_cpuset, j);
	      continue;
	    }
	    if (infos[j].ids[PKG] == packageid && infos[j].cache[l2].cacheid == cacheid) {
	      hwloc_bitmap_set(cache_cpuset, j);
	      hwloc_bitmap_clr(remaining_cpuset, j);
	    }
	  }
	  cache = hwloc_alloc_setup_object(topology, otype, HWLOC_UNKNOWN_INDEX);
          
	  cache->attr->cache.depth = level;
	  cache->attr->cache.size = infos[i].cache[l].size;
	  cache->attr->cache.linesize = infos[i].cache[l].linesize;
	  cache->attr->cache.associativity = infos[i].cache[l].ways;
	  cache->attr->cache.type = infos[i].cache[l].type;
	  cache->cpuset = cache_cpuset;
	  hwloc_obj_add_info(cache, "Inclusive", infos[i].cache[l].inclusive ? "1" : "0");
	  hwloc_debug_2args_bitmap("os L%u cache %u has cpuset %s\n",
				   level, cacheid, cache_cpuset);
	  hwloc__insert_object_by_cpuset(topology, NULL, cache, "x86:cache");
	}
      }
    }
    level--;
  }

  

  hwloc_bitmap_free(remaining_cpuset);
  hwloc_bitmap_free(complete_cpuset);

  if (gotnuma)
    topology->support.discovery->numa = 1;
}

static int
look_procs(struct hwloc_backend *backend, struct procinfo *infos, unsigned long flags,
	   unsigned highest_cpuid, unsigned highest_ext_cpuid, unsigned *features, enum cpuid_type cpuid_type,
	   int (*get_cpubind)(hwloc_topology_t topology, hwloc_cpuset_t set, int flags),
	   int (*set_cpubind)(hwloc_topology_t topology, hwloc_const_cpuset_t set, int flags),
           hwloc_bitmap_t restrict_set)
{
  struct hwloc_x86_backend_data_s *data = backend->private_data;
  struct hwloc_topology *topology = backend->topology;
  unsigned nbprocs = data->nbprocs;
  hwloc_bitmap_t orig_cpuset = NULL;
  hwloc_bitmap_t set = NULL;
  unsigned i;

  if (!data->src_cpuiddump_path) {
    orig_cpuset = hwloc_bitmap_alloc();
    if (get_cpubind(topology, orig_cpuset, HWLOC_CPUBIND_STRICT)) {
      hwloc_bitmap_free(orig_cpuset);
      return -1;
    }
    set = hwloc_bitmap_alloc();
  }

  for (i = 0; i < nbprocs; i++) {
    struct cpuiddump *src_cpuiddump = NULL;

    if (restrict_set && !hwloc_bitmap_isset(restrict_set, i)) {
      
      continue;
    }

    if (data->src_cpuiddump_path) {
      src_cpuiddump = cpuiddump_read(data->src_cpuiddump_path, i);
      if (!src_cpuiddump)
	continue;
    } else {
      hwloc_bitmap_only(set, i);
      hwloc_debug("binding to CPU%u\n", i);
      if (set_cpubind(topology, set, HWLOC_CPUBIND_STRICT)) {
	hwloc_debug("could not bind to CPU%u: %s\n", i, strerror(errno));
	continue;
      }
    }

    look_proc(backend, &infos[i], flags, highest_cpuid, highest_ext_cpuid, features, cpuid_type, src_cpuiddump);

    if (data->src_cpuiddump_path) {
      cpuiddump_free(src_cpuiddump);
    }
  }

  if (!data->src_cpuiddump_path) {
    set_cpubind(topology, orig_cpuset, 0);
    hwloc_bitmap_free(set);
    hwloc_bitmap_free(orig_cpuset);
  }

  if (data->apicid_unique) {
    summarize(backend, infos, flags);

    if (data->is_hybrid
        && !(topology->flags & HWLOC_TOPOLOGY_FLAG_NO_CPUKINDS)) {
      
      if (cpuid_type == intel) {
        
        hwloc_bitmap_t atomset = hwloc_bitmap_alloc();
        hwloc_bitmap_t coreset = hwloc_bitmap_alloc();
        for(i=0; i<nbprocs; i++) {
          if (infos[i].hybridcoretype == 0x20)
            hwloc_bitmap_set(atomset, i);
          else if (infos[i].hybridcoretype == 0x40)
            hwloc_bitmap_set(coreset, i);
        }
        
        if (!hwloc_bitmap_iszero(atomset)) {
          struct hwloc_info_s infoattr;
          infoattr.name = (char *) "CoreType";
          infoattr.value = (char *) "IntelAtom";
          hwloc_internal_cpukinds_register(topology, atomset, HWLOC_CPUKIND_EFFICIENCY_UNKNOWN, &infoattr, 1, 0);
          
        } else {
          hwloc_bitmap_free(atomset);
        }
        
        if (!hwloc_bitmap_iszero(coreset)) {
          struct hwloc_info_s infoattr;
          infoattr.name = (char *) "CoreType";
          infoattr.value = (char *) "IntelCore";
          hwloc_internal_cpukinds_register(topology, coreset, HWLOC_CPUKIND_EFFICIENCY_UNKNOWN, &infoattr, 1, 0);
          
        } else {
          hwloc_bitmap_free(coreset);
        }
      }
    }
  } else {
    hwloc_debug("x86 APIC IDs aren't unique, x86 discovery ignored.\n");
    
  }
  return 0;
}

#if defined HWLOC_FREEBSD_SYS && defined HAVE_CPUSET_SETID
#include <sys/param.h>
#include <sys/cpuset.h>
typedef cpusetid_t hwloc_x86_os_state_t;
static void hwloc_x86_os_state_save(hwloc_x86_os_state_t *state, struct cpuiddump *src_cpuiddump)
{
  if (!src_cpuiddump) {
    
    cpuset_getid(CPU_LEVEL_CPUSET, CPU_WHICH_PID, -1, state);
    cpuset_setid(CPU_WHICH_PID, -1, 0);
  }
}
static void hwloc_x86_os_state_restore(hwloc_x86_os_state_t *state, struct cpuiddump *src_cpuiddump)
{
  if (!src_cpuiddump) {
    
    cpuset_setid(CPU_WHICH_PID, -1, *state);
  }
}
#else 
typedef void * hwloc_x86_os_state_t;
static void hwloc_x86_os_state_save(hwloc_x86_os_state_t *state __hwloc_attribute_unused, struct cpuiddump *src_cpuiddump __hwloc_attribute_unused) { }
static void hwloc_x86_os_state_restore(hwloc_x86_os_state_t *state __hwloc_attribute_unused, struct cpuiddump *src_cpuiddump __hwloc_attribute_unused) { }
#endif 


#define INTEL_EBX ('G' | ('e'<<8) | ('n'<<16) | ('u'<<24))
#define INTEL_EDX ('i' | ('n'<<8) | ('e'<<16) | ('I'<<24))
#define INTEL_ECX ('n' | ('t'<<8) | ('e'<<16) | ('l'<<24))


#define AMD_EBX ('A' | ('u'<<8) | ('t'<<16) | ('h'<<24))
#define AMD_EDX ('e' | ('n'<<8) | ('t'<<16) | ('i'<<24))
#define AMD_ECX ('c' | ('A'<<8) | ('M'<<16) | ('D'<<24))


#define HYGON_EBX ('H' | ('y'<<8) | ('g'<<16) | ('o'<<24))
#define HYGON_EDX ('n' | ('G'<<8) | ('e'<<16) | ('n'<<24))
#define HYGON_ECX ('u' | ('i'<<8) | ('n'<<16) | ('e'<<24))


#define ZX_EBX ('C' | ('e'<<8) | ('n'<<16) | ('t'<<24))
#define ZX_EDX ('a' | ('u'<<8) | ('r'<<16) | ('H'<<24))
#define ZX_ECX ('a' | ('u'<<8) | ('l'<<16) | ('s'<<24))

#define SH_EBX (' ' | (' '<<8) | ('S'<<16) | ('h'<<24))
#define SH_EDX ('a' | ('n'<<8) | ('g'<<16) | ('h'<<24))
#define SH_ECX ('a' | ('i'<<8) | (' '<<16) | (' '<<24))


static int fake_get_cpubind(hwloc_topology_t topology __hwloc_attribute_unused,
			    hwloc_cpuset_t set __hwloc_attribute_unused,
			    int flags __hwloc_attribute_unused)
{
  return 0;
}
static int fake_set_cpubind(hwloc_topology_t topology __hwloc_attribute_unused,
			    hwloc_const_cpuset_t set __hwloc_attribute_unused,
			    int flags __hwloc_attribute_unused)
{
  return 0;
}

static
int hwloc_look_x86(struct hwloc_backend *backend, unsigned long flags)
{
  struct hwloc_x86_backend_data_s *data = backend->private_data;
  struct hwloc_topology *topology = backend->topology;
  unsigned nbprocs = data->nbprocs;
  unsigned eax, ebx, ecx = 0, edx;
  unsigned i;
  unsigned highest_cpuid;
  unsigned highest_ext_cpuid;
  
  unsigned features[19] = { 0 };
  struct procinfo *infos = NULL;
  enum cpuid_type cpuid_type = unknown;
  hwloc_x86_os_state_t os_state;
  struct hwloc_binding_hooks hooks;
  struct hwloc_topology_support support;
  struct hwloc_topology_membind_support memsupport __hwloc_attribute_unused;
  int (*get_cpubind)(hwloc_topology_t topology, hwloc_cpuset_t set, int flags) = NULL;
  int (*set_cpubind)(hwloc_topology_t topology, hwloc_const_cpuset_t set, int flags) = NULL;
  hwloc_bitmap_t restrict_set = NULL;
  struct cpuiddump *src_cpuiddump = NULL;
  int ret = -1;

  
  memset(&hooks, 0, sizeof(hooks));
  support.membind = &memsupport;
  
  hwloc_set_native_binding_hooks(&hooks, &support);
  

  if (data->src_cpuiddump_path) {
    
    src_cpuiddump = cpuiddump_read(data->src_cpuiddump_path, 0);
    if (!src_cpuiddump)
      goto out;

  } else {
    

    if (hooks.get_thisthread_cpubind && hooks.set_thisthread_cpubind) {
      get_cpubind = hooks.get_thisthread_cpubind;
      set_cpubind = hooks.set_thisthread_cpubind;
    } else if (hooks.get_thisproc_cpubind && hooks.set_thisproc_cpubind) {
      
      get_cpubind = hooks.get_thisproc_cpubind;
      set_cpubind = hooks.set_thisproc_cpubind;
    } else {
      
      if (nbprocs > 1)
	goto out;
      get_cpubind = fake_get_cpubind;
      set_cpubind = fake_set_cpubind;
    }
  }

  if (topology->flags & HWLOC_TOPOLOGY_FLAG_RESTRICT_TO_CPUBINDING) {
    restrict_set = hwloc_bitmap_alloc();
    if (!restrict_set)
      goto out;
    if (hooks.get_thisproc_cpubind)
      hooks.get_thisproc_cpubind(topology, restrict_set, 0);
    else if (hooks.get_thisthread_cpubind)
      hooks.get_thisthread_cpubind(topology, restrict_set, 0);
    if (hwloc_bitmap_iszero(restrict_set)) {
      hwloc_bitmap_free(restrict_set);
      restrict_set = NULL;
    }
  }

  if (!src_cpuiddump && !hwloc_have_x86_cpuid())
    goto out;

  infos = calloc(nbprocs, sizeof(struct procinfo));
  if (NULL == infos)
    goto out;
  for (i = 0; i < nbprocs; i++) {
    infos[i].ids[PKG] = (unsigned) -1;
    infos[i].ids[CORE] = (unsigned) -1;
    infos[i].ids[NODE] = (unsigned) -1;
    infos[i].ids[UNIT] = (unsigned) -1;
    infos[i].ids[TILE] = (unsigned) -1;
    infos[i].ids[MODULE] = (unsigned) -1;
    infos[i].ids[DIE] = (unsigned) -1;
  }

  eax = 0x00;
  cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
  highest_cpuid = eax;
  if (ebx == INTEL_EBX && ecx == INTEL_ECX && edx == INTEL_EDX)
    cpuid_type = intel;
  else if (ebx == AMD_EBX && ecx == AMD_ECX && edx == AMD_EDX)
    cpuid_type = amd;
  else if ((ebx == ZX_EBX && ecx == ZX_ECX && edx == ZX_EDX)
	   || (ebx == SH_EBX && ecx == SH_ECX && edx == SH_EDX))
    cpuid_type = zhaoxin;
  else if (ebx == HYGON_EBX && ecx == HYGON_ECX && edx == HYGON_EDX)
    cpuid_type = hygon;

  hwloc_debug("highest cpuid %x, cpuid type %u\n", highest_cpuid, cpuid_type);
  if (highest_cpuid < 0x01) {
      goto out_with_infos;
  }

  eax = 0x01;
  cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
  features[0] = edx;
  features[4] = ecx;

  eax = 0x80000000;
  cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
  highest_ext_cpuid = eax;

  hwloc_debug("highest extended cpuid %x\n", highest_ext_cpuid);

  if (highest_cpuid >= 0x7) {
    eax = 0x7;
    ecx = 0;
    cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
    features[9] = ebx;
    features[16] = ecx;
    features[18] = edx;
  }

  if (cpuid_type != intel && highest_ext_cpuid >= 0x80000001) {
    eax = 0x80000001;
    cpuid_or_from_dump(&eax, &ebx, &ecx, &edx, src_cpuiddump);
    features[1] = edx;
    features[6] = ecx;
  }

  hwloc_x86_os_state_save(&os_state, src_cpuiddump);

  ret = look_procs(backend, infos, flags,
		   highest_cpuid, highest_ext_cpuid, features, cpuid_type,
		   get_cpubind, set_cpubind, restrict_set);
  if (!ret)
    
    goto out_with_os_state;

  if (nbprocs == 1) {
    
    look_proc(backend, &infos[0], flags, highest_cpuid, highest_ext_cpuid, features, cpuid_type, src_cpuiddump);
    summarize(backend, infos, flags);
    ret = 0;
  }

out_with_os_state:
  hwloc_x86_os_state_restore(&os_state, src_cpuiddump);

out_with_infos:
  if (NULL != infos) {
    for (i = 0; i < nbprocs; i++) {
      free(infos[i].cache);
      free(infos[i].otherids);
    }
    free(infos);
  }

out:
  hwloc_bitmap_free(restrict_set);
  if (src_cpuiddump)
    cpuiddump_free(src_cpuiddump);
  return ret;
}

static int
hwloc_x86_discover(struct hwloc_backend *backend, struct hwloc_disc_status *dstatus)
{
  struct hwloc_x86_backend_data_s *data = backend->private_data;
  struct hwloc_topology *topology = backend->topology;
  unsigned long flags = 0;
  int alreadypus = 0;
  int ret;

  assert(dstatus->phase == HWLOC_DISC_PHASE_CPU);

  if (topology->flags & HWLOC_TOPOLOGY_FLAG_DONT_CHANGE_BINDING) {
    
    return 0;
  }

  if (getenv("HWLOC_X86_TOPOEXT_NUMANODES")) {
    flags |= HWLOC_X86_DISC_FLAG_TOPOEXT_NUMANODES;
  }

#if HAVE_DECL_RUNNING_ON_VALGRIND
  if (RUNNING_ON_VALGRIND && !data->src_cpuiddump_path) {
    fprintf(stderr, "hwloc x86 backend cannot work under Valgrind, disabling.\n"
	    "May be reenabled by dumping CPUIDs with hwloc-gather-cpuid\n"
	    "and reloading them under Valgrind with HWLOC_CPUID_PATH.\n");
    return 0;
  }
#endif

  if (data->src_cpuiddump_path) {
    assert(data->nbprocs > 0); 
    topology->support.discovery->pu = 1;
  } else {
    int nbprocs = hwloc_fallback_nbprocessors(HWLOC_FALLBACK_NBPROCESSORS_INCLUDE_OFFLINE);
    if (nbprocs >= 1)
      topology->support.discovery->pu = 1;
    else
      nbprocs = 1;
    data->nbprocs = (unsigned) nbprocs;
  }

  if (topology->levels[0][0]->cpuset) {
    
    hwloc_topology_reconnect(topology, 0);
    if (topology->nb_levels == 2 && topology->level_nbobjects[1] == data->nbprocs) {
      
      alreadypus = 1;
      goto fulldiscovery;
    }

    
    ret = hwloc_look_x86(backend, flags);
    if (ret)
      hwloc_obj_add_info(topology->levels[0][0], "Backend", "x86");
    return 0;
  } else {
    
    hwloc_alloc_root_sets(topology->levels[0][0]);
  }

fulldiscovery:
  if (hwloc_look_x86(backend, flags | HWLOC_X86_DISC_FLAG_FULL) < 0) {
    
    if (!alreadypus)
      hwloc_setup_pu_level(topology, data->nbprocs);
  }

  hwloc_obj_add_info(topology->levels[0][0], "Backend", "x86");

  if (!data->src_cpuiddump_path) { 
#ifdef HAVE_UNAME
    hwloc_add_uname_info(topology, NULL); 
#else
    
#ifdef HWLOC_X86_64_ARCH
    hwloc_obj_add_info(topology->levels[0][0], "Architecture", "x86_64");
#else
    hwloc_obj_add_info(topology->levels[0][0], "Architecture", "x86");
#endif
#endif
  }

  return 1;
}

static int
hwloc_x86_check_cpuiddump_input(const char *src_cpuiddump_path, hwloc_bitmap_t set)
{

#if !(defined HWLOC_WIN_SYS && !defined __MINGW32__ && !defined __CYGWIN__) 
  struct dirent *dirent;
  DIR *dir;
  char *path;
  FILE *file;
  char line [32];

  dir = opendir(src_cpuiddump_path);
  if (!dir) 
    return -1;

  path = malloc(strlen(src_cpuiddump_path) + strlen("/hwloc-cpuid-info") + 1);
  if (!path)
    goto out_with_dir;
  sprintf(path, "%s/hwloc-cpuid-info", src_cpuiddump_path);
  file = fopen(path, "r");
  if (!file) {
    fprintf(stderr, "hwloc/x86: Couldn't open dumped cpuid summary %s\n", path);
    goto out_with_path;
  }
  if (!fgets(line, sizeof(line), file)) {
    fprintf(stderr, "hwloc/x86: Found read dumped cpuid summary in %s\n", path);
    fclose(file);
    goto out_with_path;
  }
  fclose(file);
  if (strncmp(line, "Architecture: x86", 17)) {
    fprintf(stderr, "hwloc/x86: Found non-x86 dumped cpuid summary in %s: %s\n", path, line);
    goto out_with_path;
  }
  free(path);

  while ((dirent = readdir(dir)) != NULL) {
    if (!strncmp(dirent->d_name, "pu", 2)) {
      char *end;
      unsigned long idx = strtoul(dirent->d_name+2, &end, 10);
      if (!*end)
	hwloc_bitmap_set(set, idx);
      else
	fprintf(stderr, "hwloc/x86: Ignoring invalid dirent `%s' in dumped cpuid directory `%s'\n",
		dirent->d_name, src_cpuiddump_path);
    }
  }
  closedir(dir);

  if (hwloc_bitmap_iszero(set)) {
    fprintf(stderr, "hwloc/x86: Did not find any valid pu%%u entry in dumped cpuid directory `%s'\n",
	    src_cpuiddump_path);
    return -1;
  } else if (hwloc_bitmap_last(set) != hwloc_bitmap_weight(set) - 1) {
    
    fprintf(stderr, "hwloc/x86: Found non-contigous pu%%u range in dumped cpuid directory `%s'\n",
	    src_cpuiddump_path);
    return -1;
  }

  return 0;

 out_with_path:
  free(path);
 out_with_dir:
  closedir(dir);
#endif 
  return -1;
}

static void
hwloc_x86_backend_disable(struct hwloc_backend *backend)
{
  struct hwloc_x86_backend_data_s *data = backend->private_data;
  hwloc_bitmap_free(data->apicid_set);
  free(data->src_cpuiddump_path);
  free(data);
}

static struct hwloc_backend *
hwloc_x86_component_instantiate(struct hwloc_topology *topology,
				struct hwloc_disc_component *component,
				unsigned excluded_phases __hwloc_attribute_unused,
				const void *_data1 __hwloc_attribute_unused,
				const void *_data2 __hwloc_attribute_unused,
				const void *_data3 __hwloc_attribute_unused)
{
  struct hwloc_backend *backend;
  struct hwloc_x86_backend_data_s *data;
  const char *src_cpuiddump_path;

  backend = hwloc_backend_alloc(topology, component);
  if (!backend)
    goto out;

  data = malloc(sizeof(*data));
  if (!data) {
    errno = ENOMEM;
    goto out_with_backend;
  }

  backend->private_data = data;
  backend->discover = hwloc_x86_discover;
  backend->disable = hwloc_x86_backend_disable;

  
  data->is_knl = 0;
  data->is_hybrid = 0;
  data->apicid_set = hwloc_bitmap_alloc();
  data->apicid_unique = 1;
  data->src_cpuiddump_path = NULL;
  data->found_die_ids = 0;
  data->found_complex_ids = 0;
  data->found_unit_ids = 0;
  data->found_module_ids = 0;
  data->found_tile_ids = 0;

  src_cpuiddump_path = getenv("HWLOC_CPUID_PATH");
  if (src_cpuiddump_path) {
    hwloc_bitmap_t set = hwloc_bitmap_alloc();
    if (!hwloc_x86_check_cpuiddump_input(src_cpuiddump_path, set)) {
      backend->is_thissystem = 0;
      data->src_cpuiddump_path = strdup(src_cpuiddump_path);
      assert(!hwloc_bitmap_iszero(set)); 
      data->nbprocs = hwloc_bitmap_weight(set);
    } else {
      fprintf(stderr, "hwloc/x86: Ignoring dumped cpuid directory.\n");
    }
    hwloc_bitmap_free(set);
  }

  return backend;

 out_with_backend:
  free(backend);
 out:
  return NULL;
}

static struct hwloc_disc_component hwloc_x86_disc_component = {
  "x86",
  HWLOC_DISC_PHASE_CPU,
  HWLOC_DISC_PHASE_GLOBAL,
  hwloc_x86_component_instantiate,
  45, 
  1,
  NULL
};

const struct hwloc_component hwloc_x86_component = {
  HWLOC_COMPONENT_ABI,
  NULL, NULL,
  HWLOC_COMPONENT_TYPE_DISC,
  0,
  &hwloc_x86_disc_component
};

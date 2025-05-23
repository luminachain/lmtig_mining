

#include "private/autogen/config.h"
#include "hwloc.h"
#include "private/private.h"
#include "private/misc.h"
#include "private/debug.h"

#include <limits.h>
#include <assert.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

struct hwloc_synthetic_attr_s {
  hwloc_obj_type_t type;
  unsigned depth; 
  hwloc_obj_cache_type_t cachetype; 
  hwloc_uint64_t memorysize; 
  hwloc_uint64_t memorysidecachesize; 
};

struct hwloc_synthetic_indexes_s {
  
  const char *string;
  unsigned long string_length;
  
  unsigned *array;

  
  unsigned next; 
};

struct hwloc_synthetic_level_data_s {
  unsigned arity;
  unsigned long totalwidth;

  struct hwloc_synthetic_attr_s attr;
  struct hwloc_synthetic_indexes_s indexes;

  struct hwloc_synthetic_attached_s {
    struct hwloc_synthetic_attr_s attr;

    struct hwloc_synthetic_attached_s *next;
  } *attached;
};

struct hwloc_synthetic_backend_data_s {
  
  char *string;

  unsigned long numa_attached_nr;
  struct hwloc_synthetic_indexes_s numa_attached_indexes;

#define HWLOC_SYNTHETIC_MAX_DEPTH 128
  struct hwloc_synthetic_level_data_s level[HWLOC_SYNTHETIC_MAX_DEPTH];
};

struct hwloc_synthetic_intlv_loop_s {
  unsigned step;
  unsigned nb;
  unsigned level_depth;
};

static void
hwloc_synthetic_process_indexes(struct hwloc_synthetic_backend_data_s *data,
				struct hwloc_synthetic_indexes_s *indexes,
				unsigned long total,
				int verbose)
{
  const char *attr = indexes->string;
  unsigned long length = indexes->string_length;
  unsigned *array = NULL;
  size_t i;

  if (!attr)
    return;

  array = calloc(total, sizeof(*array));
  if (!array) {
    if (verbose)
      fprintf(stderr, "Failed to allocate synthetic index array of size %lu\n", total);
    goto out;
  }

  i = strspn(attr, "0123456789,");
  if (i == length) {
    

    for(i=0; i<total; i++) {
      const char *next;
      unsigned idx = strtoul(attr, (char **) &next, 10);
      if (next == attr) {
	if (verbose)
	  fprintf(stderr, "Failed to read synthetic index #%lu at '%s'\n", (unsigned long) i, attr);
	goto out_with_array;
      }

      array[i] = idx;
      if (i != total-1) {
	if (*next != ',') {
	  if (verbose)
	    fprintf(stderr, "Missing comma after synthetic index #%lu at '%s'\n", (unsigned long) i, attr);
	  goto out_with_array;
	}
	attr = next+1;
      } else {
	attr = next;
      }
    }
    indexes->array = array;

  } else {
    
    unsigned nr_loops = 1, cur_loop;
    unsigned minstep = total;
    unsigned long nbs = 1;
    unsigned j, mul;
    const char *tmp;
    struct hwloc_synthetic_intlv_loop_s *loops;

    tmp = attr;
    while (tmp) {
      tmp = strchr(tmp, ':');
      if (!tmp || tmp >= attr+length)
	break;
      nr_loops++;
      tmp++;
    }

    
    loops = malloc((nr_loops+1) * sizeof(*loops));
    if (!loops)
      goto out_with_array;

    if (*attr >= '0' && *attr <= '9') {
      
      unsigned step, nb;

      tmp = attr;
      cur_loop = 0;
      while (tmp) {
	char *tmp2, *tmp3;
	step = (unsigned) strtol(tmp, &tmp2, 0);
	if (tmp2 == tmp || *tmp2 != '*') {
	  if (verbose)
	    fprintf(stderr, "Failed to read synthetic index interleaving loop '%s' without number before '*'\n", tmp);
	  free(loops);
	  goto out_with_array;
	}
	if (!step) {
	  if (verbose)
	    fprintf(stderr, "Invalid interleaving loop with step 0 at '%s'\n", tmp);
	  free(loops);
	  goto out_with_array;
	}
	tmp2++;
	nb = (unsigned) strtol(tmp2, &tmp3, 0);
	if (tmp3 == tmp2 || (*tmp3 && *tmp3 != ':' && *tmp3 != ')' && *tmp3 != ' ')) {
	  if (verbose)
	    fprintf(stderr, "Failed to read synthetic index interleaving loop '%s' without number between '*' and ':'\n", tmp);
	  free(loops);
	  goto out_with_array;
	}
	if (!nb) {
	  if (verbose)
	    fprintf(stderr, "Invalid interleaving loop with number 0 at '%s'\n", tmp2);
	  free(loops);
	  goto out_with_array;
	}
	loops[cur_loop].step = step;
	loops[cur_loop].nb = nb;
	if (step < minstep)
	  minstep = step;
	nbs *= nb;
	cur_loop++;
	if (*tmp3 == ')' || *tmp3 == ' ')
	  break;
	tmp = (const char*) (tmp3+1);
      }

    } else {
      
      hwloc_obj_type_t type;
      union hwloc_obj_attr_u attrs;
      int err;

      
      tmp = attr;
      cur_loop = 0;
      while (tmp) {
	err = hwloc_type_sscanf(tmp, &type, &attrs, sizeof(attrs));
	if (err < 0) {
	  if (verbose)
	    fprintf(stderr, "Failed to read synthetic index interleaving loop type '%s'\n", tmp);
	  free(loops);
	  goto out_with_array;
	}
	if (type == HWLOC_OBJ_MISC || type == HWLOC_OBJ_BRIDGE || type == HWLOC_OBJ_PCI_DEVICE || type == HWLOC_OBJ_OS_DEVICE) {
	  if (verbose)
	    fprintf(stderr, "Misc object type disallowed in synthetic index interleaving loop type '%s'\n", tmp);
	  free(loops);
	  goto out_with_array;
	}
	for(i=0; ; i++) {
	  if (!data->level[i].arity) {
	    loops[cur_loop].level_depth = (unsigned)-1;
	    break;
	  }
	  if (type != data->level[i].attr.type)
	    continue;
	  if (type == HWLOC_OBJ_GROUP
	      && attrs.group.depth != (unsigned) -1
	      && attrs.group.depth != data->level[i].attr.depth)
	    continue;
	  loops[cur_loop].level_depth = (unsigned)i;
	  break;
	}
	if (loops[cur_loop].level_depth == (unsigned)-1) {
	  if (verbose)
	    fprintf(stderr, "Failed to find level for synthetic index interleaving loop type '%s'\n",
		    tmp);
	  free(loops);
	  goto out_with_array;
	}
	tmp = strchr(tmp, ':');
	if (!tmp || tmp > attr+length)
	  break;
	tmp++;
	cur_loop++;
      }

      
      for(cur_loop=0; cur_loop<nr_loops; cur_loop++) {
	unsigned mydepth = loops[cur_loop].level_depth;
	unsigned prevdepth = 0;
	unsigned step, nb;
	for(i=0; i<nr_loops; i++) {
	  if (loops[i].level_depth == mydepth && i != cur_loop) {
	    if (verbose)
	      fprintf(stderr, "Invalid duplicate interleaving loop type in synthetic index '%s'\n", attr);
	    free(loops);
	    goto out_with_array;
	  }
	  if (loops[i].level_depth < mydepth
	      && loops[i].level_depth > prevdepth)
	    prevdepth = loops[i].level_depth;
	}
	step = total / data->level[mydepth].totalwidth; 
	nb = data->level[mydepth].totalwidth / data->level[prevdepth].totalwidth; 

	loops[cur_loop].step = step;
	loops[cur_loop].nb = nb;
	assert(nb);
	assert(step);
	if (step < minstep)
	  minstep = step;
	nbs *= nb;
      }
    }
    assert(nbs);

    if (nbs != total) {
      
      if (minstep == total/nbs) {
	loops[nr_loops].step = 1;
	loops[nr_loops].nb = total/nbs;
	nr_loops++;
      } else {
	if (verbose)
	  fprintf(stderr, "Invalid index interleaving total width %lu instead of %lu\n", nbs, total);
	free(loops);
	goto out_with_array;
      }
    }

    
    mul = 1;
    for(i=0; i<nr_loops; i++) {
      unsigned step = loops[i].step;
      unsigned nb = loops[i].nb;
      for(j=0; j<total; j++)
	array[j] += ((j / step) % nb) * mul;
      mul *= nb;
    }

    free(loops);

    
    for(j=0; j<total; j++) {
      if (array[j] >= total) {
	if (verbose)
	  fprintf(stderr, "Invalid index interleaving generates out-of-range index %u\n", array[j]);
	goto out_with_array;
      }
      if (!array[j] && j) {
	if (verbose)
	  fprintf(stderr, "Invalid index interleaving generates duplicate index values\n");
	goto out_with_array;
      }
    }

    indexes->array = array;
  }

  return;

 out_with_array:
  free(array);
 out:
  return;
}

static hwloc_uint64_t
hwloc_synthetic_parse_memory_attr(const char *attr, const char **endp)
{
  const char *endptr;
  hwloc_uint64_t size;
  size = strtoull(attr, (char **) &endptr, 0);
  if (!hwloc_strncasecmp(endptr, "TB", 2)) {
    size *= 1000ULL*1000ULL*1000ULL*1000ULL;
    endptr += 2;
  } else if (!hwloc_strncasecmp(endptr, "TiB", 3)) {
    size <<= 40;
    endptr += 3;
  } else if (!hwloc_strncasecmp(endptr, "GB", 2)) {
    size *= 1000ULL*1000ULL*1000ULL;
    endptr += 2;
  } else if (!hwloc_strncasecmp(endptr, "GiB", 3)) {
    size <<= 30;
    endptr += 3;
  } else if (!hwloc_strncasecmp(endptr, "MB", 2)) {
    size *= 1000ULL*1000ULL;
    endptr += 2;
  } else if (!hwloc_strncasecmp(endptr, "MiB", 3)) {
    size <<= 20;
    endptr += 3;
  } else if (!hwloc_strncasecmp(endptr, "kB", 2)) {
    size *= 1000ULL;
    endptr += 2;
  } else if (!hwloc_strncasecmp(endptr, "kiB", 3)) {
    size <<= 10;
    endptr += 3;
  }
  *endp = endptr;
  return size;
}

static int
hwloc_synthetic_parse_attrs(const char *attrs, const char **next_posp,
			    struct hwloc_synthetic_attr_s *sattr,
			    struct hwloc_synthetic_indexes_s *sind,
			    int verbose)
{
  hwloc_obj_type_t type = sattr->type;
  const char *next_pos;
  hwloc_uint64_t memorysize = 0;
  const char *index_string = NULL;
  size_t index_string_length = 0;

  next_pos = (const char *) strchr(attrs, ')');
  if (!next_pos) {
    if (verbose)
      fprintf(stderr, "Missing attribute closing bracket in synthetic string doesn't have a number of objects at '%s'\n", attrs);
    errno = EINVAL;
    return -1;
  }

  while (')' != *attrs) {
    int iscache = hwloc__obj_type_is_cache(type);

    if (iscache && !strncmp("size=", attrs, 5)) {
      memorysize = hwloc_synthetic_parse_memory_attr(attrs+5, &attrs);

    } else if (!iscache && !strncmp("memory=", attrs, 7)) {
      memorysize = hwloc_synthetic_parse_memory_attr(attrs+7, &attrs);

    } else if (!strncmp("memorysidecachesize=", attrs, 20)) {
      sattr->memorysidecachesize = hwloc_synthetic_parse_memory_attr(attrs+20, &attrs);

    } else if (!strncmp("indexes=", attrs, 8)) {
      index_string = attrs+8;
      attrs += 8;
      index_string_length = strcspn(attrs, " )");
      attrs += index_string_length;

    } else {
      size_t length = strcspn(attrs, " )");
      fprintf(stderr, "hwloc/synthetic: Ignoring unknown attribute at '%s'\n", attrs);
      attrs += length;
    }

    if (' ' == *attrs)
      attrs++;
    else if (')' != *attrs) {
      if (verbose)
	fprintf(stderr, "Missing parameter separator at '%s'\n", attrs);
      errno = EINVAL;
      return -1;
    }
  }

  sattr->memorysize = memorysize;

  if (index_string) {
    if (sind->string && verbose)
      fprintf(stderr, "Overwriting duplicate indexes attribute with last occurence\n");
    sind->string = index_string;
    sind->string_length = (unsigned long)index_string_length;
  }

  *next_posp = next_pos+1;
  return 0;
}

static void
hwloc_synthetic_set_default_attrs(struct hwloc_synthetic_attr_s *sattr,
                                  int *type_count)
{
  hwloc_obj_type_t type = sattr->type;

  if (type == HWLOC_OBJ_GROUP) {
    if (sattr->depth == (unsigned)-1)
      sattr->depth = type_count[HWLOC_OBJ_GROUP]--;

  } else if (hwloc__obj_type_is_cache(type)) {
    if (!sattr->memorysize) {
      if (1 == sattr->depth)
        
        sattr->memorysize = 32*1024;
      else
        
        sattr->memorysize = 256ULL*1024 << (2*sattr->depth);
    }

  } else if (type == HWLOC_OBJ_NUMANODE && !sattr->memorysize) {
    
    sattr->memorysize = 1024*1024*1024;
  }
}


static void
hwloc_synthetic_free_levels(struct hwloc_synthetic_backend_data_s *data)
{
  unsigned i;
  for(i=0; i<HWLOC_SYNTHETIC_MAX_DEPTH; i++) {
    struct hwloc_synthetic_level_data_s *curlevel = &data->level[i];
    struct hwloc_synthetic_attached_s **pprev = &curlevel->attached;
    while (*pprev) {
      struct hwloc_synthetic_attached_s *cur = *pprev;
      *pprev = cur->next;
      free(cur);
    }
    free(curlevel->indexes.array);
    if (!curlevel->arity)
      break;
  }
  free(data->numa_attached_indexes.array);
}


static int
hwloc_backend_synthetic_init(struct hwloc_synthetic_backend_data_s *data,
			     const char *description)
{
  const char *pos, *next_pos;
  unsigned long item, count;
  unsigned i;
  int type_count[HWLOC_OBJ_TYPE_MAX];
  unsigned unset;
  int verbose = 0;
  const char *env = getenv("HWLOC_SYNTHETIC_VERBOSE");
  int err;
  unsigned long totalarity = 1;

  if (env)
    verbose = atoi(env);

  data->numa_attached_nr = 0;
  data->numa_attached_indexes.array = NULL;

  
  data->level[0].totalwidth = 1;
  data->level[0].attr.type = HWLOC_OBJ_MACHINE;
  data->level[0].indexes.string = NULL;
  data->level[0].indexes.array = NULL;
  data->level[0].attr.memorysize = 0;
  data->level[0].attr.memorysidecachesize = 0;
  data->level[0].attached = NULL;
  type_count[HWLOC_OBJ_MACHINE] = 1;
  if (*description == '(') {
    err = hwloc_synthetic_parse_attrs(description+1, &description, &data->level[0].attr, &data->level[0].indexes, verbose);
    if (err < 0)
      return err;
  }

  data->numa_attached_indexes.string = NULL;
  data->numa_attached_indexes.array = NULL;

  for (pos = description, count = 1; *pos; pos = next_pos) {
    hwloc_obj_type_t type = HWLOC_OBJ_TYPE_NONE;
    union hwloc_obj_attr_u attrs;

    
    data->level[count-1].arity = 0;

    while (*pos == ' ' || *pos == '\n')
      pos++;

    if (!*pos)
      break;

    if (*pos == '[') {
      
      struct hwloc_synthetic_attached_s *attached, **pprev;
      char *attr;

      pos++;

      if (hwloc_type_sscanf(pos, &type, &attrs, sizeof(attrs)) < 0) {
	if (verbose)
	  fprintf(stderr, "Synthetic string with unknown attached object type at '%s'\n", pos);
	errno = EINVAL;
	goto error;
      }
      if (type != HWLOC_OBJ_NUMANODE) {
	if (verbose)
	  fprintf(stderr, "Synthetic string with disallowed attached object type at '%s'\n", pos);
	errno = EINVAL;
	goto error;
      }
      data->numa_attached_nr += data->level[count-1].totalwidth;

      attached = malloc(sizeof(*attached));
      if (attached) {
	attached->attr.type = type;
	attached->attr.memorysize = 0;
	attached->attr.memorysidecachesize = 0;
	
	attached->next = NULL;
	pprev = &data->level[count-1].attached;
	while (*pprev)
	  pprev = &((*pprev)->next);
	*pprev = attached;
      }

      next_pos = strchr(pos, ']');
      if (!next_pos) {
	if (verbose)
	  fprintf(stderr,"Synthetic string doesn't have a closing `]' after attached object type at '%s'\n", pos);
	errno = EINVAL;
	goto error;
      }

      attr = strchr(pos, '(');
      if (attr && attr < next_pos && attached) {
	const char *dummy;
	err = hwloc_synthetic_parse_attrs(attr+1, &dummy, &attached->attr, &data->numa_attached_indexes, verbose);
	if (err < 0)
	  goto error;
      }

      next_pos++;
      continue;
    }

    

    
    data->level[count].indexes.string = NULL;
    data->level[count].indexes.array = NULL;
    data->level[count].attached = NULL;

    if (*pos < '0' || *pos > '9') {
      if (hwloc_type_sscanf(pos, &type, &attrs, sizeof(attrs)) < 0) {
	if (!strncmp(pos, "Tile", 4) || !strncmp(pos, "Module", 6)) {
	  
	  type = HWLOC_OBJ_GROUP;
	} else {
	  
	  if (verbose)
	    fprintf(stderr, "Synthetic string with unknown object type at '%s'\n", pos);
	  errno = EINVAL;
	  goto error;
	}
      }
      if (type == HWLOC_OBJ_MACHINE || type == HWLOC_OBJ_MISC || type == HWLOC_OBJ_BRIDGE || type == HWLOC_OBJ_PCI_DEVICE || type == HWLOC_OBJ_OS_DEVICE) {
	if (verbose)
	  fprintf(stderr, "Synthetic string with disallowed object type at '%s'\n", pos);
	errno = EINVAL;
	goto error;
      }

      next_pos = strchr(pos, ':');
      if (!next_pos) {
	if (verbose)
	  fprintf(stderr,"Synthetic string doesn't have a `:' after object type at '%s'\n", pos);
	errno = EINVAL;
	goto error;
      }
      pos = next_pos + 1;
    }

    data->level[count].attr.type = type;
    data->level[count].attr.depth = (unsigned) -1;
    data->level[count].attr.cachetype = (hwloc_obj_cache_type_t) -1;
    if (hwloc__obj_type_is_cache(type)) {
      
      data->level[count].attr.depth = attrs.cache.depth;
      data->level[count].attr.cachetype = attrs.cache.type;
    } else if (type == HWLOC_OBJ_GROUP) {
      
      data->level[count].attr.depth = attrs.group.depth;
    }

    
    item = strtoul(pos, (char **)&next_pos, 0);
    if (next_pos == pos) {
      if (verbose)
	fprintf(stderr,"Synthetic string doesn't have a number of objects at '%s'\n", pos);
      errno = EINVAL;
      goto error;
    }
    if (!item) {
      if (verbose)
	fprintf(stderr,"Synthetic string with disallowed 0 number of objects at '%s'\n", pos);
      errno = EINVAL;
      goto error;
    }

    totalarity *= item;
    data->level[count].totalwidth = totalarity;
    data->level[count].indexes.string = NULL;
    data->level[count].indexes.array = NULL;
    data->level[count].attr.memorysize = 0;
    data->level[count].attr.memorysidecachesize = 0;
    if (*next_pos == '(') {
      err = hwloc_synthetic_parse_attrs(next_pos+1, &next_pos, &data->level[count].attr, &data->level[count].indexes, verbose);
      if (err < 0)
	goto error;
    }

    if (count + 1 >= HWLOC_SYNTHETIC_MAX_DEPTH) {
      if (verbose)
	fprintf(stderr,"Too many synthetic levels, max %d\n", HWLOC_SYNTHETIC_MAX_DEPTH);
      errno = EINVAL;
      goto error;
    }
    if (item > UINT_MAX) {
      if (verbose)
	fprintf(stderr,"Too big arity, max %u\n", UINT_MAX);
      errno = EINVAL;
      goto error;
    }

    data->level[count-1].arity = (unsigned)item;
    count++;
  }

  if (data->level[count-1].attr.type != HWLOC_OBJ_TYPE_NONE && data->level[count-1].attr.type != HWLOC_OBJ_PU) {
    if (verbose)
      fprintf(stderr, "Synthetic string cannot use non-PU type for last level\n");
    errno = EINVAL;
    return -1;
  }
  data->level[count-1].attr.type = HWLOC_OBJ_PU;

  for(i=HWLOC_OBJ_TYPE_MIN; i<HWLOC_OBJ_TYPE_MAX; i++) {
    type_count[i] = 0;
  }
  for(i=count-1; i>0; i--) {
    hwloc_obj_type_t type = data->level[i].attr.type;
    if (type != HWLOC_OBJ_TYPE_NONE) {
      type_count[type]++;
    }
  }

  
  if (!type_count[HWLOC_OBJ_PU]) {
    if (verbose)
      fprintf(stderr, "Synthetic string missing ending number of PUs\n");
    errno = EINVAL;
    return -1;
  } else if (type_count[HWLOC_OBJ_PU] > 1) {
    if (verbose)
      fprintf(stderr, "Synthetic string cannot have several PU levels\n");
    errno = EINVAL;
    return -1;
  }
  if (type_count[HWLOC_OBJ_PACKAGE] > 1) {
    if (verbose)
      fprintf(stderr, "Synthetic string cannot have several package levels\n");
    errno = EINVAL;
    return -1;
  }
  if (type_count[HWLOC_OBJ_DIE] > 1) {
    if (verbose)
      fprintf(stderr, "Synthetic string cannot have several die levels\n");
    errno = EINVAL;
    return -1;
  }
  if (type_count[HWLOC_OBJ_NUMANODE] > 1) {
    if (verbose)
      fprintf(stderr, "Synthetic string cannot have several NUMA node levels\n");
    errno = EINVAL;
    return -1;
  }
  if (type_count[HWLOC_OBJ_NUMANODE] && data->numa_attached_nr) {
    if (verbose)
      fprintf(stderr,"Synthetic string cannot have NUMA nodes both as a level and attached\n");
    errno = EINVAL;
    return -1;
  }
  if (type_count[HWLOC_OBJ_CORE] > 1) {
    if (verbose)
      fprintf(stderr, "Synthetic string cannot have several core levels\n");
    errno = EINVAL;
    return -1;
  }

  
  unset = 0;
  for(i=1; i<count-1; i++) {
    if (data->level[i].attr.type == HWLOC_OBJ_TYPE_NONE)
      unset++;
  }
  if (unset && unset != count-2) {
    if (verbose)
      fprintf(stderr, "Synthetic string cannot mix unspecified and specified types for levels\n");
    errno = EINVAL;
    return -1;
  }
  if (unset) {
    
    unsigned _count = count;
    unsigned neednuma = 0;
    unsigned needpack = 0;
    unsigned needcore = 0;
    unsigned needcaches = 0;
    unsigned needgroups = 0;
    
    _count -= 2;

    neednuma = (_count >= 1 && !data->numa_attached_nr);
    _count -= neednuma;

    needpack = (_count >= 1);
    _count -= needpack;

    needcore = (_count >= 1);
    _count -= needcore;

    needcaches = (_count > 4 ? 4 : _count);
    _count -= needcaches;

    needgroups = _count;

    
    for(i = 0; i < needgroups; i++) {
      unsigned depth = 1 + i;
      data->level[depth].attr.type = HWLOC_OBJ_GROUP;
      type_count[HWLOC_OBJ_GROUP]++;
    }
    if (needpack) {
      unsigned depth = 1 + needgroups;
      data->level[depth].attr.type = HWLOC_OBJ_PACKAGE;
      type_count[HWLOC_OBJ_PACKAGE] = 1;
    }
    if (neednuma) {
      unsigned depth = 1 + needgroups + needpack;
      data->level[depth].attr.type = HWLOC_OBJ_NUMANODE;
      type_count[HWLOC_OBJ_NUMANODE] = 1;
    }
    if (needcaches) {
      
      
      unsigned l3depth = 1 + needgroups + needpack + neednuma;
      unsigned l2depth = l3depth + (needcaches >= 3);
      unsigned l1depth = l2depth + 1;
      unsigned l1idepth = l1depth + 1;
      if (needcaches >= 3) {
	data->level[l3depth].attr.type = HWLOC_OBJ_L3CACHE;
	data->level[l3depth].attr.depth = 3;
	data->level[l3depth].attr.cachetype = HWLOC_OBJ_CACHE_UNIFIED;
	type_count[HWLOC_OBJ_L3CACHE] = 1;
      }
      data->level[l2depth].attr.type = HWLOC_OBJ_L2CACHE;
      data->level[l2depth].attr.depth = 2;
      data->level[l2depth].attr.cachetype = HWLOC_OBJ_CACHE_UNIFIED;
      type_count[HWLOC_OBJ_L2CACHE] = 1;
      if (needcaches >= 2) {
	data->level[l1depth].attr.type = HWLOC_OBJ_L1CACHE;
	data->level[l1depth].attr.depth = 1;
	data->level[l1depth].attr.cachetype = HWLOC_OBJ_CACHE_DATA;
	type_count[HWLOC_OBJ_L1CACHE] = 1;
      }
      if (needcaches >= 4) {
	data->level[l1idepth].attr.type = HWLOC_OBJ_L1ICACHE;
	data->level[l1idepth].attr.depth = 1;
	data->level[l1idepth].attr.cachetype = HWLOC_OBJ_CACHE_INSTRUCTION;
	type_count[HWLOC_OBJ_L1ICACHE] = 1;
      }
    }
    if (needcore) {
      unsigned depth = 1 + needgroups + needpack + neednuma + needcaches;
      data->level[depth].attr.type = HWLOC_OBJ_CORE;
      type_count[HWLOC_OBJ_CORE] = 1;
    }
  }

  
  if (!type_count[HWLOC_OBJ_NUMANODE] && !data->numa_attached_nr) {
    
    if (verbose)
      fprintf(stderr, "Inserting a NUMA level with a single object at depth 1\n");
    
    memmove(&data->level[2], &data->level[1], count*sizeof(struct hwloc_synthetic_level_data_s));
    data->level[1].attr.type = HWLOC_OBJ_NUMANODE;
    data->level[1].indexes.string = NULL;
    data->level[1].indexes.array = NULL;
    data->level[1].attr.memorysize = 0;
    data->level[1].attr.memorysidecachesize = 0;
    data->level[1].totalwidth = data->level[0].totalwidth;
    
    data->level[1].arity = data->level[0].arity;
    data->level[0].arity = 1;
    count++;
  }

  
  for (i=0; i<count; i++) {
    struct hwloc_synthetic_attached_s *attached;
    struct hwloc_synthetic_level_data_s *curlevel = &data->level[i];
    hwloc_synthetic_set_default_attrs(&curlevel->attr, type_count);
    for(attached = curlevel->attached; attached != NULL; attached = attached->next)
      hwloc_synthetic_set_default_attrs(&attached->attr, type_count);
    hwloc_synthetic_process_indexes(data, &curlevel->indexes, curlevel->totalwidth, verbose);
  }

  hwloc_synthetic_process_indexes(data, &data->numa_attached_indexes, data->numa_attached_nr, verbose);

  data->string = strdup(description);
  data->level[count-1].arity = 0;
  return 0;

 error:
  hwloc_synthetic_free_levels(data);
  return -1;
}

static void
hwloc_synthetic_set_attr(struct hwloc_synthetic_attr_s *sattr,
			 hwloc_obj_t obj)
{
  switch (obj->type) {
  case HWLOC_OBJ_GROUP:
    obj->attr->group.kind = HWLOC_GROUP_KIND_SYNTHETIC;
    obj->attr->group.subkind = sattr->depth-1;
    break;
  case HWLOC_OBJ_MACHINE:
    break;
  case HWLOC_OBJ_NUMANODE:
    obj->attr->numanode.local_memory = sattr->memorysize;
    obj->attr->numanode.page_types_len = 1;
    obj->attr->numanode.page_types = malloc(sizeof(*obj->attr->numanode.page_types));
    memset(obj->attr->numanode.page_types, 0, sizeof(*obj->attr->numanode.page_types));
    obj->attr->numanode.page_types[0].size = 4096;
    obj->attr->numanode.page_types[0].count = sattr->memorysize / 4096;
    break;
  case HWLOC_OBJ_MEMCACHE:
    obj->attr->cache.depth = 1;
    obj->attr->cache.linesize = 64;
    obj->attr->cache.type = HWLOC_OBJ_CACHE_UNIFIED;
    obj->attr->cache.size = sattr->memorysidecachesize;
    break;
  case HWLOC_OBJ_PACKAGE:
  case HWLOC_OBJ_DIE:
    break;
  case HWLOC_OBJ_L1CACHE:
  case HWLOC_OBJ_L2CACHE:
  case HWLOC_OBJ_L3CACHE:
  case HWLOC_OBJ_L4CACHE:
  case HWLOC_OBJ_L5CACHE:
  case HWLOC_OBJ_L1ICACHE:
  case HWLOC_OBJ_L2ICACHE:
  case HWLOC_OBJ_L3ICACHE:
    obj->attr->cache.depth = sattr->depth;
    obj->attr->cache.linesize = 64;
    obj->attr->cache.type = sattr->cachetype;
    obj->attr->cache.size = sattr->memorysize;
    break;
  case HWLOC_OBJ_CORE:
    break;
  case HWLOC_OBJ_PU:
    break;
  default:
    
    assert(0);
    break;
  }
}

static unsigned
hwloc_synthetic_next_index(struct hwloc_synthetic_indexes_s *indexes, hwloc_obj_type_t type)
{
  unsigned os_index = indexes->next++;

  if (indexes->array)
    os_index = indexes->array[os_index];
  else if (hwloc__obj_type_is_cache(type) || type == HWLOC_OBJ_GROUP)
    
    os_index = HWLOC_UNKNOWN_INDEX;

  return os_index;
}

static void
hwloc_synthetic_insert_attached(struct hwloc_topology *topology,
				struct hwloc_synthetic_backend_data_s *data,
				struct hwloc_synthetic_attached_s *attached,
				hwloc_bitmap_t set)
{
  hwloc_obj_t child;
  unsigned attached_os_index;

  if (!attached)
    return;

  assert(attached->attr.type == HWLOC_OBJ_NUMANODE);

  attached_os_index = hwloc_synthetic_next_index(&data->numa_attached_indexes, HWLOC_OBJ_NUMANODE);

  child = hwloc_alloc_setup_object(topology, attached->attr.type, attached_os_index);
  child->cpuset = hwloc_bitmap_dup(set);

  child->nodeset = hwloc_bitmap_alloc();
  hwloc_bitmap_set(child->nodeset, attached_os_index);

  hwloc_synthetic_set_attr(&attached->attr, child);

  hwloc__insert_object_by_cpuset(topology, NULL, child, "synthetic:attached");

  if (attached->attr.memorysidecachesize) {
    hwloc_obj_t mscachechild = hwloc_alloc_setup_object(topology, HWLOC_OBJ_MEMCACHE, HWLOC_UNKNOWN_INDEX);
    mscachechild->cpuset = hwloc_bitmap_dup(set);
    mscachechild->nodeset = hwloc_bitmap_dup(child->nodeset);
    hwloc_synthetic_set_attr(&attached->attr, mscachechild);
    hwloc__insert_object_by_cpuset(topology, NULL, mscachechild, "synthetic:attached:mscache");
  }

  hwloc_synthetic_insert_attached(topology, data, attached->next, set);
}


static void
hwloc__look_synthetic(struct hwloc_topology *topology,
		      struct hwloc_synthetic_backend_data_s *data,
		      int level,
		      hwloc_bitmap_t parent_cpuset)
{
  hwloc_obj_t obj;
  unsigned i;
  struct hwloc_synthetic_level_data_s *curlevel = &data->level[level];
  hwloc_obj_type_t type = curlevel->attr.type;
  hwloc_bitmap_t set;
  unsigned os_index;

  assert(hwloc__obj_type_is_normal(type) || type == HWLOC_OBJ_NUMANODE);
  assert(type != HWLOC_OBJ_MACHINE);

  os_index = hwloc_synthetic_next_index(&curlevel->indexes, type);

  set = hwloc_bitmap_alloc();
  if (!curlevel->arity) {
    hwloc_bitmap_set(set, os_index);
  } else {
    for (i = 0; i < curlevel->arity; i++)
      hwloc__look_synthetic(topology, data, level + 1, set);
  }

  hwloc_bitmap_or(parent_cpuset, parent_cpuset, set);

  if (hwloc_filter_check_keep_object_type(topology, type)) {
    obj = hwloc_alloc_setup_object(topology, type, os_index);
    obj->cpuset = hwloc_bitmap_dup(set);

    if (type == HWLOC_OBJ_NUMANODE) {
      obj->nodeset = hwloc_bitmap_alloc();
      hwloc_bitmap_set(obj->nodeset, os_index);
    }

    hwloc_synthetic_set_attr(&curlevel->attr, obj);

    hwloc__insert_object_by_cpuset(topology, NULL, obj, "synthetic");

    if (type == HWLOC_OBJ_NUMANODE && curlevel->attr.memorysidecachesize) {
      hwloc_obj_t mscachechild = hwloc_alloc_setup_object(topology, HWLOC_OBJ_MEMCACHE, HWLOC_UNKNOWN_INDEX);
      mscachechild->cpuset = hwloc_bitmap_dup(set);
      mscachechild->nodeset = hwloc_bitmap_dup(obj->nodeset);
      hwloc_synthetic_set_attr(&curlevel->attr, mscachechild);
      hwloc__insert_object_by_cpuset(topology, NULL, mscachechild, "synthetic:mscache");
    }
  }

  hwloc_synthetic_insert_attached(topology, data, curlevel->attached, set);

  hwloc_bitmap_free(set);
}

static int
hwloc_look_synthetic(struct hwloc_backend *backend, struct hwloc_disc_status *dstatus)
{
  

  struct hwloc_topology *topology = backend->topology;
  struct hwloc_synthetic_backend_data_s *data = backend->private_data;
  hwloc_bitmap_t cpuset = hwloc_bitmap_alloc();
  unsigned i;

  assert(dstatus->phase == HWLOC_DISC_PHASE_GLOBAL);

  assert(!topology->levels[0][0]->cpuset);

  hwloc_alloc_root_sets(topology->levels[0][0]);

  topology->support.discovery->pu = 1;
  topology->support.discovery->numa = 1; 
  topology->support.discovery->numa_memory = 1; 

  
  for (i = 0; data->level[i].arity > 0; i++)
    data->level[i].indexes.next = 0;
  data->numa_attached_indexes.next = 0;
  
  data->level[i].indexes.next = 0;

  
  topology->levels[0][0]->type = data->level[0].attr.type;
  hwloc_synthetic_set_attr(&data->level[0].attr, topology->levels[0][0]);

  for (i = 0; i < data->level[0].arity; i++)
    hwloc__look_synthetic(topology, data, 1, cpuset);

  hwloc_synthetic_insert_attached(topology, data, data->level[0].attached, cpuset);

  hwloc_bitmap_free(cpuset);

  hwloc_obj_add_info(topology->levels[0][0], "Backend", "Synthetic");
  hwloc_obj_add_info(topology->levels[0][0], "SyntheticDescription", data->string);
  return 0;
}

static void
hwloc_synthetic_backend_disable(struct hwloc_backend *backend)
{
  struct hwloc_synthetic_backend_data_s *data = backend->private_data;
  hwloc_synthetic_free_levels(data);
  free(data->string);
  free(data);
}

static struct hwloc_backend *
hwloc_synthetic_component_instantiate(struct hwloc_topology *topology,
				      struct hwloc_disc_component *component,
				      unsigned excluded_phases __hwloc_attribute_unused,
				      const void *_data1,
				      const void *_data2 __hwloc_attribute_unused,
				      const void *_data3 __hwloc_attribute_unused)
{
  struct hwloc_backend *backend;
  struct hwloc_synthetic_backend_data_s *data;
  int err;

  if (!_data1) {
    const char *env = getenv("HWLOC_SYNTHETIC");
    if (env) {
      
      _data1 = env;
    } else {
      errno = EINVAL;
      goto out;
    }
  }

  backend = hwloc_backend_alloc(topology, component);
  if (!backend)
    goto out;

  data = malloc(sizeof(*data));
  if (!data) {
    errno = ENOMEM;
    goto out_with_backend;
  }

  err = hwloc_backend_synthetic_init(data, (const char *) _data1);
  if (err < 0)
    goto out_with_data;

  backend->private_data = data;
  backend->discover = hwloc_look_synthetic;
  backend->disable = hwloc_synthetic_backend_disable;
  backend->is_thissystem = 0;

  return backend;

 out_with_data:
  free(data);
 out_with_backend:
  free(backend);
 out:
  return NULL;
}

static struct hwloc_disc_component hwloc_synthetic_disc_component = {
  "synthetic",
  HWLOC_DISC_PHASE_GLOBAL,
  ~0,
  hwloc_synthetic_component_instantiate,
  30,
  1,
  NULL
};

const struct hwloc_component hwloc_synthetic_component = {
  HWLOC_COMPONENT_ABI,
  NULL, NULL,
  HWLOC_COMPONENT_TYPE_DISC,
  0,
  &hwloc_synthetic_disc_component
};

static __hwloc_inline int
hwloc__export_synthetic_update_status(int *ret, char **tmp, ssize_t *tmplen, int res)
{
  if (res < 0)
    return -1;
  *ret += res;
  if (res >= *tmplen)
    res = *tmplen>0 ? (int)(*tmplen) - 1 : 0;
  *tmp += res;
  *tmplen -= res;
  return 0;
}

static __hwloc_inline void
hwloc__export_synthetic_add_char(int *ret, char **tmp, ssize_t *tmplen, char c)
{
  if (*tmplen > 1) {
    (*tmp)[0] = c;
    (*tmp)[1] = '\0';
    (*tmp)++;
    (*tmplen)--;
  }
  (*ret)++;
}

static int
hwloc__export_synthetic_indexes(hwloc_obj_t *level, unsigned total,
				char *buffer, size_t buflen)
{
  unsigned step = 1;
  unsigned nr_loops = 0;
  struct hwloc_synthetic_intlv_loop_s *loops = NULL, *tmploops;
  hwloc_obj_t cur;
  unsigned i, j;
  ssize_t tmplen = buflen;
  char *tmp = buffer;
  int res, ret = 0;

  
  if (level[0]->os_index)
    goto exportall;

  while (step != total) {
    
    if (total % step)
      goto exportall;

    
    for(i=1; i<total; i++)
      if (level[i]->os_index == step)
	break;
    if (i == total)
      goto exportall;
    for(j=2; j<total/i; j++)
      if (level[i*j]->os_index != step*j)
	break;

    nr_loops++;
    tmploops = realloc(loops, nr_loops*sizeof(*loops));
    if (!tmploops)
      goto exportall;
    loops = tmploops;
    loops[nr_loops-1].step = i;
    loops[nr_loops-1].nb = j;
    step *= j;
  }

  
  for(i=0; i<total; i++) {
    unsigned ind = 0;
    unsigned mul = 1;
    for(j=0; j<nr_loops; j++) {
      ind += (i / loops[j].step) % loops[j].nb * mul;
      mul *= loops[j].nb;
    }
    if (level[i]->os_index != ind)
      goto exportall;
  }

  
  for(j=0; j<nr_loops; j++) {
    res = hwloc_snprintf(tmp, tmplen, "%u*%u%s", loops[j].step, loops[j].nb,
			 j == nr_loops-1 ? ")" : ":");
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0) {
      free(loops);
      return -1;
    }
  }

  free(loops);
  return ret;

 exportall:
  free(loops);

  
  cur = level[0];
  while (cur) {
    res = hwloc_snprintf(tmp, tmplen, "%u%s", cur->os_index,
			 cur->next_cousin ? "," : ")");
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
      return -1;
    cur = cur->next_cousin;
  }
  return ret;
}

static int
hwloc__export_synthetic_obj_attr(struct hwloc_topology * topology,
                                 unsigned long flags,
				 hwloc_obj_t obj,
				 char *buffer, size_t buflen)
{
  const char * separator = " ";
  const char * prefix = "(";
  char cachesize[64] = "";
  char memsize[64] = "";
  char memorysidecachesize[64] = "";
  int needindexes = 0;

  if (hwloc__obj_type_is_cache(obj->type) && obj->attr->cache.size) {
    snprintf(cachesize, sizeof(cachesize), "%ssize=%llu",
	     prefix, (unsigned long long) obj->attr->cache.size);
    prefix = separator;
  }
  if (obj->type == HWLOC_OBJ_NUMANODE && obj->attr->numanode.local_memory) {
    snprintf(memsize, sizeof(memsize), "%smemory=%llu",
	     prefix, (unsigned long long) obj->attr->numanode.local_memory);
    prefix = separator;
  }
  if (obj->type == HWLOC_OBJ_NUMANODE && !(flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_V1)) {
    hwloc_obj_t memorysidecache = obj->parent;
    hwloc_uint64_t size = 0;
    while (memorysidecache && memorysidecache->type == HWLOC_OBJ_MEMCACHE) {
      size += memorysidecache->attr->cache.size;
      memorysidecache = memorysidecache->parent;
    }
    if (size) {
      snprintf(memorysidecachesize, sizeof(memorysidecachesize), "%smemorysidecachesize=%llu",
               prefix, (unsigned long long) size);
      prefix = separator;
    }
  }
  if (!obj->logical_index 
      && (obj->type == HWLOC_OBJ_PU || obj->type == HWLOC_OBJ_NUMANODE)) {
    hwloc_obj_t cur = obj;
    while (cur) {
      if (cur->os_index != cur->logical_index) {
	needindexes = 1;
	break;
      }
      cur = cur->next_cousin;
    }
  }
  if (*cachesize || *memsize || *memorysidecachesize || needindexes) {
    ssize_t tmplen = buflen;
    char *tmp = buffer;
    int res, ret = 0;

    res = hwloc_snprintf(tmp, tmplen, "%s%s%s%s", cachesize, memsize, memorysidecachesize, needindexes ? "" : ")");
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
      return -1;

    if (needindexes) {
      unsigned total;
      hwloc_obj_t *level;

      if (obj->depth < 0) {
	assert(obj->depth == HWLOC_TYPE_DEPTH_NUMANODE);
	total = topology->slevels[HWLOC_SLEVEL_NUMANODE].nbobjs;
	level = topology->slevels[HWLOC_SLEVEL_NUMANODE].objs;
      } else {
	total = topology->level_nbobjects[obj->depth];
	level = topology->levels[obj->depth];
      }

      res = hwloc_snprintf(tmp, tmplen, "%sindexes=", prefix);
      if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
	return -1;

      res = hwloc__export_synthetic_indexes(level, total, tmp, tmplen);
      if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
	return -1;
    }
    return ret;
  } else {
    return 0;
  }
}

static int
hwloc__export_synthetic_obj(struct hwloc_topology * topology, unsigned long flags,
			    hwloc_obj_t obj, unsigned arity,
			    char *buffer, size_t buflen)
{
  char aritys[12] = "";
  ssize_t tmplen = buflen;
  char *tmp = buffer;
  int res, ret = 0;

  
  if (arity != (unsigned)-1)
    snprintf(aritys, sizeof(aritys), ":%u", arity);
  if (hwloc__obj_type_is_cache(obj->type)
      && (flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_EXTENDED_TYPES)) {
    
    res = hwloc_snprintf(tmp, tmplen, "Cache%s", aritys);

  } else if (obj->type == HWLOC_OBJ_PACKAGE
	     && (flags & (HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_EXTENDED_TYPES
			  |HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_V1))) {
    
    res = hwloc_snprintf(tmp, tmplen, "Socket%s", aritys);

  } else if (obj->type == HWLOC_OBJ_DIE
	     && (flags & (HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_EXTENDED_TYPES
			  |HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_V1))) {
    
    res = hwloc_snprintf(tmp, tmplen, "Group%s", aritys);

  } else if (obj->type == HWLOC_OBJ_GROUP 
      || flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_EXTENDED_TYPES) {
    res = hwloc_snprintf(tmp, tmplen, "%s%s", hwloc_obj_type_string(obj->type), aritys);
  } else {
    char types[64];
    hwloc_obj_type_snprintf(types, sizeof(types), obj, 1);
    res = hwloc_snprintf(tmp, tmplen, "%s%s", types, aritys);
  }
  if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
    return -1;

  if (!(flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_ATTRS)) {
    
    res = hwloc__export_synthetic_obj_attr(topology, flags, obj, tmp, tmplen);
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
      return -1;
  }

  return ret;
}

static int
hwloc__export_synthetic_memory_children(struct hwloc_topology * topology, unsigned long flags,
					hwloc_obj_t parent,
					char *buffer, size_t buflen,
					int needprefix, int verbose)
{
  hwloc_obj_t mchild;
  ssize_t tmplen = buflen;
  char *tmp = buffer;
  int res, ret = 0;

  mchild = parent->memory_first_child;
  if (!mchild)
    return 0;

  if (flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_V1) {
    
    if (parent->memory_arity > 1) {
      
      if (verbose)
	fprintf(stderr, "Cannot export to synthetic v1 if multiple memory children are attached to the same location.\n");
      errno = EINVAL;
      return -1;
    }

    if (needprefix)
      hwloc__export_synthetic_add_char(&ret, &tmp, &tmplen, ' ');

    
    while (mchild->type != HWLOC_OBJ_NUMANODE)
      mchild = mchild->memory_first_child;
    res = hwloc__export_synthetic_obj(topology, flags, mchild, 1, tmp, tmplen);
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
      return -1;
    return ret;
  }

  while (mchild) {
    
    hwloc_obj_t numanode = mchild;
    
    while (numanode && numanode->type != HWLOC_OBJ_NUMANODE) {
      if (verbose && numanode->memory_arity > 1) {
        static int warned = 0;
        if (!warned)
          fprintf(stderr, "Ignoring non-first memory children at non-first level of memory hierarchy.\n");
        warned = 1;
      }
      numanode = numanode->memory_first_child;
    }
    assert(numanode); 

    if (needprefix)
      hwloc__export_synthetic_add_char(&ret, &tmp, &tmplen, ' ');

    hwloc__export_synthetic_add_char(&ret, &tmp, &tmplen, '[');

    res = hwloc__export_synthetic_obj(topology, flags, numanode, (unsigned)-1, tmp, tmplen);
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
      return -1;

    hwloc__export_synthetic_add_char(&ret, &tmp, &tmplen, ']');

    needprefix = 1;
    mchild = mchild->next_sibling;
  }

  return ret;
}

static int
hwloc_check_memory_symmetric(struct hwloc_topology * topology)
{
  hwloc_bitmap_t remaining_nodes;

  remaining_nodes = hwloc_bitmap_dup(hwloc_get_root_obj(topology)->nodeset);
  if (!remaining_nodes)
    
    return -1;

  while (!hwloc_bitmap_iszero(remaining_nodes)) {
    unsigned idx;
    hwloc_obj_t node;
    hwloc_obj_t first_parent;
    unsigned i;

    idx = hwloc_bitmap_first(remaining_nodes);
    node = hwloc_get_numanode_obj_by_os_index(topology, idx);
    assert(node);

    first_parent = node->parent;

    
    for(i=0; i<hwloc_get_nbobjs_by_depth(topology, first_parent->depth); i++) {
      hwloc_obj_t parent, mchild;

      parent = hwloc_get_obj_by_depth(topology, first_parent->depth, i);
      assert(parent);

      
      if (parent->memory_arity != first_parent->memory_arity)
	goto out_with_bitmap;

      
      mchild = parent->memory_first_child;
      while (mchild) {
	hwloc_bitmap_clr(remaining_nodes, mchild->os_index); 
	mchild = mchild->next_sibling;
      }
    }
  }

  hwloc_bitmap_free(remaining_nodes);
  return 0;

 out_with_bitmap:
  hwloc_bitmap_free(remaining_nodes);
  return -1;
}

int
hwloc_topology_export_synthetic(struct hwloc_topology * topology,
				char *buffer, size_t buflen,
				unsigned long flags)
{
  hwloc_obj_t obj = hwloc_get_root_obj(topology);
  ssize_t tmplen = buflen;
  char *tmp = buffer;
  int res, ret = 0;
  unsigned arity;
  int needprefix = 0;
  int verbose = 0;
  const char *env = getenv("HWLOC_SYNTHETIC_VERBOSE");

  if (env)
    verbose = atoi(env);

  if (!topology->is_loaded) {
    errno = EINVAL;
    return -1;
  }

  if (flags & ~(HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_EXTENDED_TYPES
		|HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_ATTRS
		|HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_V1
		|HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_IGNORE_MEMORY)) {
    errno = EINVAL;
    return -1;
  }

  

  

  

  if (!obj->symmetric_subtree) {
    if (verbose)
      fprintf(stderr, "Cannot export to synthetic unless topology is symmetric (root->symmetric_subtree must be set).\n");
    errno = EINVAL;
    return -1;
  }

  if (!(flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_IGNORE_MEMORY)
      && hwloc_check_memory_symmetric(topology) < 0) {
    if (verbose)
      fprintf(stderr, "Cannot export to synthetic unless memory is attached symmetrically.\n");
    errno = EINVAL;
    return -1;
  }

  if (flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_V1) {
    
    hwloc_obj_t node, parent;
    signed pdepth;

    node = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NUMANODE, 0);
    assert(node);
    parent = node->parent;
    while (!hwloc__obj_type_is_normal(parent->type))
      parent = parent->parent;
    pdepth = parent->depth;

    while ((node = node->next_cousin) != NULL) {
      parent = node->parent;
      while (!hwloc__obj_type_is_normal(parent->type))
        parent = parent->parent;
      if (parent->depth != pdepth) {
	if (verbose)
	  fprintf(stderr, "Cannot export to synthetic v1 if memory is attached to parents at different depths.\n");
	errno = EINVAL;
	return -1;
      }
    }
  }

  

  if (!(flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_NO_ATTRS)) {
    
    res = hwloc__export_synthetic_obj_attr(topology, flags, obj, tmp, tmplen);
    if (res > 0)
      needprefix = 1;
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
      return -1;
  }

  if (!(flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_IGNORE_MEMORY)) {
    res = hwloc__export_synthetic_memory_children(topology, flags, obj, tmp, tmplen, needprefix, verbose);
    if (res > 0)
      needprefix = 1;
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
      return -1;
  }

  arity = obj->arity;
  while (arity) {
    
    obj = obj->first_child;

    if (needprefix)
      hwloc__export_synthetic_add_char(&ret, &tmp, &tmplen, ' ');

    res = hwloc__export_synthetic_obj(topology, flags, obj, arity, tmp, tmplen);
    if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
      return -1;

    if (!(flags & HWLOC_TOPOLOGY_EXPORT_SYNTHETIC_FLAG_IGNORE_MEMORY)) {
      res = hwloc__export_synthetic_memory_children(topology, flags, obj, tmp, tmplen, 1, verbose);
      if (hwloc__export_synthetic_update_status(&ret, &tmp, &tmplen, res) < 0)
	return -1;
    }

    
    needprefix = 1;
    arity = obj->arity;
  }

  return ret;
}

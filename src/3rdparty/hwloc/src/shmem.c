

#include "private/autogen/config.h"
#include "hwloc.h"
#include "hwloc/shmem.h"
#include "private/private.h"

#ifndef HWLOC_WIN_SYS

#include <sys/mman.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <assert.h>

#define HWLOC_SHMEM_HEADER_VERSION 1

struct hwloc_shmem_header {
  uint32_t header_version; 
  uint32_t header_length; 
  uint64_t mmap_address; 
  uint64_t mmap_length; 
  
};

#define HWLOC_SHMEM_MALLOC_ALIGN 8UL

static void *
tma_shmem_malloc(struct hwloc_tma * tma,
		 size_t length)
{
  void *current = tma->data;
  tma->data = (char*)tma->data  + ((length + HWLOC_SHMEM_MALLOC_ALIGN - 1) & ~(HWLOC_SHMEM_MALLOC_ALIGN - 1));
  return current;

}

static void *
tma_get_length_malloc(struct hwloc_tma * tma,
		      size_t length)
{
  size_t *tma_length = tma->data;
  *tma_length += (length + HWLOC_SHMEM_MALLOC_ALIGN - 1) & ~(HWLOC_SHMEM_MALLOC_ALIGN - 1);
  return malloc(length);

}

int
hwloc_shmem_topology_get_length(hwloc_topology_t topology,
				size_t *lengthp,
				unsigned long flags)
{
  hwloc_topology_t new;
  struct hwloc_tma tma;
  size_t length = 0;
  unsigned long pagesize = hwloc_getpagesize(); 
  int err;

  if (flags) {
    errno = EINVAL;
    return -1;
  }

  tma.malloc = tma_get_length_malloc;
  tma.dontfree = 0;
  tma.data = &length;

  err = hwloc__topology_dup(&new, topology, &tma);
  if (err < 0)
    return err;
  hwloc_topology_destroy(new);

  *lengthp = (sizeof(struct hwloc_shmem_header) + length + pagesize - 1) & ~(pagesize - 1);
  return 0;
}

int
hwloc_shmem_topology_write(hwloc_topology_t topology,
			   int fd, hwloc_uint64_t fileoffset,
			   void *mmap_address, size_t length,
			   unsigned long flags)
{
  hwloc_topology_t new;
  struct hwloc_tma tma;
  struct hwloc_shmem_header header;
  uint32_t header_length = (sizeof(header) + sizeof(void*) - 1) & ~(sizeof(void*) - 1); 
  void *mmap_res;
  int err;

  if (flags) {
    errno = EINVAL;
    return -1;
  }

  
  hwloc_internal_distances_refresh(topology);
  hwloc_internal_memattrs_refresh(topology);

  header.header_version = HWLOC_SHMEM_HEADER_VERSION;
  header.header_length = header_length;
  header.mmap_address = (uintptr_t) mmap_address;
  header.mmap_length = length;

  err = lseek(fd, fileoffset, SEEK_SET);
  if (err < 0)
    return -1;

  err = write(fd, &header, sizeof(header));
  if (err != sizeof(header))
    return -1;

  err = ftruncate(fd, fileoffset + length);
  if (err < 0)
    return -1;

  mmap_res = mmap(mmap_address, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, fileoffset);
  if (mmap_res == MAP_FAILED)
    return -1;
  if (mmap_res != mmap_address) {
    munmap(mmap_res, length);
    errno = EBUSY;
    return -1;
  }

  tma.malloc = tma_shmem_malloc;
  tma.dontfree = 1;
  tma.data = (char *)mmap_res + header_length;
  err = hwloc__topology_dup(&new, topology, &tma);
  if (err < 0)
    return err;
  assert((char*)new == (char*)mmap_address + sizeof(header));

  assert((char *)mmap_res <= (char *)mmap_address + length);

  
  hwloc_internal_distances_refresh(new);
  hwloc_internal_memattrs_refresh(topology);

  
  munmap(mmap_address, length);
  hwloc_components_fini();

  return 0;
}

int
hwloc_shmem_topology_adopt(hwloc_topology_t *topologyp,
			   int fd, hwloc_uint64_t fileoffset,
			   void *mmap_address, size_t length,
			   unsigned long flags)
{
  hwloc_topology_t new, old;
  struct hwloc_shmem_header header;
  uint32_t header_length = (sizeof(header) + sizeof(void*) - 1) & ~(sizeof(void*) - 1); 
  void *mmap_res;
  int err;

  if (flags) {
    errno = EINVAL;
    return -1;
  }

  err = lseek(fd, fileoffset, SEEK_SET);
  if (err < 0)
    return -1;

  err = read(fd, &header, sizeof(header));
  if (err != sizeof(header))
    return -1;

  if (header.header_version != HWLOC_SHMEM_HEADER_VERSION
      || header.header_length != header_length
      || header.mmap_address != (uintptr_t) mmap_address
      || header.mmap_length != length) {
    errno = EINVAL;
    return -1;
  }

  mmap_res = mmap(mmap_address, length, PROT_READ, MAP_SHARED, fd, fileoffset);
  if (mmap_res == MAP_FAILED)
    return -1;
  if (mmap_res != mmap_address) {
    errno = EBUSY;
    goto out_with_mmap;
  }

  old = (hwloc_topology_t)((char*)mmap_address + header_length);
  if (hwloc_topology_abi_check(old) < 0) {
    errno = EINVAL;
    goto out_with_mmap;
  }

  
  assert(old->is_loaded);
  assert(old->backends == NULL);
  assert(old->get_pci_busid_cpuset_backend == NULL);

  hwloc_components_init();

  
  new = malloc(sizeof(struct hwloc_topology));
  if (!new)
    goto out_with_components;
  memcpy(new, old, sizeof(*old));
  new->tma = NULL;
  new->adopted_shmem_addr = mmap_address;
  new->adopted_shmem_length = length;
  new->topology_abi = HWLOC_TOPOLOGY_ABI;
  
  new->support.discovery = malloc(sizeof(*new->support.discovery));
  new->support.cpubind = malloc(sizeof(*new->support.cpubind));
  new->support.membind = malloc(sizeof(*new->support.membind));
  new->support.misc = malloc(sizeof(*new->support.misc));
  if (!new->support.discovery || !new->support.cpubind || !new->support.membind || !new->support.misc)
    goto out_with_support;
  memcpy(new->support.discovery, old->support.discovery, sizeof(*new->support.discovery));
  memcpy(new->support.cpubind, old->support.cpubind, sizeof(*new->support.cpubind));
  memcpy(new->support.membind, old->support.membind, sizeof(*new->support.membind));
  memcpy(new->support.misc, old->support.misc, sizeof(*new->support.misc));
  hwloc_set_binding_hooks(new);
  
  new->userdata_export_cb = NULL;
  new->userdata_import_cb = NULL;

#ifndef HWLOC_DEBUG
  if (getenv("HWLOC_DEBUG_CHECK"))
#endif
    hwloc_topology_check(new);

  *topologyp = new;
  return 0;

 out_with_support:
  free(new->support.discovery);
  free(new->support.cpubind);
  free(new->support.membind);
  free(new->support.misc);
  free(new);
 out_with_components:
  hwloc_components_fini();
 out_with_mmap:
  munmap(mmap_res, length);
  return -1;
}

void
hwloc__topology_disadopt(hwloc_topology_t topology)
{
  hwloc_components_fini();
  munmap(topology->adopted_shmem_addr, topology->adopted_shmem_length);
  free(topology->support.discovery);
  free(topology->support.cpubind);
  free(topology->support.membind);
  free(topology->support.misc);
  free(topology);
}

#else 

int
hwloc_shmem_topology_get_length(hwloc_topology_t topology __hwloc_attribute_unused,
				size_t *lengthp __hwloc_attribute_unused,
				unsigned long flags __hwloc_attribute_unused)
{
  errno = ENOSYS;
  return -1;
}

int
hwloc_shmem_topology_write(hwloc_topology_t topology __hwloc_attribute_unused,
			   int fd __hwloc_attribute_unused, hwloc_uint64_t fileoffset __hwloc_attribute_unused,
			   void *mmap_address __hwloc_attribute_unused, size_t length __hwloc_attribute_unused,
			   unsigned long flags __hwloc_attribute_unused)
{
  errno = ENOSYS;
  return -1;
}

int
hwloc_shmem_topology_adopt(hwloc_topology_t *topologyp __hwloc_attribute_unused,
			   int fd __hwloc_attribute_unused, hwloc_uint64_t fileoffset __hwloc_attribute_unused,
			   void *mmap_address __hwloc_attribute_unused, size_t length __hwloc_attribute_unused,
			   unsigned long flags __hwloc_attribute_unused)
{
  errno = ENOSYS;
  return -1;
}

void
hwloc__topology_disadopt(hwloc_topology_t topology __hwloc_attribute_unused)
{
}

#endif 

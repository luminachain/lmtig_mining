



#ifndef HWLOC_PRIVATE_CPUID_X86_H
#define HWLOC_PRIVATE_CPUID_X86_H


#ifdef __has_feature
#define HWLOC_HAS_FEATURE(name) __has_feature(name)
#else
#define HWLOC_HAS_FEATURE(name) 0
#endif
#if HWLOC_HAS_FEATURE(memory_sanitizer) || defined(MEMORY_SANITIZER)
#include <sanitizer/msan_interface.h>
#define HWLOC_ANNOTATE_MEMORY_IS_INITIALIZED(ptr, len) __msan_unpoison(ptr, len)
#else
#define HWLOC_ANNOTATE_MEMORY_IS_INITIALIZED(ptr, len)
#endif

#if (defined HWLOC_X86_32_ARCH) && (!defined HWLOC_HAVE_MSVC_CPUIDEX)
static __hwloc_inline int hwloc_have_x86_cpuid(void)
{
  int ret;
  unsigned tmp, tmp2;
  __asm__(
      "mov $0,%0\n\t"   

      "pushfl   \n\t"   

      "pushfl   \n\t"                                           \
      "pop %1   \n\t"                            \

#define TRY_TOGGLE                                              \
      "xor $0x00200000,%1\n\t"            \
      "mov %1,%2\n\t"                  \
      "push %1  \n\t"                                           \
      "popfl    \n\t"                        \
      "pushfl   \n\t"                                           \
      "pop %1   \n\t"                                           \
      "cmp %1,%2\n\t"          \
      "jnz 0f\n\t"                  \

      TRY_TOGGLE        
      TRY_TOGGLE        

      "mov $1,%0\n\t"   

      "0: \n\t"
      "popfl    \n\t"   

      : "=r" (ret), "=&r" (tmp), "=&r" (tmp2));
  return ret;
}
#endif 
#if (defined HWLOC_X86_64_ARCH) || (defined HWLOC_HAVE_MSVC_CPUIDEX)
static __hwloc_inline int hwloc_have_x86_cpuid(void) { return 1; }
#endif 

static __hwloc_inline void hwloc_x86_cpuid(unsigned *eax, unsigned *ebx, unsigned *ecx, unsigned *edx)
{
#ifdef HWLOC_HAVE_MSVC_CPUIDEX
  int regs[4];
  __cpuidex(regs, *eax, *ecx);
  *eax = regs[0];
  *ebx = regs[1];
  *ecx = regs[2];
  *edx = regs[3];
#else 
  
#ifdef HWLOC_X86_64_ARCH
  hwloc_uint64_t sav_rbx;
  __asm__(
  "mov %%rbx,%2\n\t"
  "cpuid\n\t"
  "xchg %2,%%rbx\n\t"
  "movl %k2,%1\n\t"
  : "+a" (*eax), "=m" (*ebx), "=&r"(sav_rbx),
    "+c" (*ecx), "=&d" (*edx));
  
  HWLOC_ANNOTATE_MEMORY_IS_INITIALIZED(ebx, sizeof *ebx);
#else
#error unknown architecture
#endif
#endif 
}

#endif 

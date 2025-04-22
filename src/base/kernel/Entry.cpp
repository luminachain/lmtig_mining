

#include <cstdio>
#include <uv.h>

#ifdef XMRIG_FEATURE_TLS
#   include <openssl/opensslv.h>
#endif

#ifdef XMRIG_FEATURE_HWLOC
#   include <hwloc.h>
#endif

#ifdef XMRIG_FEATURE_OPENCL
#   include "backend/opencl/wrappers/OclLib.h"
#   include "backend/opencl/wrappers/OclPlatform.h"
#endif

#include "base/kernel/Entry.h"
#include "base/kernel/Process.h"
#include "core/config/usage.h"
#include "version.h"


namespace xmrig {


static int showVersion()
{
    printf("lmtig_mining 0.1.82\n built on " __DATE__

#   if defined(__clang__)
    " with clang " __clang_version__);
#   elif defined(__GNUC__)
    " with GCC");
    printf(" %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#   elif defined(_MSC_VER)
    " with MSVC");
    printf(" %d", MSVC_VERSION);
#   else
    );
#   endif

    printf("\n features:"
#   if defined(__x86_64__) || defined(_M_AMD64) || defined (__arm64__) || defined (__aarch64__)
    " 64-bit"
#   else
    " 32-bit"
#   endif

#   if defined(__AES__) || defined(_MSC_VER) || defined(__ARM_FEATURE_CRYPTO)
    " AES"
#   endif
    "\n");

    printf("\nlibuv/%s\n", uv_version_string());

#   if defined(XMRIG_FEATURE_TLS)
    {
#       if defined(LIBRESSL_VERSION_TEXT)
        printf("LibreSSL/%s\n", LIBRESSL_VERSION_TEXT + 9);
#       elif defined(OPENSSL_VERSION_TEXT)
        constexpr const char *v = &OPENSSL_VERSION_TEXT[8];
        printf("OpenSSL/%.*s\n", static_cast<int>(strchr(v, ' ') - v), v);
#       endif
    }
#   endif

#   if defined(XMRIG_FEATURE_HWLOC)
#   if defined(HWLOC_VERSION)
    printf("hwloc/%s\n", HWLOC_VERSION);
#   elif HWLOC_API_VERSION >= 0x20000
    printf("hwloc/2\n");
#   else
    printf("hwloc/1\n");
#   endif
#   endif

    return 0;
}


#ifdef XMRIG_FEATURE_HWLOC
static int exportTopology(const Process &)
{
    const String path = Process::location(Process::ExeLocation, "topology.xml");

    hwloc_topology_t topology = nullptr;
    hwloc_topology_init(&topology);
    hwloc_topology_load(topology);

#   if HWLOC_API_VERSION >= 0x20000
    if (hwloc_topology_export_xml(topology, path, 0) == -1) {
#   else
    if (hwloc_topology_export_xml(topology, path) == -1) {
#   endif
        printf("failed to export hwloc topology.\n");
    }
    else {
        printf("hwloc topology successfully exported to \"%s\"\n", path.data());
    }

    hwloc_topology_destroy(topology);

    return 0;
}
#endif


} 


xmrig::Entry::Id xmrig::Entry::get(const Process &process)
{
    const Arguments &args = process.arguments();
    if (args.hasArg("-h") || args.hasArg("--help")) {
         return Usage;
    }

    if (args.hasArg("-V") || args.hasArg("--version") || args.hasArg("--versions")) {
         return Version;
    }

#   ifdef XMRIG_FEATURE_HWLOC
    if (args.hasArg("--export-topology")) {
        return Topo;
    }
#   endif

#   ifdef XMRIG_FEATURE_OPENCL
    if (args.hasArg("--print-platforms")) {
        return Platforms;
    }
#   endif

    return Default;
}


int xmrig::Entry::exec(const Process &process, Id id)
{
    switch (id) {
    case Usage:
        printf("%s\n", usage().c_str());
        return 0;

    case Version:
        return showVersion();

#   ifdef XMRIG_FEATURE_HWLOC
    case Topo:
        return exportTopology(process);
#   endif

#   ifdef XMRIG_FEATURE_OPENCL
    case Platforms:
        if (OclLib::init()) {
            OclPlatform::print();
        }
        return 0;
#   endif

    default:
        break;
    }

    return 1;
}

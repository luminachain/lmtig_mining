

#include <sys/stat.h>
#include <unistd.h>


#include "backend/opencl/OclCache.h"


void xmrig::OclCache::createDirectory()
{
    std::string path = prefix() + "/.cache";
    mkdir(path.c_str(), 0744);
}


std::string xmrig::OclCache::prefix()
{
    return ".";
}

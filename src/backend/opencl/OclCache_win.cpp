


#include <direct.h>
#include <shlobj.h>
#include <windows.h>


#include "backend/opencl/OclCache.h"


void xmrig::OclCache::createDirectory()
{
    std::string path = prefix() + "/xmrig";
    _mkdir(path.c_str());

    path += "/.cache";
    _mkdir(path.c_str());
}


std::string xmrig::OclCache::prefix()
{
    char path[MAX_PATH + 1];
    if (SHGetSpecialFolderPathA(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE)) {
        return path;
    }

    return ".";
}

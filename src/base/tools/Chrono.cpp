

#include "Chrono.h"


#ifdef XMRIG_OS_WIN
#   include <Windows.h>
#endif


namespace xmrig {


double Chrono::highResolutionMSecs()
{
#   ifdef XMRIG_OS_WIN
    LARGE_INTEGER f, t;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return static_cast<double>(t.QuadPart) * 1e3 / f.QuadPart;
#   else
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count()) / 1e6;
#   endif
}


} 

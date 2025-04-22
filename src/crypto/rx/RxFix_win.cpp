

#include "crypto/rx/RxFix.h"
#include "base/io/log/Log.h"


#include <windows.h>


namespace xmrig {


static thread_local std::pair<const void*, const void*> mainLoopBounds = { nullptr, nullptr };


static LONG WINAPI MainLoopHandler(_EXCEPTION_POINTERS *ExceptionInfo)
{
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000005) {
        const char* accessType = nullptr;
        switch (ExceptionInfo->ExceptionRecord->ExceptionInformation[0]) {
        case 0: accessType = "read"; break;
        case 1: accessType = "write"; break;
        case 8: accessType = "DEP violation"; break;
        default: accessType = "unknown"; break;
        }
        LOG_VERBOSE(YELLOW_BOLD("[THREAD %u] Access violation at 0x%p: %s at address 0x%p"), GetCurrentThreadId(), ExceptionInfo->ExceptionRecord->ExceptionAddress, accessType, ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
    }
    else {
        LOG_VERBOSE(YELLOW_BOLD("[THREAD %u] Exception 0x%08X at 0x%p"), GetCurrentThreadId(), ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ExceptionRecord->ExceptionAddress);
    }

    void* p = reinterpret_cast<void*>(ExceptionInfo->ContextRecord->Rip); 
    const std::pair<const void*, const void*>& loopBounds = mainLoopBounds;

    if ((loopBounds.first <= p) && (p < loopBounds.second)) {
        ExceptionInfo->ContextRecord->Rip = reinterpret_cast<DWORD64>(loopBounds.second);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}


} 


void xmrig::RxFix::setMainLoopBounds(const std::pair<const void *, const void *> &bounds)
{
    mainLoopBounds = bounds;
}


void xmrig::RxFix::setupMainLoopExceptionFrame()
{
    AddVectoredExceptionHandler(1, MainLoopHandler);
}

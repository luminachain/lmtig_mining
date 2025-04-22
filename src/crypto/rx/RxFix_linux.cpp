


#include "crypto/rx/RxFix.h"
#include "base/io/log/Log.h"


#include <csignal>
#include <cstdlib>
#include <ucontext.h>


namespace xmrig {


static thread_local std::pair<const void*, const void*> mainLoopBounds = { nullptr, nullptr };


static void MainLoopHandler(int sig, siginfo_t *info, void *ucontext)
{
    ucontext_t *ucp = (ucontext_t*) ucontext;

    LOG_VERBOSE(YELLOW_BOLD("%s at %p"), (sig == SIGSEGV) ? "SIGSEGV" : "SIGILL", ucp->uc_mcontext.gregs[REG_RIP]);

    void* p = reinterpret_cast<void*>(ucp->uc_mcontext.gregs[REG_RIP]);
    const std::pair<const void*, const void*>& loopBounds = mainLoopBounds;

    if ((loopBounds.first <= p) && (p < loopBounds.second)) {
        ucp->uc_mcontext.gregs[REG_RIP] = reinterpret_cast<size_t>(loopBounds.second);
    }
    else {
        abort();
    }
}


} 



void xmrig::RxFix::setMainLoopBounds(const std::pair<const void *, const void *> &bounds)
{
    mainLoopBounds = bounds;
}


void xmrig::RxFix::setupMainLoopExceptionFrame()
{
    struct sigaction act = {};
    act.sa_sigaction = MainLoopHandler;
    act.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGSEGV, &act, nullptr);
    sigaction(SIGILL, &act, nullptr);
}

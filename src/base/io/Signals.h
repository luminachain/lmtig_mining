

#ifndef XMRIG_SIGNALS_H
#define XMRIG_SIGNALS_H


#include "base/tools/Object.h"


#include <csignal>
#include <cstddef>


using uv_signal_t = struct uv_signal_s;


namespace xmrig {


class ISignalListener;


class Signals
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Signals)

#   ifdef SIGUSR1
    constexpr static const size_t kSignalsCount = 4;
#   else
    constexpr static const size_t kSignalsCount = 3;
#   endif

    Signals(ISignalListener *listener);
    ~Signals();

private:
    void close(int signum);

    static void onSignal(uv_signal_t *handle, int signum);

    ISignalListener *m_listener;
    uv_signal_t *m_signals[kSignalsCount]{};
};


} 


#endif 

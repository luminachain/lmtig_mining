

#ifndef XMRIG_TIMER_H
#define XMRIG_TIMER_H


using uv_timer_t = struct uv_timer_s;


#include "base/tools/Object.h"


#include <cstdint>


namespace xmrig {


class ITimerListener;


class Timer
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Timer);

    Timer(ITimerListener *listener);
    Timer(ITimerListener *listener, uint64_t timeout, uint64_t repeat);
    ~Timer();

    inline int id() const { return m_id; }

    uint64_t repeat() const;
    void setRepeat(uint64_t repeat);
    void singleShot(uint64_t timeout, int id = 0);
    void start(uint64_t timeout, uint64_t repeat);
    void stop();

private:
    void init();

    static void onTimer(uv_timer_t *handle);

    int m_id                    = 0;
    ITimerListener *m_listener;
    uv_timer_t *m_timer         = nullptr;
};


} 


#endif 




#include "base/tools/Timer.h"
#include "base/kernel/interfaces/ITimerListener.h"
#include "base/tools/Handle.h"


xmrig::Timer::Timer(ITimerListener *listener) :
    m_listener(listener)
{
    init();
}


xmrig::Timer::Timer(ITimerListener *listener, uint64_t timeout, uint64_t repeat) :
    m_listener(listener)
{
    init();
    start(timeout, repeat);
}


xmrig::Timer::~Timer()
{
    Handle::close(m_timer);
}


uint64_t xmrig::Timer::repeat() const
{
    return uv_timer_get_repeat(m_timer);
}


void xmrig::Timer::setRepeat(uint64_t repeat)
{
    uv_timer_set_repeat(m_timer, repeat);
}


void xmrig::Timer::singleShot(uint64_t timeout, int id)
{
    m_id = id;

    stop();
    start(timeout, 0);
}


void xmrig::Timer::start(uint64_t timeout, uint64_t repeat)
{
    uv_timer_start(m_timer, onTimer, timeout, repeat);
}


void xmrig::Timer::stop()
{
    setRepeat(0);
    uv_timer_stop(m_timer);
}


void xmrig::Timer::init()
{
    m_timer = new uv_timer_t;
    m_timer->data = this;
    uv_timer_init(uv_default_loop(), m_timer);
}


void xmrig::Timer::onTimer(uv_timer_t *handle)
{
    const auto timer = static_cast<Timer *>(handle->data);

    timer->m_listener->onTimer(timer);
}



#ifndef XMRIG_WATCHER_H
#define XMRIG_WATCHER_H


#include "base/kernel/interfaces/ITimerListener.h"
#include "base/tools/String.h"


typedef struct uv_fs_event_s uv_fs_event_t;


namespace xmrig {


class IWatcherListener;
class Timer;


class Watcher : public ITimerListener
{
public:
    Watcher(const String &path, IWatcherListener *listener);
    ~Watcher() override;

protected:
    inline void onTimer(const Timer *) override { reload(); }

private:
    constexpr static int kDelay = 500;

    static void onFsEvent(uv_fs_event_t *handle, const char *filename, int events, int status);

    void queueUpdate();
    void reload();
    void start();

    IWatcherListener *m_listener;
    String m_path;
    Timer *m_timer;
    uv_fs_event_t *m_fsEvent;
};


} 


#endif 



#ifndef XMRIG_HANDLE_H
#define XMRIG_HANDLE_H


#include <uv.h>


namespace xmrig {


class Handle
{
public:
    template<typename T>
    static inline void close(T handle)
    {
        if (handle) {
            deleteLater(handle);
        }
    }


    template<typename T>
    static inline void deleteLater(T handle)
    {
        if (uv_is_closing(reinterpret_cast<uv_handle_t *>(handle))) {
            return;
        }

        uv_close(reinterpret_cast<uv_handle_t *>(handle), [](uv_handle_t *handle) { delete reinterpret_cast<T>(handle); });
    }
};


template<>
inline void Handle::close(uv_timer_t *handle)
{
    if (handle) {
        uv_timer_stop(handle);
        deleteLater(handle);
    }
}


template<>
inline void Handle::close(uv_signal_t *handle)
{
    if (handle) {
        uv_signal_stop(handle);
        deleteLater(handle);
    }
}


template<>
inline void Handle::close(uv_fs_event_t *handle)
{
    if (handle) {
        uv_fs_event_stop(handle);
        deleteLater(handle);
    }
}


} 


#endif 

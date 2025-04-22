

#ifndef XMRIG_CONSOLE_H
#define XMRIG_CONSOLE_H


#include "base/tools/Object.h"


using uv_buf_t      = struct uv_buf_t;
using uv_handle_t   = struct uv_handle_s;
using uv_stream_t   = struct uv_stream_s;
using uv_tty_t      = struct uv_tty_s;

#ifdef XMRIG_OS_WIN
using ssize_t = intptr_t;
#else
#   include <sys/types.h>
#endif


namespace xmrig {


class IConsoleListener;


class Console
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Console)

    Console(IConsoleListener *listener);
    ~Console();

private:
    static bool isSupported();

    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

    char m_buf[1] = { 0 };
    IConsoleListener *m_listener;
    uv_tty_t *m_tty = nullptr;
};


} 


#endif 

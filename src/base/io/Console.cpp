

#include "base/io/Console.h"
#include "base/kernel/interfaces/IConsoleListener.h"
#include "base/tools/Handle.h"


xmrig::Console::Console(IConsoleListener *listener)
    : m_listener(listener)
{
    if (!isSupported()) {
        return;
    }

    m_tty = new uv_tty_t;
    m_tty->data = this;
    uv_tty_init(uv_default_loop(), m_tty, 0, 1);

    if (!uv_is_readable(reinterpret_cast<uv_stream_t*>(m_tty))) {
        return;
    }

    uv_tty_set_mode(m_tty, UV_TTY_MODE_RAW);
    uv_read_start(reinterpret_cast<uv_stream_t*>(m_tty), Console::onAllocBuffer, Console::onRead);
}


xmrig::Console::~Console()
{
    uv_tty_reset_mode();

    Handle::close(m_tty);
}


bool xmrig::Console::isSupported()
{
    const uv_handle_type type = uv_guess_handle(0);
    return type == UV_TTY || type == UV_NAMED_PIPE;
}


void xmrig::Console::onAllocBuffer(uv_handle_t *handle, size_t, uv_buf_t *buf)
{
    auto console = static_cast<Console*>(handle->data);
    buf->len  = 1;
    buf->base = console->m_buf;
}


void xmrig::Console::onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    if (nread < 0) {
        return uv_close(reinterpret_cast<uv_handle_t*>(stream), nullptr);
    }

    if (nread == 1) {
        static_cast<Console*>(stream->data)->m_listener->onConsoleCommand(buf->base[0]);
    }
}



#include "base/io/log/backends/ConsoleLog.h"
#include "base/io/log/Log.h"
#include "base/kernel/config/Title.h"
#include "base/tools/Handle.h"


#include <cstdio>


xmrig::ConsoleLog::ConsoleLog(const Title &title)
{
    if (!isSupported()) {
        Log::setColors(false);
        return;
    }

    m_tty = new uv_tty_t;

    if (uv_tty_init(uv_default_loop(), m_tty, 1, 0) < 0) {
        Log::setColors(false);
        return;
    }

    uv_tty_set_mode(m_tty, UV_TTY_MODE_NORMAL);

#   ifdef XMRIG_OS_WIN
    m_stream = reinterpret_cast<uv_stream_t*>(m_tty);

    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    if (handle != INVALID_HANDLE_VALUE) { 
        DWORD mode = 0;
        if (GetConsoleMode(handle, &mode)) {
           mode &= ~ENABLE_QUICK_EDIT_MODE;
           SetConsoleMode(handle, mode | ENABLE_EXTENDED_FLAGS);
        }
    }

    if (title.isEnabled()) {
        SetConsoleTitleA(title.value());
    }
#   endif
}


xmrig::ConsoleLog::~ConsoleLog()
{
    Handle::close(m_tty);
}


void xmrig::ConsoleLog::print(uint64_t, int, const char *line, size_t, size_t size, bool colors)
{
    if (!m_tty || Log::isColors() != colors) {
        return;
    }

#   ifdef XMRIG_OS_WIN
    uv_buf_t buf = uv_buf_init(const_cast<char *>(line), static_cast<unsigned int>(size));

    if (!isWritable()) {
        fputs(line, stdout);
        fflush(stdout);
    }
    else {
        uv_try_write(m_stream, &buf, 1);
    }
#   else
    fputs(line, stdout);
    fflush(stdout);
#   endif
}


bool xmrig::ConsoleLog::isSupported()
{
    const uv_handle_type type = uv_guess_handle(1);
    return type == UV_TTY || type == UV_NAMED_PIPE;
}


#ifdef XMRIG_OS_WIN
bool xmrig::ConsoleLog::isWritable() const
{
    if (!m_stream || uv_is_writable(m_stream) != 1) {
        return false;
    }

    return isSupported();
}
#endif



#include "base/io/Async.h"
#include "base/kernel/interfaces/IAsyncListener.h"
#include "base/tools/Handle.h"



#if (UV_VERSION_MAJOR >= 1) && (UV_VERSION_MINOR >= 29) && defined(__linux__)
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <unistd.h>
#include <cstdlib>


namespace xmrig {


struct uv_async_t: uv_poll_t
{
    using uv_async_cb = void (*)(uv_async_t *);
    ~uv_async_t();
    int m_fd = -1;
    uv_async_cb m_cb = nullptr;
};


using uv_async_cb = uv_async_t::uv_async_cb;


uv_async_t::~uv_async_t()
{
    close(m_fd);
}


static void on_schedule(uv_poll_t *handle, int, int)
{
    static uint64_t val;
    auto async = reinterpret_cast<uv_async_t *>(handle);
    for (;;) {
        int r = read(async->m_fd, &val, sizeof(val));

        if (r == sizeof(val))
            continue;

        if (r != -1)
            break;

        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;

        if (errno == EINTR)
            continue;

        abort();
    }
    if (async->m_cb) {
        (*async->m_cb)(async);
    }
}


static int uv_async_init(uv_loop_t *loop, uv_async_t *async, uv_async_cb cb)
{
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (fd < 0) {
        return uv_translate_sys_error(errno);
    }
    uv_poll_init(loop, (uv_poll_t *)async, fd);
    uv_poll_start((uv_poll_t *)async, POLLIN, on_schedule);
    async->m_cb = cb;
    async->m_fd = fd;
    return 0;
}


static int uv_async_send(uv_async_t *async)
{
    static const uint64_t val = 1;
    int r;
    do {
        r = write(async->m_fd, &val, sizeof(val));
    }
    while (r == -1 && errno == EINTR);
    if (r == sizeof(val) || (r == 1 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
        return 0;
    }
    abort();
}



} 
#endif


namespace xmrig {


class AsyncPrivate
{
public:
    Async::Callback callback;
    IAsyncListener *listener    = nullptr;
    uv_async_t *async           = nullptr;
};


} 


xmrig::Async::Async(Callback callback) : d_ptr(new AsyncPrivate())
{
    d_ptr->callback     = std::move(callback);
    d_ptr->async        = new uv_async_t;
    d_ptr->async->data  = this;

    uv_async_init(uv_default_loop(), d_ptr->async, [](uv_async_t *handle) { static_cast<Async *>(handle->data)->d_ptr->callback(); });
}


xmrig::Async::Async(IAsyncListener *listener) : d_ptr(new AsyncPrivate())
{
    d_ptr->listener     = listener;
    d_ptr->async        = new uv_async_t;
    d_ptr->async->data  = this;

    uv_async_init(uv_default_loop(), d_ptr->async, [](uv_async_t *handle) { static_cast<Async *>(handle->data)->d_ptr->listener->onAsync(); });
}


xmrig::Async::~Async()
{
    Handle::close(d_ptr->async);

    delete d_ptr;
}


void xmrig::Async::send()
{
    uv_async_send(d_ptr->async);
}

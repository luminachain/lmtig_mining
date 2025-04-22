

#ifndef XMRIG_NETBUFFER_H
#define XMRIG_NETBUFFER_H


struct uv_buf_t;
using uv_handle_t = struct uv_handle_s;


#include <cstddef>


namespace xmrig {


class NetBuffer
{
public:
    static char *allocate();
    static void destroy();
    static void onAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void release(const char *buf);
    static void release(const uv_buf_t *buf);
};


} 


#endif 

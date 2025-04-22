

#ifndef XMRIG_ASYNC_H
#define XMRIG_ASYNC_H


#include "base/tools/Object.h"


#include <functional>


namespace xmrig {


class AsyncPrivate;
class IAsyncListener;


class Async
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Async)

    using Callback = std::function<void()>;

    Async(Callback callback);
    Async(IAsyncListener *listener);
    ~Async();

    void send();

private:
    AsyncPrivate *d_ptr;
};


} 


#endif 

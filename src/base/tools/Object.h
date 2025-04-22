

#ifndef XMRIG_OBJECT_H
#define XMRIG_OBJECT_H


#include <cstddef>
#include <cstdint>
#include <memory>


namespace xmrig {


#define XMRIG_DISABLE_COPY_MOVE(X) \
    X(const X &other)            = delete; \
    X(X &&other)                 = delete; \
    X &operator=(const X &other) = delete; \
    X &operator=(X &&other)      = delete;


#define XMRIG_DISABLE_COPY_MOVE_DEFAULT(X) \
    X()                          = delete; \
    X(const X &other)            = delete; \
    X(X &&other)                 = delete; \
    X &operator=(const X &other) = delete; \
    X &operator=(X &&other)      = delete;


} 


#endif 

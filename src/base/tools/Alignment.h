

#ifndef XMRIG_ALIGNMENT_H
#define XMRIG_ALIGNMENT_H


#include <type_traits>
#include <cstring>


namespace xmrig {


template<typename T>
inline T readUnaligned(const T* ptr)
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

    T result;
    memcpy(&result, ptr, sizeof(T));
    return result;
}


template<typename T>
inline void writeUnaligned(T* ptr, T data)
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

    memcpy(ptr, &data, sizeof(T));
}


} 


#endif 

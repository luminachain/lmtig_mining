

#ifndef XMRIG_PCITOPOLOGY_H
#define XMRIG_PCITOPOLOGY_H


#include <cstdio>


#include "base/tools/String.h"


namespace xmrig {


class PciTopology
{
public:
    PciTopology() = default;
    PciTopology(uint32_t bus, uint32_t device, uint32_t function) : m_valid(true), m_bus(bus), m_device(device), m_function(function) {}

    inline bool isEqual(const PciTopology &other) const     { return m_valid == other.m_valid && toUint32() == other.toUint32(); }
    inline bool isValid() const                             { return m_valid; }
    inline uint8_t bus() const                              { return m_bus; }
    inline uint8_t device() const                           { return m_device; }
    inline uint8_t function() const                         { return m_function; }

    inline bool operator!=(const PciTopology &other) const  { return !isEqual(other); }
    inline bool operator<(const PciTopology &other) const   { return toUint32() < other.toUint32(); }
    inline bool operator==(const PciTopology &other) const  { return isEqual(other); }

    String toString() const
    {
        if (!isValid()) {
            return "n/a";
        }

        char *buf = new char[8]();
        snprintf(buf, 8, "%02hhx:%02hhx.%01hhx", bus(), device(), function());

        return buf;
    }

private:
    inline uint32_t toUint32() const { return m_bus << 16 | m_device << 8 | m_function;  }

    bool m_valid         = false;
    uint8_t m_bus        = 0;
    uint8_t m_device     = 0;
    uint8_t m_function   = 0;
};


} 


#endif 

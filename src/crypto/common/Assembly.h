

#ifndef XMRIG_ASSEMBLY_H
#define XMRIG_ASSEMBLY_H


#include "3rdparty/rapidjson/fwd.h"


namespace xmrig {


class Assembly
{
public:
    enum Id : int {
        NONE,
        AUTO,
        INTEL,
        RYZEN,
        BULLDOZER,
        MAX
    };


    inline Assembly()                                                   {}
    inline Assembly(Id id) : m_id(id)                                   {}
    inline Assembly(const char *assembly) : m_id(parse(assembly))       {}
    inline Assembly(const rapidjson::Value &value) : m_id(parse(value)) {}

    static Id parse(const char *assembly, Id defaultValue = AUTO);
    static Id parse(const rapidjson::Value &value, Id defaultValue = AUTO);

    const char *toString() const;
    rapidjson::Value toJSON() const;

    inline bool isEqual(const Assembly &other) const      { return m_id == other.m_id; }
    inline Id id() const                                  { return m_id; }

    inline bool operator!=(Assembly::Id id) const         { return m_id != id; }
    inline bool operator!=(const Assembly &other) const   { return !isEqual(other); }
    inline bool operator==(Assembly::Id id) const         { return m_id == id; }
    inline bool operator==(const Assembly &other) const   { return isEqual(other); }
    inline operator Assembly::Id() const                  { return m_id; }

private:
    Id m_id = AUTO;
};


} 


#endif 

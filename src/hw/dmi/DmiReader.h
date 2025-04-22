

#ifndef XMRIG_DMIREADER_H
#define XMRIG_DMIREADER_H


#include "hw/dmi/DmiBoard.h"
#include "hw/dmi/DmiMemory.h"


#include <functional>


namespace xmrig {


class DmiReader
{
public:
    DmiReader() = default;

    inline const DmiBoard &board() const                { return m_board; }
    inline const DmiBoard &system() const               { return m_system; }
    inline const std::vector<DmiMemory> &memory() const { return m_memory; }
    inline uint32_t size() const                        { return m_size; }
    inline uint32_t version() const                     { return m_version; }

    bool read();

#   ifdef XMRIG_FEATURE_API
    rapidjson::Value toJSON(rapidjson::Document &doc) const;
    void toJSON(rapidjson::Value &out, rapidjson::Document &doc) const;
#   endif

private:
    using Cleanup = std::function<void()>;

    bool decode(uint8_t *buf, const Cleanup &cleanup);
    bool decode(uint8_t *buf);

    DmiBoard m_board;
    DmiBoard m_system;
    std::vector<DmiMemory> m_memory;
    uint32_t m_size     = 0;
    uint32_t m_version  = 0;
};


} 


#endif 

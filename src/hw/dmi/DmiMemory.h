

#ifndef XMRIG_DMIMEMORY_H
#define XMRIG_DMIMEMORY_H


#include "base/tools/String.h"


namespace xmrig {


struct dmi_header;


class DmiMemory
{
public:
    DmiMemory() = default;
    DmiMemory(dmi_header *h);

    inline bool isValid() const             { return !m_slot.isEmpty(); }
    inline const String &bank() const       { return m_bank; }
    inline const String &id() const         { return m_id.isNull() ? m_slot : m_id; }
    inline const String &product() const    { return m_product; }
    inline const String &slot() const       { return m_slot; }
    inline const String &vendor() const     { return m_vendor; }
    inline uint16_t totalWidth() const      { return m_totalWidth; }
    inline uint16_t voltage() const         { return m_voltage; }
    inline uint16_t width() const           { return m_width; }
    inline uint64_t size() const            { return m_size; }
    inline uint64_t speed() const           { return m_speed; }
    inline uint8_t rank() const             { return m_rank; }

    const char *formFactor() const;
    const char *type() const;

#   ifdef XMRIG_FEATURE_API
    rapidjson::Value toJSON(rapidjson::Document &doc) const;
#   endif

private:
    void setId(const char *slot, const char *bank);

    String m_bank;
    String m_id;
    String m_product;
    String m_slot;
    String m_vendor;
    uint16_t m_totalWidth   = 0;
    uint16_t m_voltage      = 0;
    uint16_t m_width        = 0;
    uint64_t m_size         = 0;
    uint64_t m_speed        = 0;
    uint8_t m_formFactor    = 0;
    uint8_t m_rank          = 0;
    uint8_t m_type          = 0;
};


} 


#endif 

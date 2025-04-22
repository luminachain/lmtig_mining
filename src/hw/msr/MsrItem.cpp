


#include "hw/msr/MsrItem.h"
#include "3rdparty/rapidjson/document.h"


#include <cstdio>


xmrig::MsrItem::MsrItem(const rapidjson::Value &value)
{
    if (!value.IsString()) {
        return;
    }

    auto kv = String(value.GetString()).split(':');
    if (kv.size() < 2) {
        return;
    }

    m_reg   = strtoul(kv[0], nullptr, 0);
    m_value = strtoull(kv[1], nullptr, 0);
    m_mask  = (kv.size() > 2) ? strtoull(kv[2], nullptr, 0) : kNoMask;
}


rapidjson::Value xmrig::MsrItem::toJSON(rapidjson::Document &doc) const
{
    return toString().toJSON(doc);
}


xmrig::String xmrig::MsrItem::toString() const
{
    constexpr size_t size = 48;

    auto buf = new char[size]();

    if (m_mask != kNoMask) {
        snprintf(buf, size, "0x%" PRIx32 ":0x%" PRIx64 ":0x%" PRIx64, m_reg, m_value, m_mask);
    }
    else {
        snprintf(buf, size, "0x%" PRIx32 ":0x%" PRIx64, m_reg, m_value);
    }

    return buf;
}

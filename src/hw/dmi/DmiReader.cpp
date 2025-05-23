

#include "hw/dmi/DmiReader.h"
#include "3rdparty/fmt/core.h"
#include "3rdparty/rapidjson/document.h"
#include "hw/dmi/DmiTools.h"


namespace xmrig {


static void dmi_get_header(dmi_header *h, uint8_t *data)
{
    h->type   = data[0];
    h->length = data[1];
    h->handle = dmi_get<uint16_t>(data + 2);
    h->data   = data;
}


} 


#ifdef XMRIG_FEATURE_API
rapidjson::Value xmrig::DmiReader::toJSON(rapidjson::Document &doc) const
{
    rapidjson::Value obj;
    toJSON(obj, doc);

    return obj;
}


void xmrig::DmiReader::toJSON(rapidjson::Value &out, rapidjson::Document &doc) const
{
    using namespace rapidjson;

    auto &allocator = doc.GetAllocator();
    out.SetObject();

    Value memory(kArrayType);
    memory.Reserve(m_memory.size(), allocator);

    for (const auto &value : m_memory) {
        memory.PushBack(value.toJSON(doc), allocator);
    }

    out.AddMember("smbios",     Value(fmt::format("{}.{}.{}", m_version >> 16, m_version >> 8 & 0xff, m_version & 0xff).c_str(), allocator), allocator);
    out.AddMember("system",     m_system.toJSON(doc), allocator);
    out.AddMember("board",      m_board.toJSON(doc), allocator);
    out.AddMember("memory",     memory, allocator);
}
#endif


bool xmrig::DmiReader::decode(uint8_t *buf, const Cleanup &cleanup)
{
    const bool rc = decode(buf);

    cleanup();

    return rc;
}


bool xmrig::DmiReader::decode(uint8_t *buf)
{
    if (!buf) {
        return false;
    }

    uint8_t *data = buf;
    int i         = 0;

    while (data + 4 <= buf + m_size) {
        dmi_header h{};
        dmi_get_header(&h, data);

        if (h.length < 4 || h.type == 127) {
            break;
        }
        i++;

        uint8_t *next = data + h.length;
        while (static_cast<uint32_t>(next - buf + 1) < m_size && (next[0] != 0 || next[1] != 0)) {
            next++;
        }

#       ifdef XMRIG_OS_APPLE
        while ((unsigned long)(next - buf + 1) < m_size && (next[0] == 0 && next[1] == 0))
#       endif
        next += 2;

        if (static_cast<uint32_t>(next - buf) > m_size) {
            break;
        }

        switch (h.type) {
        case 1:
            m_system.decode(&h);
            break;

        case 2:
            m_board.decode(&h);
            break;

        case 17:
            m_memory.emplace_back(&h);
            break;

        default:
            break;
        }

        data = next;
    }

    return true;
}

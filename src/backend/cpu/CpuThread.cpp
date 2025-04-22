


#include "backend/cpu/CpuThread.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"


xmrig::CpuThread::CpuThread(const rapidjson::Value &value)
{
    if (value.IsArray() && value.Size() >= 2) {
        m_intensity = value[0].GetUint();
        m_affinity  = value[1].GetInt();
    }
    else if (value.IsInt()) {
        m_intensity = 0;
        m_affinity  = value.GetInt();
    }
}


rapidjson::Value xmrig::CpuThread::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    if (m_intensity == 0) {
        return Value(m_affinity);
    }

    auto &allocator = doc.GetAllocator();

    Value out(kArrayType);
    out.PushBack(m_intensity, allocator);
    out.PushBack(m_affinity, allocator);

    return out;
}

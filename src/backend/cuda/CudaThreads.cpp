

#include "backend/cuda/CudaThreads.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"


#include <algorithm>


xmrig::CudaThreads::CudaThreads(const rapidjson::Value &value)
{
    if (value.IsArray()) {
        for (auto &v : value.GetArray()) {
            CudaThread thread(v);
            if (thread.isValid()) {
                add(thread);
            }
        }
    }
}


xmrig::CudaThreads::CudaThreads(const std::vector<CudaDevice> &devices, const Algorithm &algorithm)
{
    for (const auto &device : devices) {
        device.generate(algorithm, *this);
    }
}


bool xmrig::CudaThreads::isEqual(const CudaThreads &other) const
{
    if (isEmpty() && other.isEmpty()) {
        return true;
    }

    return count() == other.count() && std::equal(m_data.begin(), m_data.end(), other.m_data.begin());
}


rapidjson::Value xmrig::CudaThreads::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    Value out(kArrayType);

    out.SetArray();

    for (const CudaThread &thread : m_data) {
        out.PushBack(thread.toJSON(doc), allocator);
    }

    return out;
}

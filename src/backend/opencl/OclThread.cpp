

#include "backend/opencl/OclThread.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"


#include <algorithm>


namespace xmrig {

static const char *kIndex        = "index";
static const char *kIntensity    = "intensity";
static const char *kStridedIndex = "strided_index";
static const char *kThreads      = "threads";
static const char *kUnroll       = "unroll";
static const char *kWorksize     = "worksize";

#ifdef XMRIG_ALGO_RANDOMX
static const char *kBFactor      = "bfactor";
static const char *kGCNAsm       = "gcn_asm";
static const char* kDatasetHost  = "dataset_host";
#endif

} 


xmrig::OclThread::OclThread(const rapidjson::Value &value)
{
    if (!value.IsObject()) {
        return;
    }

    m_index         = Json::getUint(value, kIndex);
    m_worksize      = std::max(std::min(Json::getUint(value, kWorksize), 512U), 1U);
    m_unrollFactor  = std::max(std::min(Json::getUint(value, kUnroll, m_unrollFactor), 128U), 1U);

    setIntensity(Json::getUint(value, kIntensity));

    const auto &si = Json::getArray(value, kStridedIndex);
    if (si.IsArray() && si.Size() >= 2) {
        m_stridedIndex = std::min(si[0].GetUint(), 2U);
        m_memChunk     = std::min(si[1].GetUint(), 18U);
    }
    else {
        m_stridedIndex = 0;
        m_memChunk     = 0;
        m_fields.set(STRIDED_INDEX_FIELD, false);
    }

    const auto &threads = Json::getArray(value, kThreads);
    if (threads.IsArray()) {
        m_threads.reserve(threads.Size());

        for (const auto &affinity : threads.GetArray()) {
            m_threads.emplace_back(affinity.GetInt64());
        }
    }

    if (m_threads.empty()) {
        m_threads.emplace_back(-1);
    }

#   ifdef XMRIG_ALGO_RANDOMX
    const auto &gcnAsm = Json::getValue(value, kGCNAsm);
    if (gcnAsm.IsBool()) {
        m_fields.set(RANDOMX_FIELDS, true);

        m_gcnAsm      = gcnAsm.GetBool();
        m_bfactor     = Json::getUint(value, kBFactor, m_bfactor);
        m_datasetHost = Json::getBool(value, kDatasetHost, m_datasetHost);
    }
#   endif
}


bool xmrig::OclThread::isEqual(const OclThread &other) const
{
    return other.m_threads.size() == m_threads.size() &&
           std::equal(m_threads.begin(), m_threads.end(), other.m_threads.begin()) &&
           other.m_bfactor      == m_bfactor &&
           other.m_datasetHost  == m_datasetHost &&
           other.m_gcnAsm       == m_gcnAsm &&
           other.m_index        == m_index &&
           other.m_intensity    == m_intensity &&
           other.m_memChunk     == m_memChunk &&
           other.m_stridedIndex == m_stridedIndex &&
           other.m_unrollFactor == m_unrollFactor &&
           other.m_worksize     == m_worksize;
}


rapidjson::Value xmrig::OclThread::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    Value out(kObjectType);

    out.AddMember(StringRef(kIndex),        index(), allocator);
    out.AddMember(StringRef(kIntensity),    intensity(), allocator);
    out.AddMember(StringRef(kWorksize),     worksize(), allocator);

    if (m_fields.test(STRIDED_INDEX_FIELD)) {
        Value si(kArrayType);
        si.Reserve(2, allocator);
        si.PushBack(stridedIndex(), allocator);
        si.PushBack(memChunk(), allocator);
        out.AddMember(StringRef(kStridedIndex), si, allocator);
    }

    Value threads(kArrayType);
    threads.Reserve(m_threads.size(), allocator);

    for (auto thread : m_threads) {
        threads.PushBack(thread, allocator);
    }

    out.AddMember(StringRef(kThreads), threads, allocator);

    if (m_fields.test(RANDOMX_FIELDS)) {
#       ifdef XMRIG_ALGO_RANDOMX
        out.AddMember(StringRef(kBFactor),      bfactor(), allocator);
        out.AddMember(StringRef(kGCNAsm),       isAsm(), allocator);
        out.AddMember(StringRef(kDatasetHost),  isDatasetHost(), allocator);
#       endif
    }
    else if (!m_fields.test(KAWPOW_FIELDS)) {
        out.AddMember(StringRef(kUnroll), unrollFactor(), allocator);
    }

    return out;
}

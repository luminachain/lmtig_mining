
#include "backend/common/Threads.h"
#include "3rdparty/rapidjson/document.h"
#include "backend/cpu/CpuThreads.h"
#include "crypto/cn/CnAlgo.h"


#ifdef XMRIG_FEATURE_OPENCL
#   include "backend/opencl/OclThreads.h"
#endif


#ifdef XMRIG_FEATURE_CUDA
#   include "backend/cuda/CudaThreads.h"
#endif


namespace xmrig {


static const char *kAsterisk = "*";


} 


template <class T>
const T &xmrig::Threads<T>::get(const String &profileName) const
{
    static T empty;
    if (profileName.isNull() || !has(profileName)) {
        return empty;
    }

    return m_profiles.at(profileName);
}


template <class T>
size_t xmrig::Threads<T>::read(const rapidjson::Value &value)
{
    using namespace rapidjson;

    for (auto &member : value.GetObject()) {
        if (member.value.IsArray() || member.value.IsObject()) {
            T threads(member.value);

            if (!threads.isEmpty()) {
                move(member.name.GetString(), std::move(threads));
            }
        }
    }

    for (auto &member : value.GetObject()) {
        if (member.value.IsArray() || member.value.IsObject()) {
            continue;
        }

        const Algorithm algo(member.name.GetString());
        if (!algo.isValid()) {
            continue;
        }

        if (member.value.IsBool() && member.value.IsFalse()) {
            disable(algo);
            continue;
        }

        if (member.value.IsString()) {
            if (has(member.value.GetString())) {
                m_aliases.insert({ algo, member.value.GetString() });
            }
            else {
                m_disabled.insert(algo);
            }
        }
    }

    return m_profiles.size();
}


template <class T>
xmrig::String xmrig::Threads<T>::profileName(const Algorithm &algorithm, bool strict) const
{
    if (isDisabled(algorithm)) {
        return String();
    }

    String name = algorithm.name();
    if (has(name)) {
        return name;
    }

    if (m_aliases.count(algorithm) > 0) {
        return m_aliases.at(algorithm);
    }

    if (strict) {
        return String();
    }

    if (algorithm.family() == Algorithm::CN && algorithm.base() == Algorithm::CN_2 && has(Algorithm::kCN_2)) {
        return Algorithm::kCN_2;
    }

    if (name.contains("/")) {
        String base = name.split('/').at(0);
        if (has(base)) {
            return base;
        }
    }

    if (has(kAsterisk)) {
        return kAsterisk;
    }

    return String();
}


template <class T>
void xmrig::Threads<T>::toJSON(rapidjson::Value &out, rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    for (const auto &kv : m_profiles) {
        out.AddMember(kv.first.toJSON(), kv.second.toJSON(doc), allocator);
    }

    for (const Algorithm &algo : m_disabled) {
        out.AddMember(StringRef(algo.name()), false, allocator);
    }

    for (const auto &kv : m_aliases) {
        out.AddMember(StringRef(kv.first.name()), kv.second.toJSON(), allocator);
    }
}


namespace xmrig {

template class Threads<CpuThreads>;

#ifdef XMRIG_FEATURE_OPENCL
template class Threads<OclThreads>;
#endif

#ifdef XMRIG_FEATURE_CUDA
template class Threads<CudaThreads>;
#endif

} 

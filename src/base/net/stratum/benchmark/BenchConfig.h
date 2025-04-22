

#ifndef XMRIG_BENCHCONFIG_H
#define XMRIG_BENCHCONFIG_H


#include "base/crypto/Algorithm.h"
#include "base/tools/String.h"


namespace xmrig {


class BenchConfig
{
public:
    static const char *kAlgo;
    static const char *kApiHost;
    static const char *kBenchmark;
    static const char *kHash;
    static const char *kId;
    static const char *kSeed;
    static const char *kSize;
    static const char* kRotation;
    static const char *kSubmit;
    static const char *kToken;
    static const char *kUser;
    static const char *kVerify;

#   ifndef XMRIG_DEBUG_BENCHMARK_API
    static constexpr bool kApiTLS               = true;
    static constexpr const uint16_t kApiPort    = 443;
#   else
    static constexpr bool kApiTLS               = false;
    static constexpr const uint16_t kApiPort    = 18805;
#   endif

    BenchConfig(uint32_t size, const String &id, const rapidjson::Value &object, bool dmi, uint32_t rotation);

    static BenchConfig *create(const rapidjson::Value &object, bool dmi);

    inline bool isDMI() const                   { return m_dmi; }
    inline bool isSubmit() const                { return m_submit; }
    inline const Algorithm &algorithm() const   { return m_algorithm; }
    inline const String &id() const             { return m_id; }
    inline const String &seed() const           { return m_seed; }
    inline const String &token() const          { return m_token; }
    inline const String &user() const           { return m_user; }
    inline uint32_t size() const                { return m_size; }
    inline uint64_t hash() const                { return m_hash; }
    inline uint32_t rotation() const            { return m_rotation; }

    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    static uint32_t getSize(const char *benchmark);

    Algorithm m_algorithm;
    bool m_dmi;
    bool m_submit;
    String m_id;
    String m_seed;
    String m_token;
    String m_user;
    uint32_t m_size;
    uint32_t m_rotation;
    uint64_t m_hash = 0;
};


} 


#endif 

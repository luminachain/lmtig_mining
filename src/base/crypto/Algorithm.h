

#ifndef XMRIG_ALGORITHM_H
#define XMRIG_ALGORITHM_H


#include <functional>
#include <vector>


#include "3rdparty/rapidjson/fwd.h"


namespace xmrig {


class Algorithm
{
public:
    
    
    
    
    
    
    
    
    
    enum Id : uint32_t {
        INVALID         = 0,
        RX_LMN          = 0x72151200,   
        RX_WOW          = 0x72141177,   
        RX_ARQ          = 0x72121061,   
        RX_GRAFT        = 0x72151267,   
        RX_SFX          = 0x72151273,   
        RX_YADA         = 0x72151279,   
    };

    enum Family : uint32_t {
        UNKNOWN         = 0,
        RANDOM_X        = 0x72000000
    };

    static const char *kINVALID;
    static const char *kRX;
    static const char *kRX_LMN;
    static const char *kRX_WOW;
    static const char *kRX_ARQ;
    static const char *kRX_GRAFT;
    static const char *kRX_SFX;
    static const char *kRX_YADA;

    inline Algorithm() = default;
    inline Algorithm(const char *algo) : m_id(parse(algo))  {}
    inline Algorithm(Id id) : m_id(id)                      {}
    Algorithm(const rapidjson::Value &value);
    Algorithm(uint32_t id);

    static inline constexpr bool isCN(Id id)                { return (id & 0xff000000) == CN_ANY; }
    static inline constexpr Id base(Id id)                  { return isCN(id) ? static_cast<Id>(CN_0 | (id & 0xff00)) : INVALID; }
    static inline constexpr size_t l2(Id id)                { return family(id) == RANDOM_X ? (1U << ((id >> 8) & 0xff)) : 0U; }
    static inline constexpr size_t l3(Id id)                { return 1ULL << ((id >> 16) & 0xff); }
    static inline constexpr uint32_t family(Id id)          { return id & (isCN(id) ? 0xffff0000 : 0xff000000); }

    inline bool isCN() const                                { return isCN(m_id); }
    inline bool isEqual(const Algorithm &other) const       { return m_id == other.m_id; }
    inline bool isValid() const                             { return m_id != INVALID && family() > UNKNOWN; }
    inline Id base() const                                  { return base(m_id); }
    inline Id id() const                                    { return m_id; }
    inline size_t l2() const                                { return l2(m_id); }
    inline uint32_t family() const                          { return family(m_id); }
    inline uint32_t minIntensity() const                    { return ((m_id == GHOSTRIDER_RTM) ? 8 : 1); };
    inline uint32_t maxIntensity() const                    { return isCN() ? 5 : ((m_id == GHOSTRIDER_RTM) ? 8 : 1); };

    inline size_t l3() const                                { return l3(m_id); }

    inline bool operator!=(Algorithm::Id id) const          { return m_id != id; }
    inline bool operator!=(const Algorithm &other) const    { return !isEqual(other); }
    inline bool operator==(Algorithm::Id id) const          { return m_id == id; }
    inline bool operator==(const Algorithm &other) const    { return isEqual(other); }
    inline operator Algorithm::Id() const                   { return m_id; }

    const char *name() const;
    rapidjson::Value toJSON() const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;

    static Id parse(const char *name);
    static size_t count();
    static std::vector<Algorithm> all(const std::function<bool(const Algorithm &algo)> &filter = nullptr);

private:
    Id m_id = INVALID;
};


using Algorithms = std::vector<Algorithm>;


} 


#endif

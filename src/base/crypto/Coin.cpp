

#include "base/crypto/Coin.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"
#include "base/io/log/Log.h"


#include <cstring>


#ifdef _MSC_VER
#   define strcasecmp _stricmp
#endif


namespace xmrig {


struct CoinInfo
{
    const Algorithm::Id algorithm;
    const char *code;
    const char *name;
    const uint64_t target;
    const uint64_t units;
    const char *tag;
};


static const CoinInfo coinInfo[] = {
    { Algorithm::INVALID,         nullptr,    nullptr,        0,      0,              nullptr },
    { Algorithm::RX_LMN,            "LMT",      "LMT",         120,    1000000000000,  YELLOW_BG_BOLD( WHITE_BOLD_S " lmt     ") },
};


static_assert(Coin::MAX == sizeof(coinInfo) / sizeof(coinInfo[0]), "size mismatch");


const char *Coin::kDisabled = "DISABLED_COIN";
const char *Coin::kField    = "coin";
const char *Coin::kUnknown  = "UNKNOWN_COIN";


} 


xmrig::Coin::Coin(const rapidjson::Value &value)
{
    if (value.IsString()) {
        m_id = parse(value.GetString());
    }
    else if (value.IsObject() && !value.ObjectEmpty()) {
        m_id = parse(Json::getString(value, kField));
    }
}


xmrig::Algorithm xmrig::Coin::algorithm(uint8_t) const
{
    return coinInfo[m_id].algorithm;
}


const char *xmrig::Coin::code() const
{
    return coinInfo[m_id].code;
}


const char *xmrig::Coin::name() const
{
    return coinInfo[m_id].name;
}


rapidjson::Value xmrig::Coin::toJSON() const
{
    using namespace rapidjson;

    return isValid() ? Value(StringRef(code())) : Value(kNullType);
}


uint64_t xmrig::Coin::target(uint8_t) const
{
    return coinInfo[m_id].target;
}


uint64_t xmrig::Coin::units() const
{
    return coinInfo[m_id].units;
}


xmrig::Coin::Id xmrig::Coin::parse(const char *name)
{
    if (name == nullptr || strlen(name) < 3) {
        return INVALID;
    }

    for (uint32_t i = 1; i < MAX; ++i) {
        if (strcasecmp(name, coinInfo[i].code) == 0 || strcasecmp(name, coinInfo[i].name) == 0) {
            return static_cast<Id>(i);
        }
    }

    return INVALID;
}


const char *xmrig::Coin::tag(Id id)
{
    return coinInfo[id].tag;
}

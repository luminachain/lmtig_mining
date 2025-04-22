

#ifndef XMRIG_CVT_H
#define XMRIG_CVT_H


#include "3rdparty/rapidjson/fwd.h"
#include "base/tools/Buffer.h"
#include "base/tools/Span.h"
#include "base/tools/String.h"


#include <string>


namespace xmrig {


class Cvt
{
public:
    inline static bool fromHex(Buffer &buf, const String &hex)                  { return fromHex(buf, hex.data(), hex.size()); }
    inline static Buffer fromHex(const std::string &hex)                        { return fromHex(hex.data(), hex.size()); }
    inline static Buffer fromHex(const String &hex)                             { return fromHex(hex.data(), hex.size()); }
    inline static String toHex(const std::string &data)                         { return toHex(reinterpret_cast<const uint8_t *>(data.data()), data.size()); }

    template<typename T>
    inline static String toHex(const T &data)                                   { return toHex(data.data(), data.size()); }

    static bool fromHex(Buffer &buf, const char *in, size_t size);
    static bool fromHex(Buffer &buf, const rapidjson::Value &value);
    static bool fromHex(std::string &buf, const char *in, size_t size);
    static bool fromHex(uint8_t *bin, size_t bin_maxlen, const char *hex, size_t hex_len);
    static bool fromHex(uint8_t *bin, size_t bin_maxlen, const rapidjson::Value &value);
    static bool toHex(char *hex, size_t hex_maxlen, const uint8_t *bin, size_t bin_len);
    static Buffer fromHex(const char *in, size_t size);
    static Buffer randomBytes(size_t size);
    static rapidjson::Value toHex(const Buffer &data, rapidjson::Document &doc);
    static rapidjson::Value toHex(const Span &data, rapidjson::Document &doc);
    static rapidjson::Value toHex(const std::string &data, rapidjson::Document &doc);
    static rapidjson::Value toHex(const uint8_t *in, size_t size, rapidjson::Document &doc);
    static String toHex(const uint8_t *in, size_t size);
    static void randomBytes(void *buf, size_t size);
};


} 


#endif 

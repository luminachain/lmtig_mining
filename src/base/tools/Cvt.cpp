

#include "base/tools/Cvt.h"
#include "3rdparty/rapidjson/document.h"


#include <cassert>
#include <random>


#ifdef XMRIG_SODIUM
#   include <sodium.h>
#endif


namespace xmrig {


static char *cvt_bin2hex(char *const hex, const size_t hex_maxlen, const unsigned char *const bin, const size_t bin_len)
{
    size_t       i = 0U;
    unsigned int x = 0U;
    int          b = 0;
    int          c = 0;

    if (bin_len >= SIZE_MAX / 2 || hex_maxlen < bin_len * 2U) {
        return nullptr; 
    }

    while (i < bin_len) {
        c = bin[i] & 0xf;
        b = bin[i] >> 4;
        x = (unsigned char) (87U + c + (((c - 10U) >> 8) & ~38U)) << 8 |
            (unsigned char) (87U + b + (((b - 10U) >> 8) & ~38U));
        hex[i * 2U] = (char) x;
        x >>= 8;
        hex[i * 2U + 1U] = (char) x;
        i++;
    }

    if (i * 2U < hex_maxlen) {
        hex[i * 2U] = 0U;
    }

    return hex;
}


#ifndef XMRIG_SODIUM
static std::random_device randomDevice;
static std::mt19937 randomEngine(randomDevice());


static int cvt_hex2bin(unsigned char *const bin, const size_t bin_maxlen, const char *const hex, const size_t hex_len, const char *const ignore, size_t *const bin_len, const char **const hex_end)
{
    size_t        bin_pos   = 0U;
    size_t        hex_pos   = 0U;
    int           ret       = 0;
    unsigned char c         = 0U;
    unsigned char c_acc     = 0U;
    unsigned char c_alpha0  = 0U;
    unsigned char c_alpha   = 0U;
    unsigned char c_num0    = 0U;
    unsigned char c_num     = 0U;
    unsigned char c_val     = 0U;
    unsigned char state     = 0U;

    while (hex_pos < hex_len) {
        c        = (unsigned char) hex[hex_pos];
        c_num    = c ^ 48U;
        c_num0   = (c_num - 10U) >> 8;
        c_alpha  = (c & ~32U) - 55U;
        c_alpha0 = ((c_alpha - 10U) ^ (c_alpha - 16U)) >> 8;

        if ((c_num0 | c_alpha0) == 0U) {
            if (ignore != nullptr && state == 0U && strchr(ignore, c) != nullptr) {
                hex_pos++;
                continue;
            }
            break;
        }

        c_val = (c_num0 & c_num) | (c_alpha0 & c_alpha);

        if (bin_pos >= bin_maxlen) {
            ret   = -1;
            errno = ERANGE;
            break;
        }

        if (state == 0U) {
            c_acc = c_val * 16U;
        } else {
            bin[bin_pos++] = c_acc | c_val;
        }

        state = ~state;
        hex_pos++;
    }

    if (state != 0U) {
        hex_pos--;
        errno = EINVAL;
        ret = -1;
    }

    if (ret != 0) {
        bin_pos = 0U;
    }

    if (hex_end != nullptr) {
        *hex_end = &hex[hex_pos];
    } else if (hex_pos != hex_len) {
        errno = EINVAL;
        ret = -1;
    }

    if (bin_len != nullptr) {
        *bin_len = bin_pos;
    }

    return ret;
}

#define sodium_hex2bin cvt_hex2bin
#endif


template<typename T>
inline bool fromHexImpl(T &buf, const char *in, size_t size)
{
    assert(in != nullptr && size > 0);
    if (in == nullptr || size == 0) {
        return false;
    }

    buf.resize(size / 2);

    return sodium_hex2bin(reinterpret_cast<uint8_t *>(&buf.front()), buf.size(), in, size, nullptr, nullptr, nullptr) == 0;
}


} 


bool xmrig::Cvt::fromHex(Buffer &buf, const char *in, size_t size)
{
    return fromHexImpl(buf, in, size);
}


bool xmrig::Cvt::fromHex(Buffer &buf, const rapidjson::Value &value)
{
    if (!value.IsString()) {
        return false;
    }

    return fromHexImpl(buf, value.GetString(), value.GetStringLength());
}


bool xmrig::Cvt::fromHex(std::string &buf, const char *in, size_t size)
{
    return fromHexImpl(buf, in, size);
}


bool xmrig::Cvt::fromHex(uint8_t *bin, size_t bin_maxlen, const char *hex, size_t hex_len)
{
    assert(hex != nullptr && hex_len > 0);
    if (hex == nullptr || hex_len == 0) {
        return false;
    }

    return sodium_hex2bin(bin, bin_maxlen, hex, hex_len, nullptr, nullptr, nullptr) == 0;
}


bool xmrig::Cvt::fromHex(uint8_t *bin, size_t bin_maxlen, const rapidjson::Value &value)
{
    if (!value.IsString()) {
        return false;
    }

    return fromHex(bin, bin_maxlen, value.GetString(), value.GetStringLength());
}


xmrig::Buffer xmrig::Cvt::fromHex(const char *in, size_t size)
{
    Buffer buf;
    if (!fromHex(buf, in, size)) {
        return {};
    }

    return buf;
}


bool xmrig::Cvt::toHex(char *hex, size_t hex_maxlen, const uint8_t *bin, size_t bin_len)
{
    return cvt_bin2hex(hex, hex_maxlen, bin, bin_len) != nullptr;
}


xmrig::Buffer xmrig::Cvt::randomBytes(const size_t size)
{
    Buffer buf(size);

#   ifndef XMRIG_SODIUM
    std::uniform_int_distribution<> dis(0, 255);

    for (size_t i = 0; i < size; ++i) {
        buf[i] = static_cast<char>(dis(randomEngine));
    }
#   else
    randombytes_buf(buf.data(), size);
#   endif

    return buf;
}


rapidjson::Value xmrig::Cvt::toHex(const Buffer &data, rapidjson::Document &doc)
{
    return toHex(data.data(), data.size(), doc);
}


rapidjson::Value xmrig::Cvt::toHex(const Span &data, rapidjson::Document &doc)
{
    return toHex(data.data(), data.size(), doc);
}


rapidjson::Value xmrig::Cvt::toHex(const std::string &data, rapidjson::Document &doc)
{
    return toHex(reinterpret_cast<const uint8_t *>(data.data()), data.size(), doc);
}


rapidjson::Value xmrig::Cvt::toHex(const uint8_t *in, size_t size, rapidjson::Document &doc)
{
    return toHex(in, size).toJSON(doc);
}


xmrig::String xmrig::Cvt::toHex(const uint8_t *in, size_t size)
{
    assert(in != nullptr && size > 0);
    if (in == nullptr || size == 0) {
        return {};
    }

    const size_t hex_maxlen = size * 2 + 1;

    char *buf = new char[hex_maxlen];
    if (!toHex(buf, hex_maxlen, in, size)) {
        delete [] buf;

        return {};
    }

    return buf;
}


void xmrig::Cvt::randomBytes(void *buf, size_t size)
{
#   ifndef XMRIG_SODIUM
    std::uniform_int_distribution<> dis(0, 255);

    for (size_t i = 0; i < size; ++i) {
        static_cast<uint8_t *>(buf)[i] = static_cast<char>(dis(randomEngine));
    }
#   else
    randombytes_buf(buf, size);
#   endif
}

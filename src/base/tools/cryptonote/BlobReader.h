

#ifndef XMRIG_BLOBREADER_H
#define XMRIG_BLOBREADER_H


#include <cstdint>
#include <cstring>
#include <stdexcept>


namespace xmrig {


template<bool EXCEPTIONS>
class BlobReader
{
public:
    inline BlobReader(const uint8_t *data, size_t size) :
        m_size(size),
        m_data(data)
    {}

    inline bool operator()(uint64_t &data)  { return getVarint(data); }
    inline bool operator()(uint8_t &data)   { return getByte(data); }
    inline size_t index() const             { return m_index; }
    inline size_t remaining() const         { return m_size - m_index;  }

    inline bool skip(size_t n)
    {
        if (m_index + n > m_size) {
            return outOfRange();
        }

        m_index += n;

        return true;
    }

    template<size_t N>
    inline bool operator()(uint8_t(&data)[N])
    {
        if (m_index + N > m_size) {
            return outOfRange();
        }

        memcpy(data, m_data + m_index, N);
        m_index += N;

        return true;
    }

    template<typename T>
    inline bool operator()(T &data, size_t n)
    {
        if (m_index + n > m_size) {
            return outOfRange();
        }

        data = { m_data + m_index, n };
        m_index += n;

        return true;
    }

private:
    inline bool getByte(uint8_t &data)
    {
        if (m_index >= m_size) {
            return outOfRange();
        }

        data = m_data[m_index++];

        return true;
    }

    inline bool getVarint(uint64_t &data)
    {
        uint64_t result = 0;
        uint8_t t;
        int shift = 0;

        do {
            if (!getByte(t)) {
                return outOfRange();
            }

            result |= static_cast<uint64_t>(t & 0x7F) << shift;
            shift += 7;
        } while (t & 0x80);

        data = result;

        return true;
    }

    inline bool outOfRange()
    {
        if (EXCEPTIONS) {
            throw std::out_of_range("Blob read out of range");
        }

        return false;
    }

    const size_t m_size;
    const uint8_t *m_data;
    size_t m_index  = 0;
};


} 


#endif 

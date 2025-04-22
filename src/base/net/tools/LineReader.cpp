


#include "base/net/tools/LineReader.h"
#include "base/kernel/constants.h"
#include "base/kernel/interfaces/ILineListener.h"
#include "base/net/tools/NetBuffer.h"

#include <cassert>
#include <cstring>


xmrig::LineReader::~LineReader()
{
    NetBuffer::release(m_buf);
}


void xmrig::LineReader::parse(char *data, size_t size)
{
    assert(m_listener != nullptr && size > 0);
    if (!m_listener || size == 0) {
        return;
    }

    getline(data, size);
}


void xmrig::LineReader::reset()
{
    if (m_buf) {
        NetBuffer::release(m_buf);
        m_buf = nullptr;
        m_pos = 0;
    }
}


void xmrig::LineReader::add(const char *data, size_t size)
{
    if (size + m_pos > XMRIG_NET_BUFFER_CHUNK_SIZE) {
        
        return;
    }

    if (!m_buf) {
        m_buf = NetBuffer::allocate();
        m_pos = 0;
    }

    memcpy(m_buf + m_pos, data, size);
    m_pos += size;
}


void xmrig::LineReader::getline(char *data, size_t size)
{
    char *end        = nullptr;
    char *start      = data;
    size_t remaining = size;

    while ((end = static_cast<char*>(memchr(start, '\n', remaining))) != nullptr) {
        *end = '\0';

        end++;

        const auto len = static_cast<size_t>(end - start);
        if (m_pos) {
            add(start, len);
            m_listener->onLine(m_buf, m_pos - 1);
            m_pos = 0;
        }
        else if (len > 1) {
            m_listener->onLine(start, len - 1);
        }

        remaining -= len;
        start = end;
    }

    if (remaining == 0) {
        return reset();
    }

    add(start, remaining);
}

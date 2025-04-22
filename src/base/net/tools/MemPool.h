

#ifndef XMRIG_MEMPOOL_H
#define XMRIG_MEMPOOL_H


#include <array>
#include <cassert>
#include <cstddef>
#include <map>
#include <set>


namespace xmrig {


template<size_t CHUNK_SIZE, size_t INIT_SIZE>
class MemPool
{
public:
    MemPool() = default;


    constexpr size_t chunkSize() const  { return CHUNK_SIZE; }
    inline size_t freeSize() const      { return m_free.size() * CHUNK_SIZE; }
    inline size_t size() const          { return m_data.size() * CHUNK_SIZE * INIT_SIZE; }


    inline char *allocate()
    {
        if (m_free.empty()) {
            resize();
        }

        const size_t i = *m_free.begin();
        const size_t r = i / INIT_SIZE;

        char *ptr = m_data[r].data() + (i - r * INIT_SIZE) * CHUNK_SIZE;

        m_used.insert({ ptr, i });
        m_free.erase(i);

        return ptr;
    }


    inline void deallocate(const char *ptr)
    {
        if (ptr == nullptr) {
            return;
        }

        assert(m_used.count(ptr));

        m_free.emplace(m_used[ptr]);
        m_used.erase(ptr);
    }


private:
    inline void resize()
    {
        const size_t index = m_data.size();
        m_data[index];

        for (size_t i = 0; i < INIT_SIZE; ++i) {
            m_free.emplace((index * INIT_SIZE) + i);
        }
    }


    std::map<const char *, size_t> m_used;
    std::map<size_t, std::array<char, CHUNK_SIZE * INIT_SIZE> > m_data;
    std::set<size_t> m_free;
};


} 


#endif 

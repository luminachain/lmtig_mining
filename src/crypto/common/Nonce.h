

#ifndef XMRIG_NONCE_H
#define XMRIG_NONCE_H


#include <atomic>


namespace xmrig {


class Nonce
{
public:
    enum Backend : uint32_t {
        CPU,
        OPENCL,
        CUDA,
        MAX
    };


    static inline bool isOutdated(Backend backend, uint64_t sequence)   { return m_sequence[backend].load(std::memory_order_relaxed) != sequence; }
    static inline bool isPaused()                                       { return m_paused.load(std::memory_order_relaxed); }
    static inline uint64_t sequence(Backend backend)                    { return m_sequence[backend].load(std::memory_order_relaxed); }
    static inline void pause(bool paused)                               { m_paused = paused; }
    static inline void reset(uint8_t index)                             { m_nonces[index] = 0; }
    static inline void stop(Backend backend)                            { m_sequence[backend] = 0; }
    static inline void touch(Backend backend)                           { m_sequence[backend]++; }

    static bool next(uint8_t index, uint32_t *nonce, uint32_t reserveCount, uint64_t mask);
    static void stop();
    static void touch();

private:
    static std::atomic<bool> m_paused;
    static std::atomic<uint64_t> m_sequence[MAX];
    static std::atomic<uint64_t> m_nonces[2];
};


} 


#endif 

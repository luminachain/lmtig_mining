

#include "base/tools/Alignment.h"
#include "crypto/common/Nonce.h"


namespace xmrig {

std::atomic<bool> Nonce::m_paused = {true};
std::atomic<uint64_t>  Nonce::m_sequence[Nonce::MAX] = { {1}, {1}, {1} };
std::atomic<uint64_t> Nonce::m_nonces[2] = { {0}, {0} };


} 


bool xmrig::Nonce::next(uint8_t index, uint32_t *nonce, uint32_t reserveCount, uint64_t mask)
{
    mask &= 0x7FFFFFFFFFFFFFFFULL;
    if (reserveCount == 0 || mask < reserveCount - 1) {
        return false;
    }

    uint64_t counter = m_nonces[index].fetch_add(reserveCount, std::memory_order_relaxed);
    while (true) {
        if (mask < counter) {
            return false;
        }

        if (mask - counter <= reserveCount - 1) {
            pause(true);
            if (mask - counter < reserveCount - 1) {
                return false;
            }
        }
        else if (0xFFFFFFFFUL - (uint32_t)counter < reserveCount - 1) {
            counter = m_nonces[index].fetch_add(reserveCount, std::memory_order_relaxed);
            continue;
        }

        writeUnaligned(nonce, static_cast<uint32_t>((readUnaligned(nonce) & ~mask) | counter));

        if (mask > 0xFFFFFFFFULL) {
            writeUnaligned(nonce + 1, static_cast<uint32_t>((readUnaligned(nonce + 1) & (~mask >> 32)) | (counter >> 32)));
        }

        return true;
    }
}


void xmrig::Nonce::stop()
{
    pause(false);

    for (auto &i : m_sequence) {
        i = 0;
    }
}


void xmrig::Nonce::touch()
{
    for (auto &i : m_sequence) {
        i++;
    }
}

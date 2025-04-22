

#ifndef XMRIG_CRYPTONIGHT_H
#define XMRIG_CRYPTONIGHT_H


#include <stddef.h>
#include <stdint.h>

#if defined _MSC_VER || defined XMRIG_ARM
#   define ABI_ATTRIBUTE
#else
#   define ABI_ATTRIBUTE __attribute__((ms_abi))
#endif


struct cryptonight_ctx;
typedef void(*cn_mainloop_fun_ms_abi)(cryptonight_ctx**) ABI_ATTRIBUTE;


struct cryptonight_r_data {
    int algo;
    uint64_t height;

    bool match(const int a, const uint64_t h) const { return (a == algo) && (h == height); }
};


struct cryptonight_ctx {
    alignas(16) uint8_t state[224];
    alignas(16) uint8_t *memory;
    const uint32_t* tweak1_table;
    uint64_t tweak1_2;

    uint8_t unused[24];
    const uint32_t *saes_table;

    cn_mainloop_fun_ms_abi generated_code;
    cryptonight_r_data generated_code_data;

    alignas(16) uint8_t save_state[128];
    bool first_half;
};


#endif 

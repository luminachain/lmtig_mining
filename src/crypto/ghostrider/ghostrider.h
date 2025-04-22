

#ifndef XMRIG_GR_HASH_H
#define XMRIG_GR_HASH_H


#include <cstddef>
#include <cstdint>
#include <vector>


struct cryptonight_ctx;


namespace xmrig
{


namespace ghostrider
{


struct HelperThread;

void benchmark();
HelperThread* create_helper_thread(int64_t cpu_index, int priority, const std::vector<int64_t>& affinities);
void destroy_helper_thread(HelperThread* t);
void hash_octa(const uint8_t* data, size_t size, uint8_t* output, cryptonight_ctx** ctx, HelperThread* helper, bool verbose = true);


} 


} 

#endif 
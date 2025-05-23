

#ifndef XMRIG_SIGNATURES_H
#define XMRIG_SIGNATURES_H


#include <cstdint>


namespace xmrig {


void generate_signature(const uint8_t* prefix_hash, const uint8_t* pub, const uint8_t* sec, uint8_t* sig);
bool check_signature(const uint8_t* prefix_hash, const uint8_t* pub, const uint8_t* sig);

bool generate_key_derivation(const uint8_t* key1, const uint8_t* key2, uint8_t* derivation, uint8_t* view_tag);
void derive_secret_key(const uint8_t* derivation, size_t output_index, const uint8_t* base, uint8_t* derived_key);
bool derive_public_key(const uint8_t* derivation, size_t output_index, const uint8_t* base, uint8_t* derived_key);

void derive_view_secret_key(const uint8_t* spend_secret_key, uint8_t* view_secret_key);

void generate_keys(uint8_t* pub, uint8_t* sec);
bool secret_key_to_public_key(const uint8_t* sec, uint8_t* pub);

} 


#endif 

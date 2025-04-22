

#include "base/tools/cryptonote/BlockTemplate.h"
#include "3rdparty/rapidjson/document.h"
#include "base/crypto/keccak.h"
#include "base/tools/cryptonote/BlobReader.h"
#include "base/tools/Cvt.h"


void xmrig::BlockTemplate::calculateMinerTxHash(const uint8_t *prefix_begin, const uint8_t *prefix_end, uint8_t *hash)
{
    uint8_t hashes[kHashSize * 3];

    

    
    keccak(prefix_begin, static_cast<int>(prefix_end - prefix_begin), hashes, kHashSize);

    
    static const uint8_t known_second_hash[kHashSize] = {
        188,54,120,158,122,30,40,20,54,70,66,41,130,143,129,125,102,18,247,180,119,214,101,145,255,150,169,224,100,188,201,138
    };
    memcpy(hashes + kHashSize, known_second_hash, kHashSize);

    
    memset(hashes + kHashSize * 2, 0, kHashSize);

    
    keccak(hashes, sizeof(hashes), hash, kHashSize);
}


void xmrig::BlockTemplate::calculateRootHash(const uint8_t *prefix_begin, const uint8_t *prefix_end, const Buffer &miner_tx_merkle_tree_branch, uint8_t *root_hash)
{
    calculateMinerTxHash(prefix_begin, prefix_end, root_hash);

    for (size_t i = 0; i < miner_tx_merkle_tree_branch.size(); i += kHashSize) {
        uint8_t h[kHashSize * 2];

        memcpy(h, root_hash, kHashSize);
        memcpy(h + kHashSize, miner_tx_merkle_tree_branch.data() + i, kHashSize);

        keccak(h, kHashSize * 2, root_hash, kHashSize);
    }
}


void xmrig::BlockTemplate::calculateMerkleTreeHash()
{
    m_minerTxMerkleTreeBranch.clear();

    const uint64_t count = m_numHashes + 1;
    const uint8_t *h = m_hashes.data();

    if (count == 1) {
        memcpy(m_rootHash, h, kHashSize);
    }
    else if (count == 2) {
        m_minerTxMerkleTreeBranch.insert(m_minerTxMerkleTreeBranch.end(), h + kHashSize, h + kHashSize * 2);
        keccak(h, kHashSize * 2, m_rootHash, kHashSize);
    }
    else {
        size_t i    = 0;
        size_t j    = 0;
        size_t cnt  = 0;

        for (i = 0, cnt = 1; cnt <= count; ++i, cnt <<= 1) {}

        cnt >>= 1;

        m_minerTxMerkleTreeBranch.reserve(kHashSize * (i - 1));

        Buffer ints(cnt * kHashSize);
        memcpy(ints.data(), h, (cnt * 2 - count) * kHashSize);

        for (i = cnt * 2 - count, j = cnt * 2 - count; j < cnt; i += 2, ++j) {
            if (i == 0) {
                m_minerTxMerkleTreeBranch.insert(m_minerTxMerkleTreeBranch.end(), h + kHashSize, h + kHashSize * 2);
            }
            keccak(h + i * kHashSize, kHashSize * 2, ints.data() + j * kHashSize, kHashSize);
        }

        while (cnt > 2) {
            cnt >>= 1;
            for (i = 0, j = 0; j < cnt; i += 2, ++j) {
                if (i == 0) {
                    m_minerTxMerkleTreeBranch.insert(m_minerTxMerkleTreeBranch.end(), ints.data() + kHashSize, ints.data() + kHashSize * 2);
                }
                keccak(ints.data() + i * kHashSize, kHashSize * 2, ints.data() + j * kHashSize, kHashSize);
            }
        }

        m_minerTxMerkleTreeBranch.insert(m_minerTxMerkleTreeBranch.end(), ints.data() + kHashSize, ints.data() + kHashSize * 2);
        keccak(ints.data(), kHashSize * 2, m_rootHash, kHashSize);
    }
}


bool xmrig::BlockTemplate::parse(const Buffer &blocktemplate, const Coin &coin, bool hashes)
{
    if (blocktemplate.size() < kMinSize) {
        return false;
    }

    m_blob  = blocktemplate;
    m_coin  = coin;
    bool rc = false;

    try {
        rc = parse(hashes);
    } catch (...) {}

    return rc;
}


bool xmrig::BlockTemplate::parse(const char *blocktemplate, size_t size, const Coin &coin, bool hashes)
{
    if (size < (kMinSize * 2) || !Cvt::fromHex(m_blob, blocktemplate, size)) {
        return false;
    }

    m_coin  = coin;
    bool rc = false;

    try {
        rc = parse(hashes);
    } catch (...) {}

    return rc;
}


bool xmrig::BlockTemplate::parse(const rapidjson::Value &blocktemplate, const Coin &coin, bool hashes)
{
    return blocktemplate.IsString() && parse(blocktemplate.GetString(), blocktemplate.GetStringLength(), coin, hashes);
}


bool xmrig::BlockTemplate::parse(const String &blocktemplate, const Coin &coin, bool hashes)
{
    return parse(blocktemplate.data(), blocktemplate.size(), coin, hashes);
}


void xmrig::BlockTemplate::generateHashingBlob(Buffer &out) const
{
    out.clear();
    out.reserve(offset(MINER_TX_PREFIX_OFFSET) + kHashSize + 3);

    out.assign(m_blob.begin(), m_blob.begin() + offset(MINER_TX_PREFIX_OFFSET));
    out.insert(out.end(), m_rootHash, m_rootHash + kHashSize);

    uint64_t k = m_numHashes + 1;
    while (k >= 0x80) {
        out.emplace_back((static_cast<uint8_t>(k) & 0x7F) | 0x80);
        k >>= 7;
    }
    out.emplace_back(static_cast<uint8_t>(k));
}


bool xmrig::BlockTemplate::parse(bool hashes)
{
    BlobReader<true> ar(m_blob.data(), m_blob.size());

    
    ar(m_version.first);
    ar(m_version.second);
    ar(m_timestamp);
    ar(m_prevId, kHashSize);

    setOffset(NONCE_OFFSET, ar.index());
    ar.skip(kNonceSize);

    
    if (m_coin == Coin::WOWNERO && majorVersion() >= 18) {
        ar(m_minerSignature, kSignatureSize);
        ar(m_vote);
    }

    if (m_coin == Coin::ZEPHYR) {
        uint8_t pricing_record[120];
        ar(pricing_record);
    }

    
    
    setOffset(MINER_TX_PREFIX_OFFSET, ar.index());

    ar(m_txVersion);

    if (m_coin != Coin::TOWNFORGE) {
      ar(m_unlockTime);
    }

    ar(m_numInputs);

    
    if (m_numInputs != 1) {
        return false;
    }

    ar(m_inputType);

    
    if (m_inputType != 0xFF) {
        return false;
    }

    ar(m_height);
    ar(m_numOutputs);

    if (m_coin == Coin::ZEPHYR) {
        if (m_numOutputs < 2) {
            return false;
        }
    }
    else if (m_numOutputs != 1) {
        return false;
    }

    ar(m_amount);
    ar(m_outputType);

    
    if ((m_outputType != 2) && (m_outputType != 3)) {
        return false;
    }

    setOffset(EPH_PUBLIC_KEY_OFFSET, ar.index());

    ar(m_ephPublicKey, kKeySize);

    if (m_coin == Coin::ZEPHYR) {
        if (m_outputType != 2) {
            return false;
        }

        uint64_t asset_type_len;
        ar(asset_type_len);
        ar.skip(asset_type_len);
        ar(m_viewTag);

        for (uint64_t k = 1; k < m_numOutputs; ++k) {
            uint64_t amount2;
            ar(amount2);

            uint8_t output_type2;
            ar(output_type2);
            if (output_type2 != 2) {
                return false;
            }

            Span key2;
            ar(key2, kKeySize);

            ar(asset_type_len);
            ar.skip(asset_type_len);

            uint8_t view_tag2;
            ar(view_tag2);
        }
    }
    else if (m_outputType == 3) {
        ar(m_viewTag);
    }

    if (m_coin == Coin::TOWNFORGE) {
      ar(m_unlockTime);
    }

    ar(m_extraSize);

    setOffset(TX_EXTRA_OFFSET, ar.index());

    BlobReader<true> ar_extra(blob(TX_EXTRA_OFFSET), m_extraSize);
    ar.skip(m_extraSize);

    bool pubkey_offset_first = true;

    while (ar_extra.index() < m_extraSize) {
        uint64_t extra_tag  = 0;
        uint64_t size       = 0;

        ar_extra(extra_tag);

        switch (extra_tag) {
        case 0x01: 
            if (pubkey_offset_first) {
                pubkey_offset_first = false;
                setOffset(TX_PUBKEY_OFFSET, offset(TX_EXTRA_OFFSET) + ar_extra.index());
            }
            ar_extra.skip(kKeySize);
            break;

        case 0x02: 
            ar_extra(size);
            setOffset(TX_EXTRA_NONCE_OFFSET, offset(TX_EXTRA_OFFSET) + ar_extra.index());
            ar_extra(m_txExtraNonce, size);
            break;

        case 0x03: 
            ar_extra(size);
            setOffset(TX_EXTRA_MERGE_MINING_TAG_OFFSET, offset(TX_EXTRA_OFFSET) + ar_extra.index());
            ar_extra(m_txMergeMiningTag, size);
            break;

        default:
            return false; 
        }
    }

    if (m_coin == Coin::ZEPHYR) {
        uint64_t pricing_record_height, amount_burnt, amount_minted;
        ar(pricing_record_height);
        ar(amount_burnt);
        ar(amount_minted);
    }

    setOffset(MINER_TX_PREFIX_END_OFFSET, ar.index());
    

    
    uint8_t vin_rct_type = 0;
    ar(vin_rct_type);

    
    if (m_coin == Coin::TOWNFORGE && m_height % 720 == 0) {
      return true;
    }

    
    if (vin_rct_type != 0) {
        return false;
    }

    const size_t miner_tx_end = ar.index();
    

    
    if ((miner_tx_end != offset(MINER_TX_PREFIX_END_OFFSET) + 1) || (*blob(MINER_TX_PREFIX_END_OFFSET) != 0)) {
        return false;
    }

    
    ar(m_numHashes);

    if (hashes) {
        m_hashes.resize((m_numHashes + 1) * kHashSize);
        calculateMinerTxHash(blob(MINER_TX_PREFIX_OFFSET), blob(MINER_TX_PREFIX_END_OFFSET), m_hashes.data());

        for (uint64_t i = 1; i <= m_numHashes; ++i) {
            Span h;
            ar(h, kHashSize);
            memcpy(m_hashes.data() + i * kHashSize, h.data(), kHashSize);
        }

        calculateMerkleTreeHash();
    }

    return true;
}

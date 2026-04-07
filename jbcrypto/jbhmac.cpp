/////////////////////////////////////////////////////////////////////////////
// Name:        jbhmac.cpp
// Purpose:     Cryptographic methods, HMAC, PBKDF2
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#include "jbhmac.h"
#include "jbcommon.hpp"

std::vector<uint8_t> JBHmac::hmacSha256(const std::vector<uint8_t>& key,
                                        const std::vector<uint8_t>& message) {
    JBSha sha;
    // key -> sha256 block size
    std::vector<uint8_t> k;
    if (key.size() > SHA256_BLOCK_SIZE) {
        k = sha.computeSha256(key); // too long → make hash
    } else {
        k = key; // copy only
    }
    k.resize(SHA256_BLOCK_SIZE, 0x00); // k is now the 64 byte key
    // prepare ipad and opad
    std::vector<uint8_t> ipad(SHA256_BLOCK_SIZE);
    std::vector<uint8_t> opad(SHA256_BLOCK_SIZE);
    for (size_t i = 0; i < SHA256_BLOCK_SIZE; i++) {
        ipad[i] = k[i] ^ 0x36;
        opad[i] = k[i] ^ 0x5C;
    }
    // inner hash = SHA256( (k ⊕ ipad) || message )
    std::vector<uint8_t> inner;
    inner.reserve(SHA256_BLOCK_SIZE + message.size());
    inner.insert(inner.begin(), ipad.begin(), ipad.end());
    inner.insert(inner.end(), message.begin(), message.end());
    std::vector<uint8_t> innerHash = sha.computeSha256(inner);
    // outer hash = SHA256( (k ⊕ opad) || innerHash )
    std::vector<uint8_t> outer;
    outer.reserve(SHA256_BLOCK_SIZE + innerHash.size());
    outer.insert(outer.begin(), opad.begin(), opad.end());
    outer.insert(outer.end(), innerHash.begin(), innerHash.end());
    return sha.computeSha256(outer);
}

std::vector<uint8_t> JBHmac::pbkdf2HmacSha256(const std::vector<uint8_t>& password,
                                              const std::vector<uint8_t>& salt,
                                              uint32_t iterations,
                                              size_t dkLen) {
    const size_t blocks = (dkLen + SHA256_DIGEST_SIZE - 1) / SHA256_DIGEST_SIZE;
    std::vector<uint8_t> result;
    result.reserve(blocks * SHA256_DIGEST_SIZE);
    for (uint32_t blockIndex = 1; blockIndex <= blocks; blockIndex++) {
        // salt || blockIndex
        std::vector<uint8_t> saltBlock = salt;
        saltBlock.push_back((blockIndex >> 24) & 0xff);
        saltBlock.push_back((blockIndex >> 16) & 0xff);
        saltBlock.push_back((blockIndex >> 8)  & 0xff);
        saltBlock.push_back(blockIndex & 0xff);
        // u1 = HMAC(password, salt || blockIndex)
        std::vector<uint8_t> u = hmacSha256(password, saltBlock);
        std::vector<uint8_t> t = u; // t = u1
        // u2..uN
        for (uint32_t i = 1; i < iterations; i++) {
            u = hmacSha256(password, u);
            for (size_t j = 0; j < SHA256_DIGEST_SIZE; j++)
                t[j] ^= u[j];
        }
        result.insert(result.end(), t.begin(), t.end());
    }
    result.resize(dkLen); // resize to desired key size
    return result;
}

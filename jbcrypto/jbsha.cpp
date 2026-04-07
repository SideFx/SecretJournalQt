/////////////////////////////////////////////////////////////////////////////
// Name:        jbsha.cpp
// Purpose:     SHA256 implementation
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-04
/////////////////////////////////////////////////////////////////////////////

#include "jbsha.h"

JBSha::JBSha() {}

std::vector<uint8_t> JBSha::computeSha256(const std::vector<uint8_t>& payload) {
    constexpr uint64_t chunk_size = SHA256_BLOCK_SIZE;
    constexpr uint64_t bits_size = sizeof(uint64_t);
    constexpr uint64_t rounds = 64;
    constexpr uint32_t kArray256[] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    uint32_t h[] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    std::vector<uint8_t> result(SHA256_DIGEST_SIZE);
    uint64_t i, j;
    const uint64_t l = payload.size();
    uint64_t sd = l / chunk_size + 1;
    if ((l % chunk_size) >= (chunk_size - bits_size)) sd++;
    const uint64_t bits = l << 3;
    uint32_t D[rounds] {};
    std::vector<uint8_t> mc(chunk_size * sd);
    std::copy(payload.begin(), payload.end(), mc.begin());
    mc[l] = 0x80;
    for (i = l + 1; i < sd * chunk_size - bits_size; i++) mc[i] = 0;
    mc[i++] = static_cast<uint8_t>(bits >> 56);
    mc[i++] = static_cast<uint8_t>(bits >> 48);
    mc[i++] = static_cast<uint8_t>(bits >> 40);
    mc[i++] = static_cast<uint8_t>(bits >> 32);
    mc[i++] = static_cast<uint8_t>(bits >> 24);
    mc[i++] = static_cast<uint8_t>(bits >> 16);
    mc[i++] = static_cast<uint8_t>(bits >> 8);
    mc[i] = static_cast<uint8_t>(bits);
    uint64_t z = 0;
    for (uint64_t n = 0; n < sd; n++) {
        uint32_t ha = h[0];
        uint32_t hb = h[1];
        uint32_t hc = h[2];
        uint32_t hd = h[3];
        uint32_t he = h[4];
        uint32_t hf = h[5];
        uint32_t hg = h[6];
        uint32_t hh = h[7];
        for (j = 0; j < 16; j++) {
            D[j] = (mc[z] << 24) | (mc[z + 1] << 16) | (mc[z + 2] << 8) | mc[z + 3];
            z += 4;
        }
        for (j = 16; j < rounds; j++) {
            D[j] = D[j - 16] + (ror32(D[j - 15], 7) ^ ror32(D[j - 15], 18) ^ (D[j - 15] >> 3)) +
                   D[j - 7] + (ror32(D[j - 2], 17) ^ ror32(D[j - 2], 19) ^ (D[j - 2] >> 10));
        }
        for (j = 0; j < rounds; j++) {
            const uint32_t t0 = hh + kArray256[j] + D[j] + ((he & hf) ^ ((~he) & hg)) +
                                (ror32(he, 6) ^ ror32(he, 11) ^ ror32(he, 25));
            const uint32_t t1 = (ror32(ha, 2) ^ ror32(ha, 13) ^ ror32(ha, 22)) +
                                ((ha & hb) ^ (ha & hc) ^ (hb & hc));
            hh = hg;
            hg = hf;
            hf = he;
            he = hd + t0;
            hd = hc;
            hc = hb;
            hb = ha;
            ha = t0 + t1;
        }
        h[0] += ha;
        h[1] += hb;
        h[2] += hc;
        h[3] += hd;
        h[4] += he;
        h[5] += hf;
        h[6] += hg;
        h[7] += hh;
    }
    i = 0;
    for (j = 0; j < std::size(h); j++) {
        result[i++] = static_cast<uint8_t>(h[j] >> 24);
        result[i++] = static_cast<uint8_t>(h[j] >> 16);
        result[i++] = static_cast<uint8_t>(h[j] >> 8);
        result[i++] = static_cast<uint8_t>(h[j]);
    }
    return result;
}

std::vector<uint8_t> JBSha::computeSha256(const std::string& payload) {
    return computeSha256(std::vector<uint8_t>(payload.begin(), payload.end()));
}


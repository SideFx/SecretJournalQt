/////////////////////////////////////////////////////////////////////////////
// Name:        jbtwofish.cpp
// Purpose:     Twofish ECB & CBC implementation
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#include "jbtwofish.h"

JBTwofish::JBTwofish(const std::vector<uint8_t>& p) {
    initialize(p);
}

JBTwofish::JBTwofish(const std::string& s) {
    initialize(s);
}

uint32_t JBTwofish::polyMultiply(uint32_t a, uint32_t b) noexcept {
    uint32_t t = 0;
    while (a != 0) {
        if ((a & 0x01) != 0) t ^= b;
        b <<= 1;
        a >>= 1;
    }
    return t;
}

uint32_t JBTwofish::gfMod(uint32_t a, uint32_t m) noexcept {
    m <<= 7;
    for (int i = 0; i < 8; i++) {
        if (const uint32_t x = a ^ m; x < a) a = x;
        m >>= 1;
    }
    return a;
}

uint32_t JBTwofish::gfMultiply(uint32_t a, uint32_t b, uint32_t m) noexcept {
    return gfMod(polyMultiply(a, b), m);
}

uint32_t JBTwofish::rsMatrixMultiply(const uint8_t *c) noexcept {
    uint8_t r[4];
    for (int j = 0; j < 4; j++) {
        uint8_t t = 0;
        for (int k = 0; k < 8; k++) t ^= gfMultiply(TFRSARRAY[j][k], c[k], RS_MOD);
        r[3 - j] = t;
    }
    return static_cast<uint32_t>((r[0] << 24) ^ (r[1] << 16) ^ (r[2] << 8) ^ r[3]);
}

uint32_t JBTwofish::gFunc(uint32_t a) noexcept {
    return tfqfarray[0][a & 0xff] ^ tfqfarray[1][(a >> 8) & 0xff] ^
           tfqfarray[2][(a >> 16) & 0xff] ^ tfqfarray[3][(a >> 24) & 0xff];
}

uint32_t JBTwofish::hFunc(uint32_t a, const uint32_t *x) noexcept {
    uint8_t y0 = a;
    uint8_t y1 = a >> 8;
    uint8_t y2 = a >> 16;
    uint8_t y3 = a >> 24;
    y0 = tfq1array[y0] ^ (x[3] & 0xff);
    y1 = tfq0array[y1] ^ ((x[3] >> 8) & 0xff);
    y2 = tfq0array[y2] ^ ((x[3] >> 16) & 0xff);
    y3 = tfq1array[y3] ^ ((x[3] >> 24) & 0xff);
    y0 = tfq1array[y0] ^ (x[2] & 0xff);
    y1 = tfq1array[y1] ^ ((x[2] >> 8) & 0xff);
    y2 = tfq0array[y2] ^ ((x[2] >> 16) & 0xff);
    y3 = tfq0array[y3] ^ ((x[2] >> 24) & 0xff);
    y0 = tfq1array[tfq0array[tfq0array[y0] ^ (x[1] & 0xff)] ^ (x[0] & 0xff)];
    y1 = tfq0array[tfq0array[tfq1array[y1] ^ ((x[1] >> 8) & 0xff)] ^ ((x[0] >> 8) & 0xff)];
    y2 = tfq1array[tfq1array[tfq0array[y2] ^ ((x[1] >> 16) & 0xff)] ^ ((x[0] >> 16) & 0xff)];
    y3 = tfq0array[tfq1array[tfq1array[y3] ^ ((x[1] >> 24) & 0xff)] ^ ((x[0] >> 24) & 0xff)];
    const uint8_t z0 = tfq3array[y0] ^ y1 ^ tfq3array[y2] ^ tfq2array[y3];
    const uint8_t z1 = tfq3array[y0] ^ tfq2array[y1] ^ y2 ^ tfq3array[y3];
    const uint8_t z2 = tfq2array[y0] ^ tfq3array[y1] ^ tfq3array[y2] ^ y3;
    const uint8_t z3 = y0 ^ tfq3array[y1] ^ tfq2array[y2] ^ tfq2array[y3];
    return static_cast<uint32_t>((z0 << 24) ^ (z1 << 16) ^ (z2 << 8) ^ z3);
}

void JBTwofish::encryptRound(const uint32_t r0,
                             const uint32_t r1,
                             uint32_t *r2,
                             uint32_t *r3,
                             const uint32_t r) {
    const uint32_t t0 = gFunc(r0);
    const uint32_t t1 = gFunc(rol32(r1, 8));
    *r2 = ror32((*r2 ^ (t0 + t1 + tfk0array[2 * r + 8])), 1);
    *r3 = rol32(*r3, 1) ^ (2 * t1 + t0 + tfk0array[2 * r + 9]);
}

void JBTwofish::decryptRound(const uint32_t r0,
                             const uint32_t r1,
                             uint32_t *r2,
                             uint32_t *r3,
                             const uint32_t r) {
    const uint32_t t0 = gFunc(r0);
    const uint32_t t1 = gFunc(rol32(r1, 8));
    *r2 = rol32(*r2, 1) ^ (t0 + t1 + tfk0array[2 * r + 8]);
    *r3 = ror32((*r3 ^ (t0 + 2 * t1 + tfk0array[2 * r + 9])), 1);
}

void JBTwofish::encryptBase(std::vector<uint8_t>& p) {
    auto r0 = static_cast<uint32_t>(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
    auto r1 = static_cast<uint32_t>(p[4] | (p[5] << 8) | (p[6] << 16) | (p[7] << 24));
    auto r2 = static_cast<uint32_t>(p[8] | (p[9] << 8) | (p[10] << 16) | (p[11] << 24));
    auto r3 = static_cast<uint32_t>(p[12] | (p[13] << 8) | (p[14] << 16) | (p[15] << 24));
    r3 ^= tfk0array[3];
    r2 ^= tfk0array[2];
    r1 ^= tfk0array[1];
    r0 ^= tfk0array[0];
    encryptRound(r0, r1, &r2, &r3, 0);
    encryptRound(r2, r3, &r0, &r1, 1);
    encryptRound(r0, r1, &r2, &r3, 2);
    encryptRound(r2, r3, &r0, &r1, 3);
    encryptRound(r0, r1, &r2, &r3, 4);
    encryptRound(r2, r3, &r0, &r1, 5);
    encryptRound(r0, r1, &r2, &r3, 6);
    encryptRound(r2, r3, &r0, &r1, 7);
    encryptRound(r0, r1, &r2, &r3, 8);
    encryptRound(r2, r3, &r0, &r1, 9);
    encryptRound(r0, r1, &r2, &r3, 10);
    encryptRound(r2, r3, &r0, &r1, 11);
    encryptRound(r0, r1, &r2, &r3, 12);
    encryptRound(r2, r3, &r0, &r1, 13);
    encryptRound(r0, r1, &r2, &r3, 14);
    encryptRound(r2, r3, &r0, &r1, 15);
    r1 ^= tfk0array[7];
    r0 ^= tfk0array[6];
    r3 ^= tfk0array[5];
    r2 ^= tfk0array[4];
    p[0] = r2;
    p[1] = (r2 >> 8);
    p[2] = (r2 >> 16);
    p[3] = (r2 >> 24);
    p[4] = r3;
    p[5] = (r3 >> 8);
    p[6] = (r3 >> 16);
    p[7] = (r3 >> 24);
    p[8] = r0;
    p[9] = (r0 >> 8);
    p[10] = (r0 >> 16);
    p[11] = (r0 >> 24);
    p[12] = r1;
    p[13] = (r1 >> 8);
    p[14] = (r1 >> 16);
    p[15] = (r1 >> 24);
}

void JBTwofish::decryptBase(std::vector<uint8_t>& p) {
    auto r0 = static_cast<uint32_t>(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
    auto r1 = static_cast<uint32_t>(p[4] | (p[5] << 8) | (p[6] << 16) | (p[7] << 24));
    auto r2 = static_cast<uint32_t>(p[8] | (p[9] << 8) | (p[10] << 16) | (p[11] << 24));
    auto r3 = static_cast<uint32_t>(p[12] | (p[13] << 8) | (p[14] << 16) | (p[15] << 24));
    r3 ^= tfk0array[7];
    r2 ^= tfk0array[6];
    r1 ^= tfk0array[5];
    r0 ^= tfk0array[4];
    decryptRound(r0, r1, &r2, &r3, 15);
    decryptRound(r2, r3, &r0, &r1, 14);
    decryptRound(r0, r1, &r2, &r3, 13);
    decryptRound(r2, r3, &r0, &r1, 12);
    decryptRound(r0, r1, &r2, &r3, 11);
    decryptRound(r2, r3, &r0, &r1, 10);
    decryptRound(r0, r1, &r2, &r3, 9);
    decryptRound(r2, r3, &r0, &r1, 8);
    decryptRound(r0, r1, &r2, &r3, 7);
    decryptRound(r2, r3, &r0, &r1, 6);
    decryptRound(r0, r1, &r2, &r3, 5);
    decryptRound(r2, r3, &r0, &r1, 4);
    decryptRound(r0, r1, &r2, &r3, 3);
    decryptRound(r2, r3, &r0, &r1, 2);
    decryptRound(r0, r1, &r2, &r3, 1);
    decryptRound(r2, r3, &r0, &r1, 0);
    r1 ^= tfk0array[3];
    r0 ^= tfk0array[2];
    r3 ^= tfk0array[1];
    r2 ^= tfk0array[0];
    p[0] = r2;
    p[1] = (r2 >> 8);
    p[2] = (r2 >> 16);
    p[3] = (r2 >> 24);
    p[4] = r3;
    p[5] = (r3 >> 8);
    p[6] = (r3 >> 16);
    p[7] = (r3 >> 24);
    p[8] = r0;
    p[9] = (r0 >> 8);
    p[10] = (r0 >> 16);
    p[11] = (r0 >> 24);
    p[12] = r1;
    p[13] = (r1 >> 8);
    p[14] = (r1 >> 16);
    p[15] = (r1 >> 24);
}

std::vector<uint8_t> JBTwofish::encryptECB(const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> result {};
    result.reserve(payload.size() + TWOFISH_BLOCK_SIZE);
    const uint64_t l = payload.size();
    uint8_t pad = 0x00;
    uint64_t i = 0, j, k;
    std::vector<uint8_t> pt(TWOFISH_BLOCK_SIZE, 0);
    while (i < l) {
        pad = (i + TWOFISH_BLOCK_SIZE < l) ? 0 : static_cast<uint8_t>(i + TWOFISH_BLOCK_SIZE - l);
        for (j = i, k = 0; j < i + TWOFISH_BLOCK_SIZE; j++) {
            const uint8_t c = (j < l) ? payload[j] : pad;
            pt[k++] = c;
        }
        i += TWOFISH_BLOCK_SIZE;
        encryptBase(pt);
        result.insert(result.end(), pt.begin(), pt.end());
    }
    return result;
}

std::vector<uint8_t> JBTwofish::decryptECB(const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> result {};
    result.reserve(payload.size() + TWOFISH_BLOCK_SIZE);
    const uint64_t l = payload.size();
    uint8_t c;
    uint64_t i = 0, j, k;
    std::vector<uint8_t> pt(TWOFISH_BLOCK_SIZE, 0);
    while (i < l) {
        for (j = i, k = 0; j < i + TWOFISH_BLOCK_SIZE; j++) {
            c = (j < l) ? payload[j] : 0;
            pt[k++] = c;
        }
        i += TWOFISH_BLOCK_SIZE;
        decryptBase(pt);
        result.insert(result.end(), pt.begin(), pt.end());
    }
    j = result.size();
    if (j > 0) {
        if (const uint8_t p = result[j - 1]; (p <= TWOFISH_BLOCK_SIZE) && (p < j)) {
            int z = p;
            c = 0;
            while (z >= 0) {
                if (result[j - z - 1] == p) c++;
                z--;
            }
            if (c == p) result.resize(j - p);
        }
    }
    return result;
}

std::vector<uint8_t> JBTwofish::encryptCBC(const std::vector<uint8_t>& payload,
                                           const std::vector<uint8_t>& vinit) {
    std::vector<uint8_t> result {};
    result.reserve(payload.size() + TWOFISH_BLOCK_SIZE);
    const uint64_t l = payload.size();
    uint8_t pad = 0x00;
    uint64_t i = 0, j, k;
    std::vector<uint8_t> pt(TWOFISH_BLOCK_SIZE, 0);
    std::vector<uint8_t> vxor(TWOFISH_BLOCK_SIZE, 0);
    if (!vinit.empty()) {
        for (j = 0, k = 0; j < TWOFISH_BLOCK_SIZE; j++) {
            if (k >= vinit.size()) k = 0;
            vxor[j] = vinit[k++];
        }
    }
    while (i < l) {
        pad = (i + TWOFISH_BLOCK_SIZE < l) ? 0 : static_cast<uint8_t>(i + TWOFISH_BLOCK_SIZE - l);
        for (j = i, k = 0; j < i + TWOFISH_BLOCK_SIZE; j++) {
            const uint8_t c = (j < l) ? payload[j] : pad;
            pt[k++] = c;
        }
        i += TWOFISH_BLOCK_SIZE;
        for (j = 0; j < TWOFISH_BLOCK_SIZE; j++) pt[j] ^= vxor.at(j);
        encryptBase(pt);
        vxor = pt;
        result.insert(result.end(), pt.begin(), pt.end());
    }
    return result;
}

std::vector<uint8_t> JBTwofish::decryptCBC(const std::vector<uint8_t>& payload,
                                           const std::vector<uint8_t>& vinit) {
    std::vector<uint8_t> result {};
    result.reserve(payload.size() + TWOFISH_BLOCK_SIZE);
    const uint64_t l = payload.size();
    uint8_t c;
    uint64_t i = 0, j, k;
    std::vector<uint8_t> pt(TWOFISH_BLOCK_SIZE, 0);
    std::vector<uint8_t> ptmp(TWOFISH_BLOCK_SIZE, 0);
    std::vector<uint8_t> vxor(TWOFISH_BLOCK_SIZE, 0);
    if (!vinit.empty()) {
        for (j = 0, k = 0; j < TWOFISH_BLOCK_SIZE; j++) {
            if (k >= vinit.size()) k = 0;
            ptmp[j] = vinit[k++];
        }
    }
    while (i < l) {
        for (j = i, k = 0; j < i + TWOFISH_BLOCK_SIZE; j++) {
            c = (j < l) ? payload[j] : 0;
            pt[k++] = c;
        }
        i += TWOFISH_BLOCK_SIZE;
        vxor = pt;
        decryptBase(pt);
        for (j = 0; j < TWOFISH_BLOCK_SIZE; j++) pt[j] ^= ptmp.at(j);
        ptmp = vxor;
        result.insert(result.end(), pt.begin(), pt.end());
    }
    j = result.size();
    if (j > 0) {
        if (const uint8_t p = result[j - 1]; (p <= TWOFISH_BLOCK_SIZE) && (p < j)) {
            int z = p;
            c = 0;
            while (z >= 0) {
                if (result[j - z - 1] == p) c++;
                z--;
            }
            if (c == p) result.resize(j - p);
        }
    }
    return result;
}

void JBTwofish::initialize(const std::vector<uint8_t>& p) {
    constexpr int k = 4;
    memcpy(&tfq0array[0], &TFQ0ARRAY[0], std::size(TFQ0ARRAY));
    memcpy(&tfq1array[0], &TFQ1ARRAY[0], std::size(TFQ1ARRAY));
    memcpy(&tfq2array[0], &TFQ2ARRAY[0], std::size(TFQ2ARRAY));
    memcpy(&tfq3array[0], &TFQ3ARRAY[0], std::size(TFQ3ARRAY));
    int i;
    uint32_t mke[4] {};
    uint32_t mko[4] {};
    uint32_t key[8] {};
    uint8_t vec[8] {};
    uint8_t mkey[TWOFISH_KEY_SIZE];
    int j = 0;
    for (i = 0; i < TWOFISH_KEY_SIZE; i++) {
        if (j >= p.size()) j = 0;
        mkey[i] = p[j++];
    }
    for (i = 0; i < TWOFISH_KEY_SIZE / 4; i++) {
        key[i] = static_cast<uint32_t>((mkey[4 * i + 3] << 24) | (mkey[4 * i + 2] << 16) |
                                       (mkey[4 * i + 1] << 8) | mkey[4 * i]);
    }
    for (i = 0; i < k; i++) {
        mke[i] = key[2 * i];
        mko[i] = key[2 * i + 1];
    }
    for (i = 0; i < k; i++) {
        for (j = 0; j < 4; j++) vec[j] = static_cast<uint8_t>(mke[i] >> (j << 3));
        for (j = 0; j < 4; j++) vec[j + 4] = static_cast<uint8_t>(mko[i] >> (j << 3));
        tfs0array[k - i - 1] = rsMatrixMultiply(vec);
    }
    for (uint32_t z = 0; z < 20; z++) {
        const uint32_t a = hFunc(2 * z * RHO, mke);
        const uint32_t b = rol32(hFunc((2 * z * RHO + RHO), mko), 8);
        tfk0array[2 * z] = a + b;
        tfk0array[2 * z + 1] = rol32((a + 2 * b), 9);
    }
    for (i = 0; i < std::size(TFQ0ARRAY); i++) {
        uint8_t y0 = i;
        uint8_t y1 = i;
        uint8_t y2 = i;
        uint8_t y3 = i;
        y0 = tfq1array[y0] ^ (tfs0array[3] & 0xff);
        y1 = tfq0array[y1] ^ ((tfs0array[3] >> 8) & 0xff);
        y2 = tfq0array[y2] ^ ((tfs0array[3] >> 16) & 0xff);
        y3 = tfq1array[y3] ^ ((tfs0array[3] >> 24) & 0xff);
        y0 = tfq1array[y0] ^ (tfs0array[2] & 0xff);
        y1 = tfq1array[y1] ^ ((tfs0array[2] >> 8) & 0xff);
        y2 = tfq0array[y2] ^ ((tfs0array[2] >> 16) & 0xff);
        y3 = tfq0array[y3] ^ ((tfs0array[2] >> 24) & 0xff);
        y0 = tfq1array[tfq0array[tfq0array[y0] ^ (tfs0array[1] & 0xff)] ^ (tfs0array[0] & 0xff)];
        y1 = tfq0array[tfq0array[tfq1array[y1] ^ ((tfs0array[1] >> 8) & 0xff)] ^ ((tfs0array[0] >> 8) & 0xff)];
        y2 = tfq1array[tfq1array[tfq0array[y2] ^ ((tfs0array[1] >> 16) & 0xff)] ^ ((tfs0array[0] >> 16) & 0xff)];
        y3 = tfq0array[tfq1array[tfq1array[y3] ^ ((tfs0array[1] >> 24) & 0xff)] ^ ((tfs0array[0] >> 24) & 0xff)];
        tfqfarray[0][i] = static_cast<uint32_t>((tfq3array[y0] << 24) | (tfq3array[y0] << 16) |
                                                (tfq2array[y0] << 8) | y0);
        tfqfarray[1][i] = static_cast<uint32_t>((y1 << 24) | (tfq2array[y1] << 16) |
                                                (tfq3array[y1] << 8) | tfq3array[y1]);
        tfqfarray[2][i] = static_cast<uint32_t>((tfq3array[y2] << 24) | (y2 << 16) |
                                                (tfq3array[y2] << 8) | tfq2array[y2]);
        tfqfarray[3][i] = static_cast<uint32_t>((tfq2array[y3] << 24) |
                                                (tfq3array[y3] << 16) | (y3 << 8) | tfq2array[y3]);
    }
}

void JBTwofish::initialize(const std::string& s) {
    initialize(std::vector<uint8_t>(s.begin(), s.end()));
}

/////////////////////////////////////////////////////////////////////////////
// Name:        jbhmac.h
// Purpose:     Cryptographic methods, HMAC, PBKDF2 (header)
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include "jbsha.h"

class JBHmac {
public:
    static std::vector<uint8_t> pbkdf2HmacSha256(const std::vector<uint8_t>& password,
                                                 const std::vector<uint8_t>& salt,
                                                 uint32_t iterations,
                                                 size_t dkLen);
    static std::vector<uint8_t> hmacSha256(const std::vector<uint8_t>& key,
                                           const std::vector<uint8_t>& message);
};

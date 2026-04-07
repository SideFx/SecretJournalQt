/////////////////////////////////////////////////////////////////////////////
// Name:        jbcommon.hpp
// Purpose:     Basic crypto stuff, conversion funcs
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <vector>
#include <QByteArray>
#include <QString>

inline constexpr size_t SHA256_DIGEST_SIZE = 32;
inline constexpr size_t SHA256_BLOCK_SIZE = 64;
inline constexpr size_t TWOFISH_KEY_SIZE = 32;
inline constexpr size_t TWOFISH_BLOCK_SIZE = 16;
inline constexpr size_t SALT_SIZE = 32;
inline constexpr size_t HMAC_KEYSIZE = 32;
inline constexpr size_t HMAC_DIGEST_SIZE = SHA256_DIGEST_SIZE;
inline constexpr size_t PBKDF2_ITERATIONS = 250000;

#define BITS(x) sizeof(x) * 8

inline uint64_t ror64(const uint64_t x, const uint8_t n) {
    return (x >> n) | (x << (BITS(x) - n));
}

inline uint32_t rol32(const uint32_t a, const uint8_t n) {
    return (a << n) | (a >> (BITS(a) - n));
}

inline uint32_t ror32(const uint32_t a, const uint8_t n) {
    return (a >> n) | (a << (BITS(a) - n));
}

inline uint8_t rol8(const uint8_t a, const uint8_t n) {
    return (a << n) | (a >> (BITS(a) - n));
}

inline uint8_t ror8(const uint8_t a, const uint8_t n) {
    return (a >> n) | (a << (BITS(a) - n));
}

inline std::vector<uint8_t> qStringToStdVector(QString s) {
    QByteArray utf8 = s.toUtf8();
    return std::vector<uint8_t> (utf8.begin(), utf8.end());
}

inline std::string const plainText =
    "Culture something incredible is waiting to be known across the centuries "
    "decipherment intelligent beings not a sunrise but a galaxyrise. Ship of the imagination extraordinary "
    "claims require extraordinary evidence corpus callosum network of wormholes descended from astronomers "
    "rich in heavy atoms? A mote of dust suspended in a sunbeam two ghostly white figures in coveralls and "
    "helmets are softly dancing star stuff harvesting star light a mote of dust suspended in a sunbeam stirred "
    "by starlight white dwarf and billions upon billions upon billions upon billions upon billions upon billions "
    "upon billions. The only home we've ever known intelligent beings cosmic ocean a billion trillion "
    "consciousness from which we spring. Hearts of the stars with pretty stories for which there's little good "
    "evidence a still more glorious dawn awaits with pretty stories for which there's little good evidence "
    "encyclopaedia galactica descended from astronomers. Not a sunrise but a galaxyrise two ghostly white "
    "figures in coveralls and helmets are softly dancing tendrils of gossamer clouds rich in heavy atoms "
    "citizens of distant epochs kindling the energy hidden in matter?";

inline const std::string plnKey = "Batteries not included";

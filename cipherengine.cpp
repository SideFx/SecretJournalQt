/////////////////////////////////////////////////////////////////////////////
// Name:        cipherengine.cpp
// Purpose:     Cryptographic engine
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#include "cipherengine.h"
#include <QDateTime>
#include "jbcrypto/jbhmac.h"
#include "jbcrypto/jbtwofish.h"
#include "io.h"
#include <random>

void CipherEngine::initializeVector(const std::vector<uint8_t>& passphrase, behavior mode) {
    if (mode == SET) {
        m_iv = {};
        std::memcpy(&m_iv.prefix, &prefixData, PREFIX_DATA_SIZE);
        m_iv.salt.resize(SALT_SIZE);
        m_iv.iv.resize(TWOFISH_BLOCK_SIZE);
        m_iv.iter = PBKDF2_ITERATIONS;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> dist(0, 255);
        for (auto& b : m_iv.salt) b = static_cast<uint8_t>(dist(gen));
        for (auto& b : m_iv.iv) b = static_cast<uint8_t>(dist(gen));
        m_iv.set = true;
    }
    if (m_iv.set) {
        std::vector<uint8_t> bigKey = JBHmac::pbkdf2HmacSha256(passphrase,
                                                               m_iv.salt, m_iv.iter,
                                                               TWOFISH_KEY_SIZE + HMAC_KEYSIZE);
        m_iv.pass_key = std::vector<uint8_t>(bigKey.begin(), bigKey.begin() + TWOFISH_KEY_SIZE);
        m_iv.hmac_key = std::vector<uint8_t>(bigKey.begin() + TWOFISH_KEY_SIZE,
                                             bigKey.begin() + TWOFISH_KEY_SIZE + HMAC_KEYSIZE);
    }
}

bool CipherEngine::checkIntegrity() {
    return m_iv.set;
}

std::vector<uint8_t> CipherEngine::createHmacMessage() {
    std::vector<uint8_t> message;
    message.reserve(PREFIX_DATA_SIZE + sizeof(uint32_t) + TWOFISH_BLOCK_SIZE +
                    SALT_SIZE + sizeof(uint32_t) + m_iv.ciphertext.size());
    // append prefix
    message.insert(message.begin(), m_iv.prefix.cbegin(), m_iv.prefix.cend());
    // append iterations
    uint8_t iterBytes[sizeof(uint32_t)];
    memcpy(iterBytes, &m_iv.iter, sizeof(uint32_t));
    message.insert(message.end(), iterBytes, iterBytes + sizeof(uint32_t));
    // append vector
    message.insert(message.end(), m_iv.iv.begin(), m_iv.iv.end());
    // append salt
    message.insert(message.end(), m_iv.salt.begin(), m_iv.salt.end());
    // append ciphertext size
    uint8_t lenBytes[sizeof(uint32_t)];
    memcpy(lenBytes, &m_iv.ciphertextlen, sizeof(uint32_t));
    message.insert(message.end(), lenBytes, lenBytes + sizeof(uint32_t));
    // append ciphertext
    message.insert(message.end(), m_iv.ciphertext.begin(), m_iv.ciphertext.end());
    return message;
}

bool CipherEngine::cipher(const std::vector<uint8_t>& plaintext) {
    if (!m_iv.set || plaintext.size() == 0 ||
        m_iv.iv.size() != TWOFISH_BLOCK_SIZE ||
        m_iv.salt.size() != SALT_SIZE ||
        m_iv.pass_key.size() != TWOFISH_KEY_SIZE ||
        m_iv.hmac_key.size() != HMAC_KEYSIZE ) return false;
    JBTwofish tf(m_iv.pass_key);
    m_iv.ciphertext = tf.encryptCBC(plaintext, m_iv.iv);
    m_iv.ciphertextlen = m_iv.ciphertext.size();
    std::vector<uint8_t> message = createHmacMessage();
    m_iv.hmac = JBHmac::hmacSha256(m_iv.hmac_key, message);
    return m_iv.hmac.size() == HMAC_DIGEST_SIZE;
}

QByteArray CipherEngine::serializePayload() const {
    QByteArray out;
    // iter (uint32_t)
    out.append(reinterpret_cast<const char*>(&m_iv.iter), sizeof(uint32_t));
    // iv
    out.append(reinterpret_cast<const char*>(m_iv.iv.data()), m_iv.iv.size());
    // salt
    out.append(reinterpret_cast<const char*>(m_iv.salt.data()), m_iv.salt.size());
    // ciphertext length
    out.append(reinterpret_cast<const char*>(&m_iv.ciphertextlen), sizeof(uint32_t));
    // ciphertext
    out.append(reinterpret_cast<const char*>(m_iv.ciphertext.data()), m_iv.ciphertext.size());
    // hmac (32 bytes)
    out.append(reinterpret_cast<const char*>(m_iv.hmac.data()), m_iv.hmac.size());
    return out;
}

QByteArray CipherEngine::toFileFormat() const {
    QByteArray file;
    // prefix as plain text
    file.append(reinterpret_cast<const char*>(m_iv.prefix.data()), m_iv.prefix.size());
    // rest Base64
    QByteArray payload = serializePayload();
    QByteArray encoded = payload.toBase64();
    file.append(encoded);
    return file;
}

int CipherEngine::encrytAndSave(const QString& filename, const QByteArray& data) {
    const std::vector<uint8_t> payload(data.begin(), data.end());
    if (!cipher(payload)) return MSG_INTERNAL_ERROR;
    QByteArray out = toFileFormat();
    Io io;
    return io.save(filename, out);
}

int CipherEngine::load(const QString& filename) {
    QByteArray inData;
    Io io;
    m_iv = {};
    std::memcpy(&m_iv.prefix, &prefixData, PREFIX_DATA_SIZE);
    int iores = io.load(filename, &inData);
    if (iores != MSG_OK) return iores;
    if (inData.size() <= PREFIX_DATA_SIZE) return MSG_TOO_SHORT_ERROR; // preliminary test
    QByteArray header = inData.left(PREFIX_DATA_SIZE);
    for (int i = 0; i < PREFIX_DATA_SIZE; ++i) {
        if (prefixData[i] != header.at(i)) return MSG_SIGNATURE_ERROR;
    }
    QByteArray binary = QByteArray::fromBase64(inData.mid(PREFIX_DATA_SIZE));
    if (binary.isEmpty()) return MSG_FILE_CORRUPT_ERROR;
    int minSize = sizeof(uint32_t) + // iter
                  TWOFISH_BLOCK_SIZE + // iv
                  SALT_SIZE + // salt
                  sizeof(uint32_t) + // ciphertextlen
                  HMAC_DIGEST_SIZE + // hmac digest size
                  TWOFISH_BLOCK_SIZE; // at least one block
    if (binary.size() < minSize) return MSG_TOO_SHORT_ERROR;
    std::vector<uint8_t> vec(binary.begin(), binary.end());
    std::span<const uint8_t> s(vec);
    size_t offset = 0;
    auto read_u32 = [&vec](size_t off) -> uint32_t {
        int shift = 0;
        uint32_t res = 0;
        for (int i = 0; i < sizeof(uint32_t); ++i) {
            res |= static_cast<uint32_t>(vec[off + i]) << shift;
            shift += 8;
        }
        return res;
    };
    // iterations
    m_iv.iter = read_u32(offset);
    offset += sizeof(uint32_t);
    // iv (initialization vector)
    m_iv.iv = std::vector<uint8_t>(s.begin() + offset, s.begin() + offset + TWOFISH_BLOCK_SIZE);
    offset += TWOFISH_BLOCK_SIZE;
    // salt
    m_iv.salt = std::vector<uint8_t>(s.begin() + offset, s.begin() + offset + SALT_SIZE);
    offset += SALT_SIZE;
    // ciphertext size
    m_iv.ciphertextlen = read_u32(offset);
    offset += sizeof(uint32_t);
    if (m_iv.ciphertextlen != vec.size() - offset - HMAC_DIGEST_SIZE) return MSG_FILE_CORRUPT_ERROR;
    // ciphertext
    m_iv.ciphertext = std::vector<uint8_t>(s.begin() + offset, s.begin() +
                                           offset + m_iv.ciphertextlen);
    offset += m_iv.ciphertextlen;
    // HMAC
    m_iv.hmac = std::vector<uint8_t>(s.begin() + offset, s.begin() + offset + HMAC_DIGEST_SIZE);
    offset += HMAC_DIGEST_SIZE; // offset should now be equal to vec.size()
    if (offset != vec.size()) return MSG_FILE_CORRUPT_ERROR;
    m_iv.set = true;
    return MSG_OK;
}

int CipherEngine::decrypt(QByteArray* ba) {
    if (!m_iv.set) return MSG_INTERNAL_ERROR;
    std::vector<uint8_t> message = createHmacMessage();
    std::vector<uint8_t> hmac = JBHmac::hmacSha256(m_iv.hmac_key, message);
    if (hmac != m_iv.hmac) {
        m_iv.set = false;
        return MSG_DECRYPTION_ERROR;
    }
    JBTwofish tf(m_iv.pass_key);
    std::vector<uint8_t> plain = tf.decryptCBC(m_iv.ciphertext, m_iv.iv);
    ba->resize(static_cast<int>(plain.size()));
    std::memcpy(ba->data(), plain.data(), plain.size());
    return MSG_OK;
}

void CipherEngine::resetVector() {
    m_iv = {};
}

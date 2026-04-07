/////////////////////////////////////////////////////////////////////////////
// Name:        cipherengine.h
// Purpose:     Cryptographic engine (header)
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "jbcrypto/jbcommon.hpp"
#include "constants.h"
#include "io.h"
#include <QByteArray>

class CipherEngine {

public:
    void initializeVector(const std::vector<uint8_t>& passphrase, behavior mode);
    bool checkIntegrity();
    bool cipher(const std::vector<uint8_t>& plainText);
    int encrytAndSave(const QString& filename, const QByteArray& data);
    int load(const QString& filename);
    int decrypt(QByteArray* data);
    void resetVector();

private:
    static constexpr int PREFIX_DATA_SIZE = 30;
    static constexpr uint8_t prefixData[PREFIX_DATA_SIZE] = { 'J', 'B', '*',
        'S', 'e', 'C', 'r', 'E', 't', 'J', 'o', 'U', 'r', 'N', 'a', 'L', 'e', 'N', 'c',
        'R', 'y', 'P', 't', 'E', 'd', 'D', 'a', 'T', 'a', '*'
    };
    struct s_vault {
        std::vector<uint8_t> data;
        behavior mode;
    };
    struct s_vector {
        bool set = false;
        std::array<uint8_t, PREFIX_DATA_SIZE> prefix;
        uint32_t iter;
        std::vector<uint8_t> iv;
        std::vector<uint8_t> salt;
        std::vector<uint8_t> pass_key;
        std::vector<uint8_t> hmac_key;
        std::vector<uint8_t> hmac;
        uint32_t ciphertextlen;
        std::vector<uint8_t> ciphertext;
    };

    QByteArray serializePayload() const;
    QByteArray toFileFormat() const;
    std::vector<uint8_t> createHmacMessage();

    s_vector m_iv;
};



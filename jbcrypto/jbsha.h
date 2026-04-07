/////////////////////////////////////////////////////////////////////////////
// Name:        jbsha.h
// Purpose:     Header for jbsha.cpp
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-04
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "jbcommon.hpp"

class JBSha {

public:
    explicit JBSha();
    std::vector<uint8_t> computeSha256(const std::vector<uint8_t>& payload);
    std::vector<uint8_t> computeSha256(const std::string& payload);
};



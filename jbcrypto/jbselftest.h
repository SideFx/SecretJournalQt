/////////////////////////////////////////////////////////////////////////////
// Name:        jbselftest.h
// Purpose:     Selftests for Sha256 & Twofish (header)
// Author:      Jan Buchholz
// Created:     2026-04-03
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#pragma once

class JBSelfTest {

public:
    explicit JBSelfTest();
    bool sha256Selftest();
    bool pbkdf2HmacSha256Selftest();
    bool twofishStandardSelftest();
    bool twofishECBSelftest();
    bool twofishCBCSelftest();
    bool doAllTests();
};


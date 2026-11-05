/////////////////////////////////////////////////////////////////////////////
// Name:        appio.h
// Purpose:     File load/save methods (header)
// Author:      Jan Buchholz
// Created:     2025-11-19
// Changed:     2026-11-05
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QString>
#include <QObject>
#include <QByteArray>

class AppIo {

public:
    int save(const QString& fileName, const QByteArray& payload);
    int load(const QString& fileName, QByteArray *payload);

};


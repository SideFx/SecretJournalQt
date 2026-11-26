/////////////////////////////////////////////////////////////////////////////
// Name:        jbstyles.hpp
// Purpose:     Stylesheets and SVG icon color change
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-11-26
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QString>

inline QString styleToolBar = R"(
        QToolBar > QToolButton {
            border-radius: 3px;
            padding: 2px 2px;
        }
        QToolBar > QToolButton:hover {
            background-color: gray;
            border-color: auto;
        }
        QToolBar > QToolButton:pressed {
            background-color: darkGray;
            border-color: auto;
        }
        QToolBar > QToolButton:checked {
            background-color: darkGray;
            border-color: auto;
        }
        QToolBar {
            border: 0px;
            padding: 1px 1px;
        }
)";

inline QString styleStatusBar = R"(
        QStatusBar {
            border: 0px;
            padding: 1px 1px;
        }
)";

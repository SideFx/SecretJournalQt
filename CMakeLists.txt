cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "SecretJournalQt")

project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Svg)
find_package(Qt6 REQUIRED COMPONENTS Core)

if(APPLE)
set(MACOSX_BUNDLE_ICON_FILE app.icns)
set(APP_ICON "${CMAKE_SOURCE_DIR}/app.icns")
set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

    qt_add_executable(${PROJECT_NAME}
        WIN32 MACOSX_BUNDLE
        MANUAL_FINALIZATION
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        stylesheets.h
        constants.h
        jbpreferences.cpp jbpreferences.h
        resources.qrc
        dialogs.h dialogs.cpp
        uilogic.h uilogic.cpp
        jblistwidget.h jblistwidget.cpp
        listfontdialog.h listfontdialog.cpp listfontdialog.ui
        listitemdialog.h listitemdialog.cpp listitemdialog.ui
        iconlist.h iconlist.cpp
        app.icns app.ico appicon.rc
        appicon.rc
        io.h io.cpp
        aboutdialog.h aboutdialog.cpp aboutdialog.ui
        passworddialog.ui
        passworddialog.h
        passworddialog.cpp
        cipherengine.h cipherengine.cpp
        jbcrypto/jbcommon.hpp jbcrypto/jbsha.cpp jbcrypto/jbsha.h
        jbcrypto/jbtwofish.h jbcrypto/jbtwofish.cpp
        jbcrypto/jbselftest.h jbcrypto/jbselftest.cpp
        jbcrypto/jbhmac.h jbcrypto/jbhmac.cpp
    )
endif()

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE app.icns)
    set(APP_ICON "${CMAKE_SOURCE_DIR}/app.icns")
    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Network Qt6::Svg)

set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
#    UPDATE_TRANSLATIONS .
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ---- macOS Deployment ----
if(APPLE)
    get_filename_component(QT_BIN_DIR "${Qt6_DIR}/../../../bin" ABSOLUTE)
    find_program(MACDEPLOYQT macdeployqt6 HINTS "${QT_BIN_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${MACDEPLOYQT} "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>" -verbose=2
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND codesign --force --deep --sign - --timestamp=none "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
    )
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

cmake_minimum_required(VERSION 3.16)
project(BrewNBites CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable automatic compilation of Qt Meta-Object System (MOC)
set(CMAKE_AUTOMOC ON)

# Enable automatic compilation of Qt Resource files (.qrc) — WAJIB agar
# gambar/aset di resource.qrc (logo, QR code, dsb.) benar-benar di-bundle
# ke dalam executable dan bisa diakses lewat path ":/images/...".
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------------------------
# Qt6 Path configuration (Windows / MinGW Cross-compilation)
# ---------------------------------------------------------------------------
if(WIN32)
    # Coba deteksi Qt6 Windows binaries di vendor/qt6 (opsional, jika ada)
    file(GLOB QT6_CANDIDATES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qt6/[0-9]*/mingw_64")
    list(LENGTH QT6_CANDIDATES CANDIDATE_COUNT)
    if(CANDIDATE_COUNT GREATER 0)
        list(GET QT6_CANDIDATES 0 DETECTED_QT6_DIR)
        message(STATUS "Found Qt6 Windows MinGW installation: ${DETECTED_QT6_DIR}")
        set(CMAKE_PREFIX_PATH "${DETECTED_QT6_DIR}")
    else()
        # Tidak ada folder vendor — biarkan CMake/Qt Creator pakai
        # CMAKE_PREFIX_PATH yang sudah terdeteksi otomatis dari kit Qt yang aktif
        # (mis. C:/Qt/6.11.0/mingw_64). Jangan hardcode path vendor yang belum tentu ada.
        message(STATUS "vendor/qt6 tidak ditemukan, memakai Qt6 dari kit aktif: ${CMAKE_PREFIX_PATH}")
    endif()
endif()

# Find Qt6 components
find_package(Qt6 COMPONENTS Widgets Network REQUIRED)

# Qt6_DIR (diset otomatis oleh find_package) biasanya berbentuk
# <QT_ROOT>/lib/cmake/Qt6 — turunkan root instalasi Qt yang SUNGGUH dipakai
# dari sini, supaya tidak bergantung pada CMAKE_PREFIX_PATH yang bisa salah/kosong.
if(WIN32 AND Qt6_DIR)
    get_filename_component(QT6_INSTALL_ROOT "${Qt6_DIR}/../../.." ABSOLUTE)
    message(STATUS "Qt6 install root terdeteksi: ${QT6_INSTALL_ROOT}")
endif()

# List source files explicitly for reliability and clean builds
set(SRCS
    src/main.cpp
    src/mainwindow.cpp
    src/registerpage.cpp
    src/orderpage.cpp
    src/paymentpage.cpp
    src/menuitem.cpp
    src/ordermanager.cpp
    src/historymanager.cpp
    src/historypage.cpp
    src/receiptdialog.cpp
    src/strukhttpserver.cpp
)

set(HEADERS
    src/mainwindow.h
    src/registerpage.h
    src/orderpage.h
    src/paymentpage.h
    src/menuitem.h
    src/ordermanager.h
    src/theme.h
    src/historymanager.h
    src/historypage.h
    src/receiptdialog.h
    src/strukhttpserver.h
)

# Add executable
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS}
    src/qrcodegen.h
    src/resource.qrc)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network)

# Optimize Qt include orders
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_AUTOMOC_RELAXED_INCLUDE_ORDER)

# ---------------------------------------------------------------------------
# Windows Post-Build Step: Copy Runtime DLLs and Plugins
# ---------------------------------------------------------------------------
if(WIN32)
    message(STATUS "Configuring Windows post-build steps for copying DLLs...")

    # 1. Copy core Qt6 DLLs
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT6_INSTALL_ROOT}/bin/Qt6Core.dll"
        "${QT6_INSTALL_ROOT}/bin/Qt6Gui.dll"
        "${QT6_INSTALL_ROOT}/bin/Qt6Widgets.dll"
        "${QT6_INSTALL_ROOT}/bin/Qt6Network.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying Qt6 DLLs to target directory..."
    )

    # 2. Copy the QWindows platform plugin (crucial for Qt to boot on Windows)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QT6_INSTALL_ROOT}/plugins/platforms/qwindows.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/"
        COMMENT "Copying Windows QPA platform plugin..."
    )
endif()
cmake_minimum_required(VERSION 3.16)
project(BrewNBites CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable automatic compilation of Qt Meta-Object System (MOC)
set(CMAKE_AUTOMOC ON)

# ---------------------------------------------------------------------------
# Qt6 Path configuration (Windows / MinGW Cross-compilation)
# ---------------------------------------------------------------------------
if(WIN32)
    # Detect the downloaded Qt6 Windows binaries in vendor/qt6
    file(GLOB QT6_CANDIDATES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qt6/[0-9]*/mingw_64")
    list(LENGTH QT6_CANDIDATES CANDIDATE_COUNT)
    if(CANDIDATE_COUNT GREATER 0)
        list(GET QT6_CANDIDATES 0 DETECTED_QT6_DIR)
        message(STATUS "Found Qt6 Windows MinGW installation: ${DETECTED_QT6_DIR}")
        set(CMAKE_PREFIX_PATH "${DETECTED_QT6_DIR}")
    else()
        message(WARNING "Qt6 MinGW binaries not found in vendor/qt6. Using default prefix path.")
        set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/qt6/6.10.2/mingw_64")
    endif()
endif()

# Find Qt6 components
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# List source files explicitly for reliability and clean builds
set(SRCS
    src/main.cpp
    src/mainwindow.cpp
    src/registerpage.cpp
    src/orderpage.cpp
    src/paymentpage.cpp
    src/menuitem.cpp
    src/ordermanager.cpp
)

set(HEADERS
    src/mainwindow.h
    src/registerpage.h
    src/orderpage.h
    src/paymentpage.h
    src/menuitem.h
    src/ordermanager.h
    src/theme.h
)

# Add executable
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

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
        "${CMAKE_PREFIX_PATH}/bin/Qt6Core.dll"
        "${CMAKE_PREFIX_PATH}/bin/Qt6Gui.dll"
        "${CMAKE_PREFIX_PATH}/bin/Qt6Widgets.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying Qt6 DLLs to target directory..."
    )
    
    # 2. Copy the QWindows platform plugin (crucial for Qt to boot on Windows)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_PREFIX_PATH}/plugins/platforms/qwindows.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/"
        COMMENT "Copying Windows QPA platform plugin..."
    )
endif()
cmake_minimum_required(VERSION 3.11.0)
project(msmanager VERSION 0.1.0.0)

set(QT_VERSION_FIND "5.*.*")
include(externals/ljh/cmake/Qt.cmake)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_PREFIX})

add_subdirectory(externals/json)
add_subdirectory(externals/fmt )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CPACK_PACKAGE_NAME "Minecraft Server Manager")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Minecraft Server Manager")
set(CPACK_PACKAGE_VENDOR "Link1J")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://link1j.me/")
set(CPACK_VERBATIM_VARIABLES true)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/assets/appicon.png")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")

set(CPACK_WIX_UPGRADE_GUID "04677607-C7E8-434D-BF16-0DBC27B6E539")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/assets/appicon-64x64.png")

set(CMAKE_INSTALL_UCRT_LIBRARIES FALSE)

include(CPack)
include(InstallRequiredSystemLibraries)

set(SOURCES
    src/main.cpp
    src/serverconnection.cpp
    src/mainwindow.cpp
    src/usermodel.cpp
    src/settingsdialog.cpp
    src/pluginmodel.cpp
    src/userinfopanel.cpp
    # Add New Files Here!
)

set(UI_SOURCES
    assets/resources.qrc
    # Add New Files Here!
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Threads                REQUIRED)

if (APPLE)
    set(OS_BUNDLE MACOSX_BUNDLE)
elseif (WIN32)
    set(OS_BUNDLE WIN32)
endif()

set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION 10.0.18363.0)

add_executable(msmanager ${OS_BUNDLE}
    ${SOURCES}
    ${UI_SOURCES}
)

set_target_properties(msmanager PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_definitions(msmanager PUBLIC 
    "_WIN32_WINNT=0x0601"
    "WINVER=0x0601"
    $<$<PLATFORM_ID:WindowsStore>:"ASIO_WINDOWS_APP=1">
)

if (WIN32 AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
    set(TEST ON)
endif()
message("${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION} ${CMAKE_SYSTEM_VERSION} ${TEST}")

target_include_directories(msmanager PUBLIC
    externals/asio/asio/include
    src
)

target_link_libraries(msmanager 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    nlohmann_json::nlohmann_json
    Threads::Threads
    fmt::fmt
)

if (WIN32 AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
    add_custom_command(TARGET msmanager POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/windeployqt"
        COMMAND Qt::windeployqt --dir "${CMAKE_BINARY_DIR}/windeployqt" "$<TARGET_FILE:msmanager>" --verbose 0 --no-compiler-runtime
        COMMENT "Deploying Qt..."
    )
    install(TARGETS msmanager RUNTIME DESTINATION .)
    install(DIRECTORY "${CMAKE_BINARY_DIR}/windeployqt" DESTINATION .)
endif()
cmake_minimum_required(VERSION 3.11.0)
project(msmanager VERSION 0.1.0.0)

set(QT_VERSION_FIND "5.*.*")
include(externals/ljh/cmake/Qt.cmake)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install    OFF CACHE INTERNAL "")
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
set(CPACK_PACKAGE_EXECUTABLES "msmanager;Minecraft Server Manager")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://link1j.me/")
set(CPACK_VERBATIM_VARIABLES true)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/assets/appicon.png")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/assets/LICENSE.txt")

set(CPACK_WIX_UPGRADE_GUID "04677607-C7E8-434D-BF16-0DBC27B6E539")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/assets/appicon-64x64.png")

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

include(CPack)

set(SOURCES
    src/main.cpp
    src/serverconnection.cpp
    src/mainwindow.cpp
    src/usermodel.cpp
    src/settingsdialog.cpp
    src/pluginmodel.cpp
    src/userinfopanel.cpp
    src/aboutthis.cpp
    # Add New Files Here!
)

set(UI_SOURCES
    assets/resources.qrc
    # Add New Files Here!
)

find_package(Qt5     REQUIRED COMPONENTS Widgets QuickWidgets)
find_package(Threads REQUIRED)

if (APPLE)
    set(OS_BUNDLE MACOSX_BUNDLE)
elseif (WIN32)
    set(OS_BUNDLE WIN32)
endif()

add_executable(msmanager ${OS_BUNDLE}
    ${SOURCES}
    ${UI_SOURCES}
)

set_target_properties(msmanager PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION 10.0.10240.0
)

target_compile_definitions(msmanager PUBLIC 
    "_WIN32_WINNT=0x0601"
    "WINVER=0x0601"
    _SILENCE_CLANG_COROUTINE_MESSAGE=1
    $<$<PLATFORM_ID:WindowsStore>:"ASIO_WINDOWS_APP=1">
)

target_include_directories(msmanager PUBLIC
    externals/asio/asio/include
    src
)

target_link_libraries(msmanager 
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Qml
    Qt::Quick
    Qt::QuickWidgets
    nlohmann_json::nlohmann_json
    Threads::Threads
    fmt::fmt
)

install(TARGETS msmanager RUNTIME DESTINATION .)

if (WIN32 AND NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
    set(windeployqt_args --no-compiler-runtime --no-translations --no-system-d3d-compiler --no-opengl-sw --verbose 0)
    add_custom_command(TARGET msmanager POST_BUILD
        COMMENT "Deploying Qt..."
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/windeployqt"
        COMMAND Qt::windeployqt --dir "${CMAKE_BINARY_DIR}/windeployqt" ${windeployqt_args} --qmldir "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE:msmanager>"
    )
    add_custom_command(TARGET msmanager POST_BUILD
        COMMENT "Creating Debug Folder..."
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/debug"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/debug/"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:msmanager>" "${CMAKE_BINARY_DIR}/debug/"
        COMMAND Qt::windeployqt --dir "${CMAKE_BINARY_DIR}/debug" ${windeployqt_args} --qmldir "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE:msmanager>"
    )
    install(DIRECTORY ${CMAKE_BINARY_DIR}/windeployqt/ DESTINATION .)
endif()
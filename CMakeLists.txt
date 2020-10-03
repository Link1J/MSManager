cmake_minimum_required(VERSION 3.7.0)
project(msmanager VERSION 0.1.0)

set(QT_VERSION_FIND "5.*.*")

include(externals/ljh/cmake/QtPrefixFinder.cmake)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_PREFIX})

add_subdirectory(externals/nlohmann_json)
add_subdirectory(externals/fmt-6.1.2    )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

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

set(WINDOWS_DEF "_WIN32_WINNT=0x601")

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
)

target_compile_definitions(msmanager PUBLIC 
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:${WINDOWS_DEF}>
)

target_include_directories(msmanager PUBLIC
    externals/asio-1.12.2/include
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
cmake_minimum_required(VERSION 3.1.0)
project(msmanager VERSION 0.1.0)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} C:/Qt/5.14.1/msvc2017_64)

add_subdirectory(external/nlohmann_json)
add_subdirectory(external/fmt-6.1.2    )

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

set(WINDOWS_DEF
	"_WIN32_WINNT=0x601"
)

find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)
find_package(Threads                          REQUIRED)

add_executable(msmanager
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
	external/asio-1.12.2/include
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
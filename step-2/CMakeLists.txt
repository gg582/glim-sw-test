cmake_minimum_required(VERSION 3.14)
project(CircleApp VERSION 1.0.0 LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
    message(FATAL_ERROR "This project requires Windows and MFC.")
endif()

set(CMAKE_MFC_FLAG 2) # Use MFC in a shared DLL

add_executable(CircleApp WIN32
    src/main.cpp
    src/CircleDialog.cpp
    src/CircleDrawer.cpp
    src/CircleApp.rc
)

target_compile_definitions(CircleApp PRIVATE
    _AFXDLL
    _UNICODE
    UNICODE
    _WINDOWS
)

target_include_directories(CircleApp PRIVATE src)

# Link against standard Windows libraries
set_target_properties(CircleApp PROPERTIES
    LINK_FLAGS "/SUBSYSTEM:WINDOWS"
)

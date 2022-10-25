add_executable(Crystal main.cpp)

include(FetchContent)
add_subdirectory(platform/linux)
add_subdirectory(core)
find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})
include_directories(${X11_xcb_INCLUDE_PATH})

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master)
FetchContent_MakeAvailable(fmt)

target_link_libraries(Crystal
        fmt::fmt
        "-lxcb -lX11 -lX11-xcb" )
add_executable(Crystal main.cpp)

include(FetchContent)
add_subdirectory(platform/linux)
add_subdirectory(core)
find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})
include_directories(${X11_xcb_INCLUDE_PATH})

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master)
FetchContent_Declare(
        Range-v3
        GIT_REPOSITORY https://github.com/ericniebler/range-v3
        GIT_TAG master
)
FetchContent_Declare(
        concurrentqueue
        GIT_REPOSITORY
        GIT_TAG master
)

FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(Range-v3)
FetchContent_MakeAvailable(concurrentqueue)

target_include_directories(Crystal PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
target_link_libraries(Crystal
        concurrentqueue
        fmt::fmt
        range-v3
        "-lxcb -lX11 -lX11-xcb" )
add_executable(Crystal main.cpp)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
elseif(WIN32)
    set(WINDOWS TRUE)
endif()

if(LINUX)
    add_compile_definitions(CRYSTAL_PLATFORM_LINUX)
    SET(CRYSTAL_ADDITIONAL_LINKER_FLAGS "-lxcb -lX11 -lX11-xcb")

    add_subdirectory(platform/linux)

    find_package(X11 REQUIRED)
    link_libraries(${X11_LIBRARIES})
    include_directories(${X11_INCLUDE_DIR})
    include_directories(${X11_xcb_INCLUDE_PATH})
elseif(WINDOWS)
    add_compile_definitions(CRYSTAL_PLATFORM_WINDOWS)
endif()

add_subdirectory(core)

include(FetchContent)
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

SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${CRYSTAL_ADDITIONAL_LINKER_FLAGS}")
target_link_libraries(Crystal
        concurrentqueue
        fmt::fmt
        range-v3
         )
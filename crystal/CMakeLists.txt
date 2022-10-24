add_executable(Crystal main.cpp)

add_subdirectory(platform/linux)
find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})
include_directories(${X11_xcb_INCLUDE_PATH})

target_link_libraries(Crystal "-lxcb -lX11 -lX11-xcb" )
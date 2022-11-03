#pragma once

#ifdef CRYSTAL_PLATFORM_LINUX
#include "linux/Keycodes.h"
#elifdef CRYSTAL_PLATFORM_WINDOWS
#include "windows/Keycodes.h"
#endif

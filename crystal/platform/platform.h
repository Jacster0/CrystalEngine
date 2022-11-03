#pragma once

#ifdef CRYSTAL_PLATFORM_LINUX
#include "linux/platform.h"
#elifdef CRYSTAL_PLATFORM_WINDOWS
#include "windows/platform.h"
#endif

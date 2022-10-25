#pragma once
#include <string_view>
#include <source_location>
#include "LogLevel.h"

namespace crystal {
    class ISink {
    public:
        virtual void emit(std::string_view message, LogLevel lvl, const std::source_location& loc = std::source_location::current()) noexcept = 0;
        virtual ~ISink() = default;
    };
}

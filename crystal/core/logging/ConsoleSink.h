#pragma once

#include <string>
#include "sink.h"

namespace crystal {
    class ConsoleSink final : public ISink {
    public:
        ConsoleSink() noexcept;
        void emit(std::string_view msg, LogLevel lvl, const std::source_location& loc) noexcept override;
    private:
        std::string m_prefix;
    };
}
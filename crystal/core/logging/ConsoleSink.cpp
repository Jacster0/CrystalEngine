#include <iostream>
#include "ConsoleSink.h"
#include "LogLevel.h"
#include "fmt/format.h"

using namespace crystal;

ConsoleSink::ConsoleSink() noexcept {
    m_prefix.append("[ ").append(" ] ");
}

void ConsoleSink::emit(std::string_view msg, LogLevel lvl, const std::source_location &loc) noexcept {
    std::string msgPrefix{};

    switch (lvl) {
        case LogLevel::info:
            msgPrefix = "<INFO>";
            break;
        case LogLevel::warning:
            msgPrefix = "<WARNING>";
            break;
        case LogLevel::error:
            msgPrefix = "<ERROR>";
            break;
        case LogLevel::debug:
            msgPrefix = "<DEBUG>";
            break;
    }

    fmt::print("{} {} {}", m_prefix, msgPrefix, msg);
}

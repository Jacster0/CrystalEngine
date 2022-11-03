#pragma once

#include <source_location>
#include <utility>
#include <string_view>
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include "sink.h"
#include "fmt/format.h"
#include "../utils/Singleton.h"

namespace crystal {
    namespace detail {
        struct log_fmt {
            constexpr log_fmt(auto &&msg, const std::source_location &location = std::source_location::current())
                :
                msg(std::forward<decltype(msg)>(msg)),
                location(location)
            {}

            log_fmt(const log_fmt &rhs) = delete;
            log_fmt &operator=(const log_fmt &rhs) = delete;
            ~log_fmt() = default;

            std::string_view msg;
            std::source_location location;
        };
    }

    class Logger : public Singleton<Logger> {
    public:
        static constexpr auto endline() noexcept { return std::endl<char, std::char_traits<char>>; }
        static constexpr auto newline() noexcept { return "\n"; }

        template<std::derived_from<ISink> T>
        static auto add_sink(auto&&... args) noexcept
        requires std::constructible_from<T, decltype(args)...> {
            get().m_sinks.emplace_back(std::make_unique<T>(std::forward<decltype(args)>(args)...));
        }

        static void add_sink(std::unique_ptr<ISink>&& sink) noexcept {
            get().m_sinks.emplace_back(std::move(sink));
        }

        template<std::derived_from<ISink> T>
        static void remove_sink() noexcept {
            std::erase_if(get().m_sinks, [](const auto& sink) {return typeid(*sink) == typeid(T); });
        }

        void Log(LogLevel lvl, const detail::log_fmt& fmt, auto&&... args) const noexcept {
            std::scoped_lock lock(m_logging_mutex);

            const std::string message = fmt::format("{}{}", fmt::vformat(fmt.msg, fmt::make_format_args(std::forward<decltype(args)>(args)...)), Logger::newline());

            for(const auto& sink : m_sinks) {
                sink->emit(message, lvl);
            }
        }

        static void Info(detail::log_fmt fmt, auto&&... args) noexcept {
            get().Log(LogLevel::info, fmt, std::forward<decltype(args)>(args)...);
        }

        static void Info(std::string_view msg, std::source_location loc = std::source_location::current()) noexcept {
            Info(detail::log_fmt{"{}", loc}, msg);
        }

        static void Warning(detail::log_fmt fmt, auto&&... args) noexcept {
            get().Log(LogLevel::warning, fmt, std::forward<decltype(args)>(args)...);
        }

        static void Warning(std::string_view msg, std::source_location loc = std::source_location::current()) noexcept {
            Warning(detail::log_fmt{"{}", loc}, msg);
        }

        static void Error(detail::log_fmt fmt, auto&&... args) noexcept {
            get().Log(LogLevel::error, fmt, std::forward<decltype(args)>(args)...);
        }

        static void Error(std::string_view msg, std::source_location loc = std::source_location::current()) noexcept {
            Error(detail::log_fmt{"{}", loc}, msg);
        }

        static void Debug(detail::log_fmt fmt, auto&&... args) noexcept {
#ifdef _DEBUG
            get().Log(LogLevel::debug, fmt, std::forward<decltype(args)>(args)...);
#endif
        }

        static void Debug(std::string_view msg, std::source_location loc = std::source_location::current()) noexcept {
            Debug(detail::log_fmt{"{}", loc}, msg);
        }

    private:
        mutable std::mutex m_logging_mutex;
        std::vector<std::unique_ptr<ISink>> m_sinks;
    };
}

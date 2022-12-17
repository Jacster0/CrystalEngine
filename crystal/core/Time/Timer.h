#pragma once
#include <chrono>
#include <type_traits>
#include <cstdint>

namespace crystal {
    namespace detail {
        template<class T>
        constexpr bool is_ratio_v = false;

        template<std::intmax_t nx, std::intmax_t dx>
        constexpr bool is_ratio_v<std::ratio<nx, dx>> = true;

        template<class T, class U>
        constexpr bool validate_args = std::is_floating_point_v<T> && is_ratio_v<U>;

        template<class, class = void>
        constexpr bool is_clock_v = false;

        template<class T>
        constexpr bool is_clock_v<T, std::void_t<
                typename T::duration,
                typename T::time_point,
                decltype(T::now())>> = true;

        template<class, template<class...> class>
        constexpr bool is_specialization_v = false;

        template<template<class...> class Template, class... Args>
        constexpr bool is_specialization_v<Template<Args...>, Template> = true;

        template<class T>
        constexpr bool is_duration_v = is_specialization_v<T, std::chrono::duration>;
    }

    template<class Clock = std::chrono::steady_clock>
    class Timer {
    public:
        static_assert(detail::is_clock_v<Clock>, "The specified clock did not meet the required implementation");

        Timer() noexcept
            :
            m_last(Clock::now())
        {}

        void begin_measure() noexcept {
            m_begin = Clock::now().time_since_epoch();
        }

        template<class Duration = std::chrono::milliseconds>
        [[nodiscard]] auto end_measure() const noexcept -> std::enable_if_t<detail::is_duration_v<Duration>, Duration> {
            const auto end = Clock::now().time_since_epoch();
            const auto dt = end - m_begin;

            return std::chrono::duration_cast<Duration>(dt);
        }

        template<class T = float, class U = std::ratio<1>>
        [[nodiscard]] auto get_delta() const noexcept -> std::enable_if_t<detail::validate_args<T,U>, T> {
            return std::chrono::duration<T,U>(m_delta).count();
        }

        template<class T = float, class U = std::ratio<1>>
        [[nodiscard]] auto get_elapsed() const noexcept -> std::enable_if_t<detail::validate_args<T,U>, T> {
            return std::chrono::duration<T,U>(m_elapsed).count();
        }

        void tick() noexcept {
            const auto now = Clock::now();

            m_delta = now - m_last;
            m_last = now;
            m_elapsed += m_delta;
        }

        void reset() noexcept {
            m_delta = Clock::now();
            m_last = Clock::now();
            m_elapsed = Clock::now();
        }

        void reset_elapsed() noexcept {
            m_elapsed = Clock::now();
        }

    private:
        Clock::duration m_begin{};
        Clock::duration m_last;
        Clock::duration m_delta{};
        Clock::duration m_elapsed{};
    };
}
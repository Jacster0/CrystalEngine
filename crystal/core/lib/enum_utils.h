#pragma once

#include <string_view>
#include <type_traits>
#include <array>
#include <optional>
#include <limits>

namespace crystal::crylib {
    namespace detail {
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9)
        inline constexpr auto suffix = sizeof("]") - 1;
#define FUNC_SIG __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
        inline constexpr auto suffix = sizeof(">(void) noexcept") - 1;
#define FUNC_SIG __FUNCSIG__
#else
#define FUNC_SIG
#endif
        constexpr auto MAX_RANGE = 256;

        consteval bool is_digit(char c) noexcept {
            return c >= '0' && c <= '9';
        }
    }

    namespace impl {
        template<class Enum, Enum e>
        constexpr std::string_view enum_to_string_impl() noexcept {
            constexpr std::string_view fullName{FUNC_SIG};
            constexpr auto index = fullName.find_last_of(", :)-", fullName.size() - detail::suffix) + 1;

            if constexpr (detail::is_digit(fullName[index])) {
                return {};
            }
            return fullName.substr(index, fullName.size() - detail::suffix - index);
        }

        template<class Enum, int offset, int... I>
        constexpr std::string_view enum_to_string_helper(Enum value, std::integer_sequence<int, I...>) {
            return std::array<decltype(&enum_to_string_impl<Enum, static_cast<Enum>(0)>), sizeof...(I)>
                {
                    {
                        enum_to_string_impl<Enum, static_cast<Enum>(I - offset)>...
                    }
                }[offset + static_cast<int>(value)]();
        }

        template <typename Enum, int Offset, int ...I>
        [[nodiscard]] constexpr std::optional<Enum> enum_from_string_impl(std::string_view name, std::integer_sequence<int, I...>) noexcept {
            std::optional<Enum> returnValue;
            (void)((impl::enum_to_string_impl<
                    Enum, static_cast<Enum>(I - Offset)
            >() != name || (returnValue = static_cast<Enum>(I - Offset), false)) && ...);

            return returnValue;
        }
    }

    namespace detail {
        template<class T>
        struct Limit {
            static constexpr auto Range = sizeof(T) <= 2
                                          ? static_cast<int>(std::numeric_limits<T>::max()) - static_cast<int>(std::numeric_limits<T>::min() + 1)
                                          : std::numeric_limits<int>::max();

            static constexpr auto Size = std::min(Range, MAX_RANGE);
            static constexpr auto Offset = std::is_signed_v<T> ? (Size + 1) / 2 : 0;
        };
    }

    template<class Enum, class U = std::underlying_type_t<std::decay_t<Enum>>>
    constexpr auto enum_to_string(Enum enumValue) noexcept -> std::enable_if_t<std::is_enum_v<std::decay_t<Enum>>, std::string_view> {
        using Indices = std::make_integer_sequence<int, detail::Limit<U>::Size>;

        if(static_cast<int>(enumValue) >= detail::MAX_RANGE || static_cast<int>(enumValue) <= -detail::MAX_RANGE) {
            return {};
        }
        return impl::enum_to_string_helper<std::decay_t<Enum>, detail::Limit<U>::Offset>(enumValue, Indices{});
    }

    template<auto EnumValue>
    constexpr auto enum_to_string() noexcept -> std::enable_if_t<std::is_enum_v<std::decay_t<decltype(EnumValue)>>, std::string_view> {
        return impl::enum_to_string_impl<decltype(EnumValue), EnumValue>();
    }

    template<class Enum, class U = std::underlying_type_t<std::decay_t<Enum>>>
    constexpr auto string_to_enum(std::string_view name) noexcept
    -> std::enable_if_t<std::is_enum_v<std::decay_t<Enum>>, std::optional<Enum>>
    {
        return impl::enum_from_string_impl<Enum, detail::Limit<U>::Offset>(
                name,
                std::make_integer_sequence<int, detail::Limit<U>::Size>{}
        );
    }
}



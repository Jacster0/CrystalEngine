#pragma once
#include "EventSystem.h"

namespace crystal {
    namespace details {
        template<class>
        struct EventHandlerTraits;

        template<class T, class U, class V>
        struct EventHandlerTraits<T(U::*)(V)> {
            using ReturnType = T;
            using Class = U;
            using Event = std::remove_cvref_t<V>;
        };
    }

    class EventListener {
    public:
        EventListener() = default;
        EventListener(const EventListener& rhs) {
            EventSystem::get().duplicate_handlers_with_listener(*this, rhs);
        }
        EventListener(EventListener&& rhs) noexcept {
            EventSystem::get().replace_listener(*this, rhs);
        }
        ~EventListener() {
            unregister_all_event_handlers();
        }

        template<auto... Handlers>
        void register_event_handlers() noexcept {
            (register_event_handler<Handlers>(), ...);
        }

        template<auto Handler>
        void register_event_handler() noexcept {
            auto& type = typeid(typename details::EventHandlerTraits<decltype(Handler)>::Event);
            auto handler = dispatch<Handler>;

            EventSystem::get().add_handler(type, handler, this);
        }

        template<auto Handler>
        void unregister_event_handler() noexcept {
            auto& type = typeid(typename details::EventHandlerTraits<decltype(Handler)>::Event);
            auto handler = dispatch<Handler>;

            EventSystem::get().remove_handler(type, handler, this);
        }

        template<auto... Handlers>
        void unregister_event_handles() const noexcept {
            (unregister_event_handles<Handlers>(), ...);
        }

        void unregister_all_event_handlers() noexcept {
            EventSystem::get().remove_handlers_with_listener(*this);
        }

    private:
        template<auto Handler>
        static constexpr bool dispatch(EventListener& listener, void* eventData) noexcept {
            using handler = details::EventHandlerTraits<decltype(Handler)>;

            auto& event = *static_cast<typename handler::Event*>(eventData);
            auto& object = static_cast<typename handler::Class&>(listener);

            if constexpr (std::same_as<typename handler::ReturnType, void>) {
                (object.*Handler)(event);
                return false;
            }
            else if constexpr (std::same_as<typename handler::ReturnType, bool>) {
                return (object.*Handler)(event);
            }
            else {
                static_assert(!sizeof(Handler), "An event handler must return void or bool.");
            }
        }
    };

}

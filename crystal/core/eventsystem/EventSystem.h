#pragma once

#include <utility>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <any>
#include "concurrentqueue.h"
#include "../utils/Singleton.h"

namespace crystal {
    using namespace moodycamel;

    class EventSystem : public Singleton<EventSystem> {
    public:
        template<class Event, class... Args>
        static void notify(Args&&... args)
        requires std::constructible_from<Event, decltype(args)...>
        {
            Event event {std::forward<Args>(args)...};
            get().dispatch(&event, typeid(Event));
        }

        template<class Event, class... Args>
        static void notify_async(Args&&... args)
        requires std::constructible_from<Event, decltype(args)...>
        {
            get().enqueue_async_event(Event {std::forward<Args>(args)...});
        }

        EventSystem()
            :
            m_consumerToken(m_events)
        {}
    private:


        friend class EventListener;

        using Callback = bool (*)(class EventListener&, void*);
        struct EventHandler {
            Callback	   callback;
            EventListener* listener;
        };

        std::unordered_map<std::type_index, std::vector<EventHandler>> event_handlers;
        ConcurrentQueue<std::any> m_events;
        ConsumerToken m_consumerToken;

        void enqueue_async_event(std::any event) noexcept;
        void dispatch(void* event, std::type_index typeIndex) noexcept;

        void add_handler(std::type_index event_type, Callback cb, EventListener* listener) noexcept;
        void remove_handler(std::type_index event_type, Callback callback, EventListener* listener) noexcept;

        void duplicate_handlers_with_listener(EventListener& newListener,  const EventListener& oldListener) noexcept;
        void replace_listener(EventListener& newListener, EventListener& oldListener) noexcept;
        void remove_handlers_with_listener(EventListener& listener) noexcept;
    };
}
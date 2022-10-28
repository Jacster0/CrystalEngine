#include "EventSystem.h"
#include "range/v3/all.hpp"

using namespace crystal;

namespace vi = ranges::views;

void EventSystem::dispatch(void *event, std::type_index typeIndex) noexcept {
    auto handlers = event_handlers.find(typeIndex);
    if(handlers == event_handlers.end()) {
        return;
    }

    for(auto handler : handlers->second | vi::reverse) {
        if(const auto consumed = handler.callback(*handler.listener, event)) {
            break;
        }
    }
}

void EventSystem::add_handler(std::type_index event_type, Callback cb, EventListener *listener) noexcept {
    event_handlers[event_type].push_back({cb, listener});
}

void EventSystem::remove_handler(std::type_index event_type, Callback cb, EventListener* listener) noexcept {
    auto handlers = event_handlers.find(event_type);
    if(handlers == event_handlers.end()){
        return;
    }

    std::erase_if(handlers->second, [=](EventHandler handler) {
        return cb == handler.callback && listener == handler.listener;
    });
}

void EventSystem::enqueue_async_event(std::any event) noexcept {
    const thread_local ProducerToken producerToken(m_events);
    m_events.enqueue(producerToken, std::move(event));
}

void EventSystem::duplicate_handlers_with_listener(EventListener& newListener, const EventListener& oldListener) noexcept {
    for(auto& [type, handlers] : event_handlers) {
        for(auto& handler : handlers) {
            if(handler.listener == &oldListener) {
                handlers.push_back({handler.callback, &newListener});
            }
        }
    }

}

void EventSystem::replace_listener(EventListener& newListener, EventListener& oldListener) noexcept {
    for (auto &[type, handlers]: event_handlers) {
        for (auto &handler: handlers) {
            if (handler.listener == &oldListener) {
                handler.listener = &newListener;
            }
        }
    }
}

void EventSystem::remove_handlers_with_listener(EventListener& listener) noexcept {
    for (auto &[type, handlers]: event_handlers) {
        std::erase_if(handlers, [&](EventHandler handler) { return handler.listener == &listener; });
    }
}

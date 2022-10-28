#pragma once

namespace crystal {
    template<class T>
    class Singleton {
    public:
        Singleton(const Singleton& rhs) = delete;
        Singleton& operator=(const Singleton& rhs) = delete;

        static T& get() {
            static T t;
            return t;
        }
    protected:
        Singleton() = default;
    };
}

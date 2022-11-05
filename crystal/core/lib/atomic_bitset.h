#pragma once
#include <cstdint>
#include <bitset>
#include <atomic>

namespace crystal::crylib {
    template<size_t Size>
    class atomic_bitset {
    public:
        auto operator[](size_t index) const noexcept { return m_bitset.load(std::memory_order_acquire)[index]; }

        void set(size_t index, bool val) {
            bool result;
            do {
                auto old_value = m_bitset.load(std::memory_order_acquire);
                auto new_value = old_value;
                new_value.set(index, val);

                result = m_bitset.compare_exchange_strong(old_value, new_value, std::memory_order_acq_rel, std::memory_order_acquire);
            } while (!result);
        }
    private:
        std::atomic<std::bitset<Size>> m_bitset;
    };
}

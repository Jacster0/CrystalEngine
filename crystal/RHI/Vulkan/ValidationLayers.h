#pragma once

namespace crystal::validationLayers {
    constexpr auto enableValidationLayers() noexcept {
#ifdef NDEBUG
        return false;
#else
        return true;
#endif
    }
    bool checkSupport() noexcept;
    std::vector<const char*> getValidationLayers() noexcept;
}

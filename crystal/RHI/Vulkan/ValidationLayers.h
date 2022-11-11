#pragma once

namespace crystal::validationLayers {
    constexpr std::array validationLayerNames = { "VK_LAYER_KHRONOS_validation"};

    constexpr auto enableValidationLayers() noexcept {
#ifdef NDEBUG
        return false;
#else
        return true;
#endif
    }

    bool checkSupport() noexcept;
    constexpr auto getValidationLayers() noexcept {
        return validationLayerNames;
    }
}

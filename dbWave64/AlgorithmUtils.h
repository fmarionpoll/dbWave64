#pragma once
#include <algorithm>

namespace dbw {
    template <typename T>
    inline T clamp_value(const T& v, const T& lo, const T& hi) {
        return std::max(lo, std::min(v, hi));
    }
}
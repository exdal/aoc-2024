#pragma once

#include <chrono>

constexpr static auto decimals_for_3sf(double value) -> u32 {
    if (value < 10)
        return 2;
    if (value < 100)
        return 1;
    return 0;
}

constexpr static auto duration_to_str(double nanos) -> std::string {
    if (nanos >= 1e9) {
        double seconds = nanos / 1e9;
        return std::format("{:.{}f}s", seconds, decimals_for_3sf(seconds));
    }

    if (nanos >= 1e6) {
        double millis = nanos / 1e6;
        return std::format("{:.{}f}ms", millis, decimals_for_3sf(millis));
    }

    if (nanos >= 1e3) {
        double micros = nanos / 1e3;
        return std::format("{:.{}f}us", micros, decimals_for_3sf(micros));
    }

    if (nanos >= 0) {
        return std::format("{:.{}f}ns", nanos, decimals_for_3sf(nanos));
    }

    return "0";
}

struct Timestamp {
    std::chrono::steady_clock::time_point tp_start = {};
    std::chrono::steady_clock::time_point tp_end = {};

    auto start() -> void { this->tp_start = std::chrono::steady_clock::now(); }
    auto end() -> void { this->tp_end = std::chrono::steady_clock::now(); }
    auto duration() -> std::chrono::duration<double, std::nano> { return this->tp_end - this->tp_start; }
    auto duration_str() -> std::string {
        auto nanos = this->duration().count();
        return duration_to_str(nanos);
    }
};

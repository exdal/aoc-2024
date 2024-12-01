#include "file.hh"
#include "stack.hh"

#include <cstdlib>
#include <ranges>
#include <algorithm>

bool is_digit(c8 c) {
    return u32(c - '0') < 10;
}

i32 main(i32 argc, c8 *argv[]) {
    ScopedStack stack;

    //  ── FILE READ ───────────────────────────────────────────────────────
    Timestamp ts_file_read;
    ts_file_read.start();
    File file("day1.txt", FileAccess::Read);
    auto file_data = stack.alloc_str(file.size);
    file.read(const_cast<c8 *>(file_data.data()), file_data.length());
    file.close();
    ts_file_read.end();

    //  ── LEXER ───────────────────────────────────────────────────────────

    std::vector<u32> col0;
    std::vector<u32> col1;

    u32 offset = 0;
    auto peek = [&]() -> c8 { return (offset < file_data.length()) ? file_data[offset] : '\0'; };
    bool eof = false;

    Timestamp ts_lexer;
    ts_lexer.start();
    while (!eof) {
        u32 num = 0;
        switch (peek()) {
            case '\0': {
                eof = true;
                break;
            }
            case ' ':
            case '\n': {
                offset++;
                break;
            }
            default: {
                auto c = peek();
                auto start_off = offset;

                do {
                    num = num * 10 + (file_data[offset++] - '0');
                } while (is_digit(peek()));
            }
        }

        if (num != 0) {
            if (col0.size() <= col1.size()) {
                col0.push_back(num);
            } else {
                col1.push_back(num);
            }
        }
    }
    ts_lexer.end();

    Timestamp ts_sorting;
    ts_sorting.start();
    std::sort(col0.begin(), col0.end());
    std::sort(col1.begin(), col1.end());
    ts_sorting.end();

    //  ── DISTANCE SUM ────────────────────────────────────────────────────
    Timestamp ts_distance_sum;
    u32 dist_sum = 0;
    ts_distance_sum.start();
    for (auto [x, y] : std::views::zip(col0, col1)) {
        dist_sum += std::abs(static_cast<i32>(x) - static_cast<i32>(y));
    }
    ts_distance_sum.end();

    //  ── OCCURRANCE PRODUCT ──────────────────────────────────────────────
    u32 occur_sum = 0;
    Timestamp ts_occur_product;
    ts_occur_product.start();
    for (const auto &x : col0) {
        occur_sum += x * std::count(col1.begin(), col1.end(), x);
    }
    ts_occur_product.end();

    std::println("Part 1 Answer: {}", dist_sum);
    std::println("Part 2 Answer: {}", occur_sum);

    auto total =
        ts_file_read.duration() + ts_lexer.duration() + ts_sorting.duration() + ts_distance_sum.duration() + ts_occur_product.duration();

    std::println(
        "\n====== STATS ======\n"
        "File read: {}\n"
        "Lexer: {}\n"
        "Sorting: {}\n"
        "Distance sum: {}\n"
        "Occurrence: {}\n\nTOTAL: {}\n",
        ts_file_read.duration_str(),
        ts_lexer.duration_str(),
        ts_sorting.duration_str(),
        ts_distance_sum.duration_str(),
        ts_occur_product.duration_str(),
        duration_to_str(total.count()));

    return 0;
}

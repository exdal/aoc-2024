#pragma once

#include "os.hh"

struct File {
    std::optional<os::FileDescriptor> handle;
    usize size = 0;
    FileResult result = FileResult::Success;

    File() = default;
    File(const fs::path &path, FileAccess access);
    File(const File &) = default;
    File(File &&) = default;
    ~File() { close(); }

    auto write(const void *data, u64 size) -> u64;
    auto read(void *data, u64 size) -> u64;
    auto seek(i64 offset) -> void;
    auto close() -> void;

    File &operator=(File &&rhs) noexcept {
        this->handle = rhs.handle;
        this->size = rhs.size;
        this->result = rhs.result;

        rhs.handle.reset();

        return *this;
    }
    bool operator==(const File &) const = default;
    explicit operator bool() { return result == FileResult::Success; }

    static void to_stdout(std::string_view str);
};

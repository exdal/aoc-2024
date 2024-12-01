#pragma once

enum class FileResult : i32 {
    Success = 0,
    NoAccess,
    Exists,
    IsDir,
    InUse,
    Unknown,
};
constexpr bool operator!(FileResult v) {
    return v != FileResult::Success;
}

enum class FileAccess : u32 {
    Write = 1 << 0,
    Read = 1 << 1,
};
consteval void enable_bitmask(FileAccess);

namespace os {
enum class FileDescriptor : uptr { Invalid = 0 };
auto file_open(const fs::path &path, FileAccess access) -> std::expected<FileDescriptor, FileResult>;
auto file_close(FileDescriptor file) -> void;
auto file_size(FileDescriptor file) -> std::expected<usize, FileResult>;
auto file_read(FileDescriptor file, void *data, usize size) -> usize;
auto file_write(FileDescriptor file, const void *data, usize size) -> usize;
auto file_seek(FileDescriptor file, i64 offset) -> void;

auto mem_page_size() -> u64;
auto mem_reserve(u64 size) -> void *;
auto mem_release(void *data, u64 size = 0) -> void;
auto mem_commit(void *data, u64 size) -> bool;
auto mem_decommit(void *data, u64 size) -> void;
}  // namespace os

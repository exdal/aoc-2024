#include "file.hh"

File::File(const fs::path &path, FileAccess access) {
    auto file_handle = os::file_open(path, access);
    if (!file_handle.has_value()) {
        this->result = file_handle.error();
        return;
    }

    this->handle = file_handle.value();
    this->size = os::file_size(this->handle.value()).value_or(0);
}

auto File::write(const void *data, u64 size) -> u64 {
    return os::file_write(this->handle.value(), data, size);
}

auto File::read(void *data, u64 size) -> u64 {
    return os::file_read(this->handle.value(), data, size);
}

auto File::seek(i64 offset) -> void {
    os::file_seek(this->handle.value(), offset);
}

auto File::close() -> void {
    if (!this->handle.has_value()) {
        return;
    }

    os::file_close(this->handle.value());
}

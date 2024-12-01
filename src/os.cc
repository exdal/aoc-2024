#include "os.hh"

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <unistd.h>

auto os::file_open(const fs::path &path, FileAccess access) -> std::expected<FileDescriptor, FileResult> {
    errno = 0;

    i32 flags = O_CREAT | O_WRONLY | O_RDONLY | O_TRUNC;
    if (access & FileAccess::Write)
        flags &= ~O_RDONLY;
    if (access & FileAccess::Read)
        flags &= ~(O_WRONLY | O_CREAT | O_TRUNC);

    i32 file = open64(path.c_str(), flags, S_IRUSR | S_IWUSR);
    if (file < 0) {
        switch (errno) {
            case EACCES:
                return std::unexpected(FileResult::NoAccess);
            case EEXIST:
                return std::unexpected(FileResult::Exists);
            case EISDIR:
                return std::unexpected(FileResult::IsDir);
            case EBUSY:
                return std::unexpected(FileResult::InUse);
            default:
                return std::unexpected(FileResult::Unknown);
        }
    }

    return static_cast<FileDescriptor>(file);
}

auto os::file_close(FileDescriptor file) -> void {
    close(std::to_underlying(file));
}

auto os::file_size(FileDescriptor file) -> std::expected<usize, FileResult> {
    errno = 0;

    struct stat st = {};
    fstat(std::to_underlying(file), &st);
    if (errno != 0) {
        return std::unexpected(FileResult::Unknown);
    }

    return st.st_size;
}

auto os::file_read(FileDescriptor file, void *data, usize size) -> usize {
    u64 read_bytes_size = 0;
    u64 target_size = size;
    while (read_bytes_size < target_size) {
        u64 remainder_size = target_size - read_bytes_size;
        u8 *cur_data = reinterpret_cast<u8 *>(data) + read_bytes_size;

        errno = 0;
        iptr cur_read_size = read(std::to_underlying(file), cur_data, remainder_size);
        if (cur_read_size < 0) {
            break;
        }

        read_bytes_size += cur_read_size;
    }

    return read_bytes_size;
}

auto os::file_write(FileDescriptor file, const void *data, usize size) -> usize {
    u64 written_bytes_size = 0;
    u64 target_size = size;
    while (written_bytes_size < target_size) {
        u64 remainder_size = target_size - written_bytes_size;
        const u8 *cur_data = reinterpret_cast<const u8 *>(data) + written_bytes_size;

        errno = 0;
        iptr cur_written_size = write(std::to_underlying(file), cur_data, remainder_size);
        if (cur_written_size < 0) {
            break;
        }

        written_bytes_size += cur_written_size;
    }

    return written_bytes_size;
}

auto os::file_seek(FileDescriptor file, i64 offset) -> void {
    lseek64(std::to_underlying(file), offset, SEEK_SET);
}

auto os::mem_page_size() -> u64 {
    return 0;
}

auto os::mem_reserve(u64 size) -> void * {
    return mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
}

auto os::mem_release(void *data, u64 size) -> void {
    munmap(data, size);
}

auto os::mem_commit(void *data, u64 size) -> bool {
    return mprotect(data, size, PROT_READ | PROT_WRITE);
}

auto os::mem_decommit(void *data, u64 size) -> void {
    madvise(data, size, MADV_DONTNEED);
    mprotect(data, size, PROT_NONE);
}

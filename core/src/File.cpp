/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/mman.h> // mmap
#include <errno.h>
#include <string.h>
#include <stdio.h> // rename
#include <flux/exceptions>
#include <flux/Mutex>
#include <flux/Random>
#include <flux/Format>
#include <flux/Dir>
#include <flux/Process>
#include <flux/System>
#include <flux/File>

namespace flux {

Ref<File> File::open(String path, int flags, int mode)
{
    int fd = ::open(path, flags, mode);
    if (fd == -1) FLUX_SYSTEM_ERROR(errno, path);
    return new File(path, flags, fd);
}

Ref<File> File::tryOpen(String path, int flags, int mode)
{
    int fd = ::open(path, flags, mode);
    if (fd != -1) return new File(path, flags, fd);
    return 0;
}

Ref<File> File::temp(int openFlags)
{
    String path = createUnique(
        Format("/tmp/%%_%%_XXXXXXXX")
        << Process::execPath()->fileName()
        << Process::currentId()
    );
    return open(path, openFlags);
}

File::File(String path, int openFlags, int fd)
    : SystemStream(fd, (fd >= 0) ? (!::isatty(fd)) : true),
      path_(path),
      openFlags_(openFlags)
{}

String File::path() const
{
    return path_;
}

String File::name() const
{
    const char sep = '/';

    int n = path_->count();
    int i = n - 1;
    while (i >= 0) {
        if (path_->at(i) == sep) {
            ++i;
            break;
        }
        --i;
    }

    String name;
    if (i < n)
        name = path_->copy(i, n);

    return name;
}

int File::openFlags() const
{
    return openFlags_;
}

Ref<FileStatus> File::status() const
{
    return FileStatus::read(fd_);
}

void File::truncate(off_t length)
{
    if (::ftruncate(fd_, length) == -1)
        FLUX_SYSTEM_ERROR(errno, path_);
}

off_t File::seek(off_t distance, int method)
{
    off_t ret = ::lseek(fd_, distance, method);
    if (ret == -1) FLUX_SYSTEM_ERROR(errno, path_);
    return ret;
}

bool File::seekable() const
{
    return ::lseek(fd_, 0, SEEK_CUR) != -1;
}

off_t File::transfer(off_t count, Stream *sink, ByteArray *buf)
{
    if (count == 0) return 0;
    if (!sink) {
        off_t ret = 0;
        if (count > 0) ret = ::lseek(fd_, count, SEEK_CUR);
        else ret = ::lseek(fd_, 0, SEEK_END);
        if (ret != -1) return count;
    }
    return Stream::transfer(count, sink, buf);
}

class MappedByteArray: public ByteArray
{
private:
    friend class File;
    MappedByteArray(char *data, int size):
        ByteArray(data, size, File::unmap)
    {}
};

String File::map() const
{
    off_t fileEnd = ::lseek(fd_, 0, SEEK_END);
    if (fileEnd == -1)
        FLUX_SYSTEM_ERROR(errno, path_);
    size_t fileSize = fileEnd;
    if (fileSize == 0) return "";
    if (fileSize >= size_t(intMax)) fileSize = intMax;
    int pageSize = System::pageSize();
    size_t mapSize = fileSize;
    int protection = PROT_READ | (PROT_WRITE * (openFlags_ & (O_WRONLY|O_RDWR)));
    void *p = 0;
    if (fileSize % pageSize > 0) {
        mapSize += pageSize - fileSize % pageSize;
        p = ::mmap(0, fileSize, protection, MAP_PRIVATE, fd_, 0);
        if (p == MAP_FAILED)
            FLUX_SYSTEM_ERROR(errno, path_);
    }
    else {
        #ifndef MAP_ANONYMOUS
        #define MAP_ANONYMOUS MAP_ANON
        #endif
        mapSize += pageSize;
        p = ::mmap(0, mapSize, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (p == MAP_FAILED)
            FLUX_SYSTEM_DEBUG_ERROR(errno);
        p = ::mmap(p, fileSize, protection, MAP_PRIVATE | MAP_FIXED, fd_, 0);
        if (p == MAP_FAILED)
            FLUX_SYSTEM_ERROR(errno, path_);
    }
    #ifdef MADV_SEQUENTIAL
    if (::madvise(p, mapSize, MADV_SEQUENTIAL) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);
    #endif
    return String(
        Ref<ByteArray>(
            new MappedByteArray(
                reinterpret_cast<char *>(p),
                fileSize
            )
        )
    );
}

void File::unmap(ByteArray *s)
{
    int pageSize = System::pageSize();
    size_t mapSize = s->count();
    if (s->count() % pageSize > 0) mapSize += pageSize - s->count() % pageSize;
    else mapSize += pageSize;
    ::munmap((void *)s->bytes(), mapSize);
}

void File::sync()
{
    if (::fsync(fd_) == -1)
        FLUX_SYSTEM_ERROR(errno, path_);
}

void File::dataSync()
{
#if _POSIX_SYNCHRONIZED_IO > 0
    if (::fdatasync(fd_) == -1)
        FLUX_SYSTEM_ERROR(errno, path_);
#else
    sync();
#endif
}

bool File::access(String path, int flags)
{
    return ::access(path, flags) == 0;
}

bool File::exists(String path)
{
    return (path != "") && access(path, Exists);
}

void File::create(String path, int mode)
{
    int fd = ::open(path, O_RDONLY|O_CREAT|O_EXCL, mode);
    if (fd == -1) FLUX_SYSTEM_RESOURCE_ERROR(errno, path);
    ::close(fd);
}

void File::chown(String path, uid_t ownerId, gid_t groupId)
{
    if (::chown(path, ownerId, groupId) == -1)
        FLUX_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::rename(String path, String newPath)
{
    if (::rename(path, newPath) == -1)
        FLUX_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::link(String path, String newPath)
{
    if (::link(path, newPath) == -1)
        FLUX_SYSTEM_RESOURCE_ERROR(errno, newPath);
}

void File::unlink(String path)
{
    if (::unlink(path) == -1)
        FLUX_SYSTEM_RESOURCE_ERROR(errno, path);
}

void File::symlink(String path, String newPath)
{
    if (::symlink(path, newPath) == -1)
        FLUX_SYSTEM_RESOURCE_ERROR(errno, path);
}

String File::readlink(String path)
{
    String buf = String(128);
    while (true) {
        ssize_t numBytes = ::readlink(path, buf, buf->count());
        if (numBytes == -1)
            return String();
        if (numBytes <= buf->count()) {
            if (numBytes < buf->count())
                buf = String(buf->chars(), numBytes);
            break;
        }
        buf = String(numBytes);
    }
    return buf;
}

String File::resolve(String path)
{
    String resolvedPath = path;
    while (File::unresolvedStatus(resolvedPath)->type() == File::Symlink) {
        String origPath = resolvedPath;
        resolvedPath = File::readlink(resolvedPath);
        if (resolvedPath == "") break;
        if (resolvedPath->isRelativePath())
            resolvedPath = origPath->reducePath()->expandPath(resolvedPath);
    }
    return resolvedPath;
}

String File::createUnique(String path, int mode, char placeHolder)
{
    Ref<Random> random = Random::open();
    while (true) {
        String candidate = path->copy();
        for (int i = 0, n = candidate->count(); i < n; ++i) {
            if (candidate->at(i) == placeHolder) {
                char r = random->get(0, 61);
                if ((0 <= r) && (r <= 9))
                    r += '0';
                else if ((10 <= r) && (r <= 35))
                    r += 'a' - 10;
                else if ((36 <= r) && (r <= 61))
                    r += 'A' - 36;
                candidate->at(i) = r;
            }
        }
        int fd = ::open(candidate, O_RDONLY|O_CREAT|O_EXCL, mode);
        if (fd == -1) {
            if (errno != EEXIST)
                FLUX_SYSTEM_RESOURCE_ERROR(errno, candidate);
        }
        else {
            ::close(fd);
            return candidate;
        }
    }
}

void File::establish(String path, int fileMode, int dirMode)
{
    if (path->contains('/'))
        Dir::establish(path->reducePath(), dirMode);
    if (!File::exists(path))
        File::create(path, fileMode);
}


String File::lookup(String fileName, StringList *dirs, int accessFlags)
{
    Ref<StringList> h;
    if (!dirs) {
        h = Process::env("PATH")->split(':');
        dirs = h;
    }
    String path;
    for (int i = 0; i < dirs->count(); ++i) {
        String candidate = Format() << dirs->at(i) << "/" << fileName;
        if (access(candidate, accessFlags)) {
            path = candidate;
            break;
        }
    }
    return path;
}

Ref<FileStatus> File::status(String path)
{
    return FileStatus::read(path, true);
}

Ref<FileStatus> File::unresolvedStatus(String path)
{
    return FileStatus::read(path, false);
}

String File::load(String path)
{
    establish(path);
    return open(path)->readAll();
}

void File::save(String path, String text)
{
    establish(path);
    Ref<File> file = open(path, File::WriteOnly);
    file->truncate(0);
    file->write(text);
}

FileUnlinkGuard::FileUnlinkGuard(String path):
    path_(path)
{
    if (path_->isRelativePath())
        path_ = path_->absolutePathRelativeTo(Process::cwd());
}

FileUnlinkGuard::~FileUnlinkGuard()
{
    try { File::unlink(path_); } catch (...) {}
}

} // namespace flux

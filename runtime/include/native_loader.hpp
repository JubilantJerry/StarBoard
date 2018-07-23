#ifndef NATIVE_LOADER_H
#define NATIVE_LOADER_H

#include <string>
#include <unordered_map>

#include "data_interface.hpp"

using LibraryHandle = void *;

extern LibraryHandle emptyLibraryHandle;

void dynamicUnload(LibraryHandle handle);

class SmartLibraryHandle {
private:
    LibraryHandle handle;

public:
    SmartLibraryHandle(LibraryHandle handle) noexcept: handle(handle) {}
    ~SmartLibraryHandle() noexcept {
        dynamicUnload(handle);
    }

    SmartLibraryHandle(SmartLibraryHandle const&) = delete;
    SmartLibraryHandle& operator=(SmartLibraryHandle const&) = delete;

    SmartLibraryHandle(SmartLibraryHandle&& other) noexcept
            :handle(other.handle) {

        other.handle = emptyLibraryHandle;
    }

    SmartLibraryHandle& operator=(SmartLibraryHandle&& other) noexcept {
        handle = other.handle;
        other.handle = emptyLibraryHandle;
        return *this;
    }

    LibraryHandle const &get() const {
        return handle;
    }
};

using ModuleFunction = void(*)(DataBlock *);

class NativeLoader {
private:
    std::unordered_map<std::string, SmartLibraryHandle> handles;

public:
    ModuleFunction load(
            std::string const &libraryPath,
            std::string const &libraryName,
            std::string const &functionName);
};

#endif